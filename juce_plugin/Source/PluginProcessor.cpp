#include "PluginProcessor.h"

JAHysteresisProcessor::JAHysteresisProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Parameters matching FAUST prototype exactly
    addParameter(inputGainParam = new juce::AudioParameterFloat(
        "input_gain", "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), -7.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    addParameter(outputGainParam = new juce::AudioParameterFloat(
        "output_gain", "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 48.0f, 0.1f), 40.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    addParameter(driveParam = new juce::AudioParameterFloat(
        "drive", "Drive",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), -13.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    addParameter(biasLevelParam = new juce::AudioParameterFloat(
        "bias_level", "Bias Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.62f));

    addParameter(biasScaleParam = new juce::AudioParameterFloat(
        "bias_scale", "Bias Scale",
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f), 11.0f));

    addParameter(modeParam = new juce::AudioParameterChoice(
        "mode", "Bias Resolution",
        juce::StringArray{"K32", "K48", "K60"}, 2)); // Default K60

    addParameter(mixParam = new juce::AudioParameterFloat(
        "mix", "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
}

void JAHysteresisProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize smoothed parameters
    inputGainSmoothed.reset(sampleRate, 0.02);
    outputGainSmoothed.reset(sampleRate, 0.02);
    driveSmoothed.reset(sampleRate, 0.02);
    mixSmoothed.reset(sampleRate, 0.02);

    // Physics parameters matching FAUST prototype
    JAHysteresisScheduler::PhysicsParams physics;
    physics.Ms = 320.0;
    physics.aDensity = 720.0;
    physics.kPinning = 280.0;
    physics.cReversibility = 0.18;
    physics.alphaCoupling = 0.015;

    // Initialize schedulers with default mode (K48)
    auto mode = static_cast<JAHysteresisScheduler::Mode>(modeParam->getIndex());
    schedulerL.initialise(sampleRate, mode, physics);
    schedulerR.initialise(sampleRate, mode, physics);

    // Hardcode to Normal quality (matching FAUST)
    schedulerL.setQuality(JAHysteresisScheduler::Quality::Normal);
    schedulerR.setQuality(JAHysteresisScheduler::Quality::Normal);

    // Set initial bias controls
    schedulerL.setBiasControls(biasLevelParam->get(), biasScaleParam->get());
    schedulerR.setBiasControls(biasLevelParam->get(), biasScaleParam->get());

    // DC blocker: SVF TPT highpass at 10 Hz, Butterworth Q (matching FAUST fi.SVFTPT.HP2)
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 };
    dcBlockerL.prepare(spec);
    dcBlockerR.prepare(spec);
    dcBlockerL.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    dcBlockerR.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    dcBlockerL.setCutoffFrequency(10.0);
    dcBlockerR.setCutoffFrequency(10.0);
    dcBlockerL.setResonance(0.7071);  // 1/sqrt(2) for Butterworth
    dcBlockerR.setResonance(0.7071);
    dcBlockerL.reset();
    dcBlockerR.reset();

    DBG("=== JA Hysteresis C++ prepareToPlay ===");
    DBG("Sample rate: " << sampleRate << " Hz");
    DBG("Mode: " << modeParam->getCurrentChoiceName());
}

void JAHysteresisProcessor::releaseResources()
{
    schedulerL.reset();
    schedulerR.reset();
}

void JAHysteresisProcessor::updateSchedulerSettings()
{
    auto mode = static_cast<JAHysteresisScheduler::Mode>(modeParam->getIndex());
    schedulerL.setMode(mode);
    schedulerR.setMode(mode);

    schedulerL.setBiasControls(biasLevelParam->get(), biasScaleParam->get());
    schedulerR.setBiasControls(biasLevelParam->get(), biasScaleParam->get());
}

void JAHysteresisProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 0 || numSamples == 0)
        return;

    // Update scheduler settings from parameters
    updateSchedulerSettings();

    // Set smoothed target values
    inputGainSmoothed.setTargetValue(juce::Decibels::decibelsToGain(inputGainParam->get()));
    outputGainSmoothed.setTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
    driveSmoothed.setTargetValue(juce::Decibels::decibelsToGain(driveParam->get()));
    mixSmoothed.setTargetValue(mixParam->get());

    // Get channel pointers
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        const float inputGain = inputGainSmoothed.getNextValue();
        const float outputGain = outputGainSmoothed.getNextValue();
        const float drive = driveSmoothed.getNextValue();
        const float mix = mixSmoothed.getNextValue();

        // Left channel
        {
            const float dryL = leftChannel[i];
            double wetL = static_cast<double>(dryL * inputGain * drive);

            // JA hysteresis
            wetL = schedulerL.process(wetL);

            // DC blocker
            wetL = dcBlockerL.processSample(0, wetL);

            // Output gain
            wetL *= outputGain;

            // Dry/wet mix
            leftChannel[i] = static_cast<float>(dryL * (1.0f - mix) + wetL * mix);
        }

        // Right channel (or copy from left if mono)
        if (rightChannel != nullptr)
        {
            const float dryR = rightChannel[i];
            double wetR = static_cast<double>(dryR * inputGain * drive);

            wetR = schedulerR.process(wetR);
            wetR = dcBlockerR.processSample(0, wetR);
            wetR *= outputGain;

            rightChannel[i] = static_cast<float>(dryR * (1.0f - mix) + wetR * mix);
        }
    }
}

juce::AudioProcessorEditor* JAHysteresisProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void JAHysteresisProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = juce::ValueTree("JAHysteresisState");

    state.setProperty("inputGain", inputGainParam->get(), nullptr);
    state.setProperty("outputGain", outputGainParam->get(), nullptr);
    state.setProperty("drive", driveParam->get(), nullptr);
    state.setProperty("biasLevel", biasLevelParam->get(), nullptr);
    state.setProperty("biasScale", biasScaleParam->get(), nullptr);
    state.setProperty("mode", modeParam->getIndex(), nullptr);
    state.setProperty("mix", mixParam->get(), nullptr);

    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void JAHysteresisProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto state = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));

    if (state.isValid())
    {
        *inputGainParam = state.getProperty("inputGain", -7.0f);
        *outputGainParam = state.getProperty("outputGain", 40.0f);
        *driveParam = state.getProperty("drive", -13.0f);
        *biasLevelParam = state.getProperty("biasLevel", 0.62f);
        *biasScaleParam = state.getProperty("biasScale", 11.0f);
        *modeParam = state.getProperty("mode", 2);
        *mixParam = state.getProperty("mix", 1.0f);
    }
}

// Plugin instantiation
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JAHysteresisProcessor();
}
