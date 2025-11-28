**FULL SPECTRUM MAGNETIZATION**

By SPENCER J. WALTERS / Walters Audio

***This paper introduces Full Spectrum Magnetization (FSM), a method for recreating the magnetic process of analog tape recording in a new magnetic component:***

***the Tape Element.***

**FULL SPECTRUM MAGNETIZATION IS PATENT PENDING.**

# Preface

y journey into the world of analog tape began out of a creative necessity. After listening to *The Dark Side of the Moon* hundreds of times

M

over a few months, I began creating a concept album. I acquired some analog equipment, attempting to get closer to the sound of the 1970s. The missing piece of the puzzle was the tape machine. I acquired a 15 IPS Revox A77 after reading that the Abbey Road engineers would sometimes use one in place of higher quality tape machines.

Of course, the Revox A77 was greatly damaged dur‑ ing shipment. After reading through the service man‑ ual and fixing the electronics, the reels were finally turning—magic was about to happen! …but the result was pitch bending disaster. I spent some time trying to repair the mechanical issues, but eventually I gave up. I tried several alternative options to get the sound of tape, but there was always something off.

At this point, I had spent significant time with the ser‑ vice manual of the Revox A77, where I was exposed to some of the underlying mechanisms of tape recording. I was unsatisfied with my limited understanding, rely‑ ing on hearsay information spread through a decades long game of telephone. I found some analog tape re‑ search papers written by Jay McKnight [1][2], and I be‑ came hooked. I began reading as many papers as I could find, piecing together the analog tape recording system from the engineers who built them. From here, I began looking into how the magnetic process of a tape record‑ ing system could be recreated without the mechanical parts that ended the life of my Revox A77.

The research expanded to magnetic materials. I discov‑ ered that there is an abundance of magnetic cores made with ferric oxide—the same as the particles of magnetic tape. I purchased some ferrite cores and wound them with as many turns as possible to achieve a high induc‑ tance to yield a flat frequency response across the au‑ dio spectrum. One of the transformers actually sounded quite nice.

”My god”, I thought, ”maybe this is it... maybe I’ve recreated *the sound of tape*!”

Unfortunately, it sounded nothing like tape.

My simple transformer and amplifier system didn’t in‑ corporate an ultrasonic bias signal like my Revox A77. I attempted to combine the audio signal with a bias sig‑ nal in a summing amplifier before the transformer, but it didn’t do anything. I realized that everything I had learned about transformers were rules of thumb, but I had no idea how a magnetic core actually worked. How does the signal get transferred from one winding to an‑ other? How does the mere presence of a magnetic core facilitate that?

After reading more research papers on analog tape and re‑learning classical electromagnetism, the inner work‑ ings of analog tape and transformers had become clear. Both processes are rooted in Faraday’s Law of Induction. This is how flux within analog tape is reproduced, and it is how the secondary winding of a transformer ”re‑ produces” flux within a magnetic core. A tape head writes flux into magnetic tape, and a transformer pri‑ mary ”writes” flux into a magnetic core. It became clear that transformers and tape machines are subject to the same laws of physics. It was no longer a matter of *if* a transformer can be made to behave like tape—it was a matter of *how*.

With this revelation, I built the first **Tape Element**. I rebuilt circuitry from the tape machine used to record *The Dark Side of the Moon*, replacing the recording heads and tape with the **Tape Element**. I was able to alter the magnetic transfer using an ultrasonic bias signal and ob‑ serve the behavior of tape as documented in research papers, demonstrating that a tape machine can be built with a stationary magnetic medium.

From here, I created a process to design a **Tape Element** to work with virtually any tape machine. The following discussion is the culmination of my research into ana‑ log tape and transformers, providing a patent pending framework to bridge the two as a unified concept: **Full Spectrum Magnetization**.

![Image: image_001](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_001.png) ![Image: image_002](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_002.png) ![Image: image_003](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_003.png)

**(a)** 1950s Mastering Tape [3][4] **(b)** 1970s Mastering Tape [3][4][5] **(c)** 1990s Mastering Tape [3][4]

![Image: image_004](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_004.png)![Image: image_005](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_005.png)![Image: image_006](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_006.png)

**(d)** MnZn Ferrite **(e)** NiZn Ferrite [6] **(f)** Electrical Steel [7]

**Figure 1.** A comparison of B‑H magnetization curves.

# Introduction

This paper introduces **Full Spectrum Magnetization (FSM)**—a new framework for magnetic signal trans‑ fer. With **FSM**, we’ve created a new class of compo‑ nent capable of magnetizing the full audio frequency spectrum and an ultrasonic bias signal: the **Tape Ele‑ ment**. Drawing on magnetic theory, analog tape record‑ ing, and transformer design, we demonstrate how the **Tape Element** replicates the magnetic transfer charac‑ teristics of analog tape. We apply the standard analog tape recording process of AC bias to a **Tape Element** and compare the results with measurements from ana‑ log tape recorders. A biased **Tape Element** exhibits high‑frequency compression and a reduction in distor‑ tion, showing significant parity with analog tape.

# Magnetic Fields & Hysteresis

We begin our discussion with a brief overview of mag‑ netic fields and magnetic hysteresis.

Magnetic fields can be thought of in terms of two key quantities: an excitation field **H** and a flux density **B**.

## The Excitation Field

An **H**‑field is created whenever electrical current flows through a wire. If the wire is wound into a coil, the mag‑ netic field becomes stronger, especially when the turns are packed closely together. The more turns in the coil and the more current that flows, the stronger the result‑ ing **H**‑field. The strength of this field is also affected by the length of the magnetic path it travels through— spreading the coil over a longer distance weakens the concentration of the field.

![Image: image_007](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_007.png) ![Image: image_008](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_008.png) ![Image: image_009](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_009.png) ![Image: image_010](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_010.png)

**(a)** MnZn Ferrite **(b)** NiZn Ferrite **(c)** 1950s Mastering Tape **(d)** 1970s Mastering Tape

**Figure 2.** Rearranged hysteresis loops with absolute values removed, allowing for comparison between relative hysteresis shapes.

This relationship can be summarized by Equation 1:

*Ni*

**H** = *l ,* (1)

where *N* is the number of turns in the coil, *i* is the cur‑ rent, and *l* is the length of the magnetic path.

## Flux Density & Permeability

Permeability acts as the transfer function between **H** and **B**—it describes how well **H** can *permeate* a magnetic material to create a flux density **B** in it. If a magnetic medium is in the presence of an **H**‑field, a flux density **B** is created in it. The amount of flux density is deter‑ mined by the combination of **H** and the permeability of the material *µ*, given by Equation 2:

**B** = *µ***H***.* (2)

Due to magnetic hysteresis, the relationship between **H** and **B** becomes nonlinear, effectively making *µ* vary with field strength. This non‑linearity can be repre‑ sented by a B‑H curve showing the major hysteresis loop and the initial magnetization curve. Figure 1 shows a representative B‑H curve for analog tape for‑ mulas spanning multiple decades (1a) 1950s (1b) 1970s (1c) 1990s and for materials used in magnetic cores (1d, 1e) ferrites (1f) electrical steel.

Hysteresis loops are further described by the magnetic properties of coercivity, saturation, and remanence.

## Coercivity

Recording media must be capable of storing infor‑ mation, so they should not be easily overwritten or changed. Magnetic *coercivity* **Hc** determines how easy or difficult it is to change the state of magnetism in a magnetic material. **Hc** is the value of **H** when **B** = 0. Ma‑ terials with high coercivity, such as analog tape, have a

wide hysteresis loop and a sharp knee at the beginning of their initial magnetization curve: the *coercive knee*. Conversely, materials used in the cores of transformers and inductors must be capable of changing quickly and favor a lower coercivity with a narrower hysteresis loop. It can be seen in Figure 1 that analog tape has a wider hysteresis loop and higher values of **Hc** than magnetic core materials.

## Saturation

Magnetic *saturation* **Bs** is the point where a negligible increase in **B** occurs as **H** is increased. This is shown in Figure 1 as the leftmost and rightmost portion of all hysteresis plots, where the hysteresis loop converges to a single line with a modest slope. The level of **H** that pushes a material into saturation is the saturation field **Hs**, and the point where saturation begins is the satu‑ ration flux density **Bs**. The onset of saturation is some‑ times defined as the point where permeability decreases by a certain percentage, typically in the range of 5% to 30%.

## Remanence

Magnetic *remanence* **Br** describes how well a material stores information after an external magnetic field is re‑ moved. **Br** is the value of **B** when **H** = 0. The *squareness* of a hysteresis loop is defined as **Br**/**Bs**. In Figure 1, we see a squareness of about: 70% in 1a; 80% in 1b; 90% in 1c; 50% for 1d and 1e; and above 90% in 1f;. Square‑ ness correlates with the sharpness of the *saturation knee*. Lower squareness typically has a gradual onset of satu‑ ration (soft knee), this is observed in the initial magneti‑ zation curves of 1a, 1d, and 1e. Higher squareness has an abrupt onset of saturation (hard knee) as shown in 1c and 1f.

## Relative Coercivity

The units of **H** in the analog tape B‑H curve charts are kA/m, whereas the magnetic core charts are A/m, sev‑ eral orders of magnitude less. From this, we may decide that analog tape and magnetic cores are fundamentally different. Although, if we take a closer look at the hys‑ teresis loops for analog tape, we see that coercivity and remanence increased over time.

If we view coercivity **Hc** as relative to saturation **Hs**, we define *relative coercivity*: **Hr** = **Hc**/**Hs**. This allows us to compare hysteresis shapes independent of absolute field strength. By rearranging the charts of Figure 1, removing the absolute values of **H** and **B**, and normal‑ izing the **B** axes, we obtain Figure 2. We find that al‑ though the absolute **H** field strengths are quite differ‑ ent, the shape of hysteresis from ferrite materials looks as though it precedes the progression of analog tape for‑ mulas, where the relative coercivity and remanence of 1950s tape is somewhere between the NiZn ferrite and 1970s tape, and NiZn is somewhere between the MnZn ferrite and 1950s tape. This suggests NiZn might be‑ have similar to 1950s tape, and MnZn might behave like a tape formula that could have been created before the 1950s, due to its lower relative coercivity and rema‑ nence.

From this observation of the hysteresis loops, we might assume that we can simply send an audio signal through a ferrite core and achieve similar sonics to ana‑ log tape. Unfortunately, it is not so simple—we must understand how hysteresis is harnessed in the analog tape recording process.

# The Analog Tape Recording Process

## Recording

To record a signal onto magnetic tape, an **H**‑field is cre‑ ated in a recording head. The recording head is made from a high permeability material such as permalloy. The recording head has a gap where the **H**‑field escapes [3, pp. 31–35]. When tape passes across the gap, **H** cre‑ ates a flux density **B** in the magnetic particles of the tape coating [3, pp. 63, 67].

In magnetic recording, we typically discuss *wavelength* response instead of *frequency* response. Frequencies are relevant to the tape recorder electronics, but the tape it‑ self operates on wavelengths. For an **H**‑field frequency in the recording head, the speed of the moving tape determines the length of the written flux pattern. The wavelength is calculated by Equation 3:

*s*

*λ* = *f ,* (3)

where *λ* is the wavelength, *s* is the tape speed, and

*f* is the frequency of the **H**‑field. For example, tape moving at 15 IPS (inches per second) across an **H**‑field with a frequency of 20 Hz will create a wavelength of

0.75 in (19.05 mm) and a frequency of 20 kHz creates a wavelength 1000 times shorter: 0.75 mil (19.05 μm). As tape speed is increased, the wavelength is increased for a given **H**‑field frequency. At 30 IPS, the wavelength is twice the size of 15 IPS: 20 Hz yields 1.5 in and 20 kHz yields 1.5 mil. It is important to understand that when we refer to *frequency* response with respect to tape recording, we’re implicitly discussing the *wavelength* re‑ sponse at a fixed tape speed [8].

As described in the *Coercivity* section, the hysteresis loop of analog tape (Figure 1b) is wide, yielding a sharp coercive knee in the initial magnetization curve. Fig‑ ure 3a shows a 1 kHz sine wave with an amplitude greater than **Hc** overlaid on the hysteresis curve. If we imagine the sine wave moving vertically through the hysteresis curve, it becomes evident that the sine wave would be greatly affected by the sharp knee—low lev‑ els of the sine wave result in drastically lower output compared to higher levels of the sine wave. The result‑ ing sound is bursts of energy appearing whenever the signal passes the sharp knee.

To overcome this sharp knee, the audio signal is com‑ bined with a bias signal, which has a frequency typi‑ cally greater than 20 kHz—often at least 3 times greater than 20 kHz—making it undetectable by the human ear [8][3]. Figure 3b shows a 100 kHz bias superimposed on a 1 kHz sine wave. The effective zero‑crossing of the 1 kHz sine wave has been shifted to a substantially linear portion of the magnetization curve. The bias signal has the same amplitude as the 1 kHz sine wave in Figure 3a, overcoming the coercivity of the tape. This allows the 1 kHz sine wave to have a lower amplitude while oper‑ ating in the linear portion, significantly reducing distor‑ tion created by the sharp knee or saturation.

This process is referred to as *AC Bias*. It is the standard method of recording audio onto analog tape. This pro‑ cess enables low distortion recordings to be made, but it does not come without caveats. Bias is typically set around maximum sensitivity for low frequencies (e.g. 700 Hz to 1000 Hz), but the maximum sensitivity of high frequencies peaks earlier than low frequencies due to bias erasure [3, pp. 75]. The result is high frequency loss when bias is adjusted for the lowest distortion. A ”flat” response is achieved by increasing high frequen‑ cies with equalization in the record preamplifier [9].

## Tape Compression

It was later discovered that the high frequency loss cre‑ ated by AC Bias is not static, and equalization alone is insufficient to provide a flat recording response. Jensen and Pramanik demonstrated that bias induced high fre‑ quency loss is dynamic [10]. The magnetic medium

![Image: image_011](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_011.png) ![Image: image_012](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_012.png)

**(a)** 1 kHz coercive field sine wave moving through tape hysteresis. **(b)** 100 kHz bias superimposed on a 1 kHz sine wave moving through

tape hysteresis.

**Figure 3.** Mastering tape (a) without bias (b) with AC bias

doesn’t ”know” if high frequencies are from an inten‑ tional ultrasonic bias signal or from high frequencies in the audio signal, so it treats them all as valid sources of AC bias. Thus, when an audio source contains sig‑ nificant high frequency and low frequency content (e.g. snare drum, picked acoustic guitar), the high frequen‑ cies of the audio source sum with the ultrasonic bias signal, giving an additional increase to the spectrum be‑ low the high frequency audio content. This doesn’t just apply to what we typically define as ”high frequencies” in the audio world. This phenomenon occurs across the full audio spectrum. For example, if the bias signal has a frequency 3 times greater than the source: 10 kHz biases 3 kHz, 3 kHz biases 1 kHz, 1 kHz biases 300 Hz, 300 Hz biases 100 Hz, 100 Hz biases 30 Hz, and in between, all frequencies are biasing lower frequencies, raising their effective operating level.

Because of this, a truly flat flux frequency response is not achievable with a fixed amplitude bias signal, The frequency response is constantly bending towards lower frequencies. The effect of this is what we often call ”tape compression”, where high frequencies sound compressed, but from the above, we see that it is actu‑ ally low frequencies increasing in level which gives a perceived level reduction to high frequencies.

## Reproducing

At a later point in time, flux stored in tape can be re‑ produced by moving the tape across a reproduce head. Like the record head, the reproduce head has a gap, which is required for the flux to travel to the coil of the head [8]. The flux difference in the gap induces a volt‑ age in the coil, following Faraday’s Law of Induction in Equation 4:

*V* = *−N d*Φ*B ,* (4)

*dt*

where *V* is the electromotive force, *d*Φ*B* is the change in magnetic flux over time, and *N* is the number of turns in the coil. Higher frequencies have a greater rate of change compared with lower frequencies, which causes them to be reproduced at a greater level. Because of this, the voltage induced in the reproduce head increases 6 dB per octave across the audio range. The reproduce head is connected to an integrator circuit, which re‑ stores the flux originally recorded to the tape.

*dt*

## High Frequency Response

Due to spatial magnetic limitations of the reproduce head, there is a limit to short wavelength reproduction. Voltages are induced by the flux difference across the

![Image: image_013](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_013.png) ![Image: image_014](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_014.png)

**(a)** 30 IPS **(b)** 15 IPS

**Figure 4.** Mastering Tape Frequency Response (a) 30 IPS (b) 15 IPS

![Image: image_015](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_015.png)![Image: image_016](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_016.png)

**(a)** 7.5 IPS **(b)** 3.75 IPS

**Figure 5.** Mastering Tape Frequency Response (a) 7.5 IPS (b) 3.75 IPS

![Image: image_017](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_017.png) ![Image: image_018](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_018.png)

**(a)** 30 IPS **(b)** 15 IPS

**Figure 6.** Theoretical head bump (a) 30 IPS (b) 15 IPS

gap, so when the wavelength equals the gap length of the reproduce head, no voltage is induced and Equa‑ tion 4 yields zero. There is a gradual reduction in level as short wavelengths approach the gap length. Inter‑ estingly, the gap does not act as a simple low pass fil‑ ter. Wavelengths smaller than the gap width actually increase in amplitude until the wavelength is 1.5 times the gap length, and the amplitude falls off again until the wavelength is 2 times the gap length, where the level becomes zero again. The behavior of gap loss can be de‑ scribed as a series of notches at integer multiples of the gap length. If the reproduce gap length is much smaller than the smallest audio wavelength at the relevant tape speeds, the short wavelength loss is well outside of the audio range, and is not a contributing factor to high fre‑ quency loss [3][1].

There are additional sources of high frequency loss [3,

pp. 77], but if a studio grade tape machine has proper mechanical maintenance, these sources are also insignif‑ icant. As noted by McKnight [1], the bulk of high fre‑ quency loss is due to the thickness of the tape coating [11].

For studio grade mastering tape running at 30 IPS, the theoretical frequency response can be plotted as in Fig‑ ure 4a. We see a deviation from the ideal response due to the thickness loss. The integrated reproduce head re‑ sponse shows about 10 dB of attenuation at 20 kHz. The

high frequency loss is greater as tape speed is decreased, as shown in Figures 4b, 5a, and 5b. Higher tape speeds yield a frequency response closer to ideal, so one can imagine that a theoretically infinite tape speed would yield the ideal response. Unfortunately, increasing the tape speed decreases the level of low frequencies, so a compromise must be made.

## Low Frequency Response

The reproduce head creates low frequency response changes, which are both additive and subtractive as shown in Figure 6. This low end response is typically referred to as the *head bump*. The spectral shape of the head bump is a series of peaks and valleys at integer multiples of the lowest frequency node, and a steep roll off occurs below the lowest node [8][12][3]. This is caused by the physical structure of the reproduce head, where the poles wound with coils act like an additional magnetic gap inducing a voltage from a flux difference. This induced voltage adds and subtracts from the volt‑ age induced in reproduce gap. The peaks and valleys are created from a series of harmonically spaced notches following the same phenomenon that causes the repro‑ duce gap loss. On the peaks, the wavelength is an inte‑ ger multiple of 1/2 the distance of the poles. On the val‑ leys, the wavelength is an integer multiple of distance of the poles.

## Analog Tape Summary

We have provided a brief overview of the tape recording process from a magnetic **H**‑field in the recording head to a flux Φ*B* in moving tape inducing a voltage in the repro‑ duce head. It should be noted that frequency response changes in the reproduce stage are linear in nature, and should not impart distortion on the signal. Contribu‑ tors to non‑linearity in the reproduce stage are mostly confined to (a) hysteresis in the reproduce head, which is typically insignificant due to a low voltage induced in high permeability permalloy heads (b) inconsistent tape speed, causing wavelength to frequency errors, of‑ ten described as ”wow and flutter” (c) and electronics of the reproduce amplifier. The record stage is the source of significant non‑linearity, due to hysteresis of the mag‑ netic tape. AC bias mitigates this while creating ”tape compression”.

# Transformers

![Image: image_019](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_019.png)

**Figure 7.** Schematic of a transformer with annotations for the location of the **H**‑field, **B**‑field, and induced voltage.

Transformers are electromagnetic components built by winding a magnetic core with at least two windings. Re‑ ferring to Figure 7, an **H**‑field is generated in the pri‑ mary coil winding, which creates a flux density **B** in the magnetic core. The changing flux in the core Φ*B* induces a voltage **V** in the secondary winding via Faraday’s Law of Induction.

One might notice that a transformer operates on the same magnetic principles as analog tape recording, where the primary winding creates an **H**‑field like a recording head, a flux density **B** is created in the mag‑ netic core like magnetic tape, and a voltage **V** is induced in the secondary winding like a reproduce head.

The induction (reproduce) stage of a transformer is closer to the ideal 6 dB per octave response with rising frequencies, because it does not have the reproduce is‑ sues of tape recording that contribute to high frequency

loss or the low frequency head bump. The induction stage of transformers stray from the ideal response due to leakage inductance and winding capacitance which together act as a low pass filter [13]. Audio transformers can be designed for a substantially flat high frequency response, so for the purposes of this discussion, we will ignore parasitic reactance.

As noted earlier, a magnetic core material can have a rel‑ ative coercivity similar to magnetic tape, so one might wonder why transformers don’t sound like unbiased tape. The reason: transformers are built to achieve a fun‑ damentally different goal than magnetic storage, lead‑ ing to designs that optimize the opposite parameters of magnetic recording.

## Audio Transformer Design

**![Image: image_020](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_020.png)**

**Figure 8.** Magnetic frequency response of a typical audio transformer.

Figure 7 shows the key parameters of transformer de‑ sign: *R*, *LP* , *LS*. When designing an audio transformer, a flat frequency response across the audio spectrum is desired. The voltage induced in the secondary winding is the *change* in flux, which favors high frequencies over low frequencies, so transformers must be designed to create an **H**‑field which decreases across the audio spec‑ trum.

The magnetic frequency response of a typical audio transformer is shown in Figure 8. The magnetic **H**‑field in the primary winding of the transformer is approxi‑

mately a 6 dB per octave low pass filter across the au‑ dio spectrum. The voltage induced in the secondary is the derivative, acting as a 6 dB per octave high pass filter, yielding a mostly flat frequency response with a slight dip at the lowest frequencies. The ‑3 dB corner frequency *fc* is same for both the low pass **H** and the high pass *V* , calculated by Equation 5:

cal steel, so we show the representative hysteresis loop for this material. Lower frequencies (200 Hz) can tra‑ verse a larger portion of the non‑linear hysteresis curve than higher frequencies (2 kHz, 20 kHz). In fact, 20 kHz creates a practically non‑existent level of **H**.

If a tape machine recorded the **H**‑field of a typical trans‑ former, the result would be a muffled recording due to the decreasing field strength across the audio spectrum.

*R*

**fc** =

2*πLP*

*,* (5)

For this reason, standard audio transformers do not be‑ have like tape, and furthermore, they can not use AC bias due to the very low field strength at ultrasonic fre‑

where *fc* is the corner frequency, *R* is the resistance, and

![Image: image_021](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_021.png)*LP* is the inductance of the primary winding. In the ex‑ ample transformer of Figure 8, *R* = 13.2 Ω, *L* = 300 mH, giving a corner frequency of 7 Hz. The resistance *R* is actually caused by the intrinsic resistance of the copper wire used to wind the magnetic core, referred to as di‑ rect current resistance (DCR). This is typically not a sep‑ arate resistor component—the resistance is part of the transformer.

quencies.

## Current Tape Emulators

![Image: image_022](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_022.png)**Figure 9. H**‑field strength of increasing frequencies in a typi‑ cal audio transformer

Transformers used in audio equipment are designed for this kind of magnetic frequency response. This type of transformer creates distortion that gradually decreases as frequency increases. The reason for this frequency de‑ pendent distortion can be understood by overlaying the amplitudes of multiple frequencies on a hysteresis chart as shown in Figure 9. A material commonly used in this kind of audio transformer is grain oriented electri‑

**Figure 10.** Magnetic frequency response of transformers used in current tape emulators.

There are existing devices which use transformers as a means to attempt emulating tape, but the transformers of these devices are similar to typical audio transform‑ ers. Referring to Equation 5, in these devices, the value of *R* is increased to shift the corner frequency inside the audio range. From Equation 1, this reduces the level of **H** as higher resistance *R* yields lower current *i*. To com‑ pensate for this, the size of the transformer is reduced for a lower value of *l*, providing a marginal increase to

**H**. Unfortunately, the resulting **H**‑field strength is still low and unable to span the full range of hysteresis of the core material.

The magnetic frequency response measured from this

variety of tape emulator transformer is shown in Fig‑ ure 10 where we observe that the **H**‑field is now flat until around 100 Hz. Some of these devices may employ pre‑ emphasis before the transformer to extend the range of the flat **H**‑field response, but this would require a high shelf with approximately 38 dB of gain starting around 100 Hz to provide a truly flat response. To make mat‑ ters worse, pre‑emphasis circuitry is unable to supply additional **H**‑field, and only aids in extending the flat response at low levels of **H**.

Looking at the hysteresis of per‑frequency amplitudes in Figure 11, we see similar results to the audio trans‑ former of Figure 9. We see a reduction in overall **H**‑field level due to the reduced current, and similar to the stan‑ dard audio transformer, the **H**‑field level at 20 kHz is practically non‑existent, making it impossible to apply AC bias.

![Image: image_023](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_023.png)

**Figure 11. H**‑field strength of transformers used in current tape emulators.

To illustrate further, the per‑frequency amplitudes are shown for analog tape in Figure 12—all frequencies in the audio range have a similar **H**‑field strength. From our analysis, we found that the current offerings behave more like transformers than tape, due to a frequency de‑ pendent **H**‑field, low levels of magnetic flux, and inabil‑ ity to recreate the process of AC bias.

![Image: image_024](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_024.png)

**Figure 12. H**‑field strength of analog tape.

# Full Spectrum Magnetization

For a transformer to behave like analog tape, one must fundamentally rethink transformer design. Traditional transformers are incapable of magnetizing the full au‑ dio spectrum, so we require a new component which *can*.

We introduce **Full Spectrum Magnetization (FSM)**, a patent pending magnetic framework which reimagines transformers and inductors at the deepest level. With **FSM**, we have the means to magnetize the full audio spectrum in a magnetic core. By applying **FSM** to trans‑ former design, we created the **Tape Element** to take the place of magnetic tape and recording heads in stan‑ dard tape machine circuitry, permitting the creation of maintenance‑free tape machines for the modern era.

## Magnetic Frequency Response

The magnetic frequency response of a **Tape Element** is shown in Figure 13. Comparing with Figure 4, we see the same frequency independent **H**‑field, but the **Tape Element** does not have the reproduce frequency response losses of tape, which allows it to obtain the ideal reproduce response. To obtain a flat frequency response, the secondary windings of the **Tape Element** are connected to an integrator, mirroring the necessity to connect integrator circuitry to a reproduce head in analog tape recording.

![Image: image_025](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_025.png)

**Figure 13.** Magnetic Frequency Response of the Tape Element

The per‑frequency amplitudes of a **Tape Element** are shown in Figure 14. Comparing with analog tape from Figure 12, we see the same relative **H**‑field strength across all frequencies.

![Image: image_026](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_026.png)

**Figure 14. H**‑field strength of the Tape Element.

## AC Bias

At this point, we’ve built a component that can magne‑ tize the full audio spectrum, but we haven’t recreated the standard process used to obtain high fidelity record‑ ing and tape compression: AC bias. We’ve designed the **H**‑field response of the **Tape Element** to remain flat up to the frequency of an ultrasonic bias signal, so we sim‑ ply combine the ultrasonic bias signal with the audio signal and send the composite signal to the **Tape Ele‑ ment**.

The relative **H**‑field strength in the **Tape Element** hys‑ teresis loop (Figure 15) matches what we see in tape recording (Figure 3b). The zero‑crossing of a 1 kHz sine wave is shifted to a more linear portion of the hystere‑ sis loop through a 100 kHz bias signal, mitigating dis‑ tortion caused by the coercive knee of hysteresis in the **Tape Element**.

# Measurements

The effect of AC Bias on a **Tape Element** is best under‑ stood by measurements taken directly from a **Tape Ele‑ ment** in operation.

![Image: image_027](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_027.png) ![Image: image_028](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_028.png)

**Figure 15.** AC bias applied to the Tape Element.

## Sensitivity

In analog tape recording, *sensitivity* refers to the output level as a function of bias level for a single‑tone input signal. As noted by McKnight [9], as the level of bias is increased, the output level increases until maximum sensitivity is reached and after this point, additional in‑ creases to the bias level reduce the output level. Apply‑ ing AC bias to a magnetic medium results in frequency response changes, where higher frequencies experience less of a level increase than lower frequencies. This has been demonstrated for analog tape [9], and this phe‑ nomenon has been measured in the **Tape Element** as shown in Figure 16.1

We see that 20 kHz is approximately 2 dB less than 1 kHz at maximum sensitivity. Additionally, we see that increasing bias past maximum sensitivity actually restores high frequencies, whereas analog tape sees deeper high frequency losses as bias is increased past maximum sensitivity, due to bias erasure. This shows that the **Tape Element** does not experience bias erasure, which allows for adjusting the bias signal to levels that would sacrifice high frequency content in analog tape, thus providing a greater range of magnetic control.

This high frequency loss is corrected with high fre‑ quency equalization before the **Tape Element**, analo‑

1See McKnight’s Figure 2C in [9], which isolates the effect of AC bias on frequency response by excluding losses from sources such as tape thickness or reproduce head gap.

**Figure 16.** Tape Element Sensitivity. Output level vs. AC Bias level.

gous to record high frequency equalization used in me‑ chanical tape machines to restore high frequencies atten‑ uated from bias.

## Magnetic Compression

The high frequency behavior in the **Tape Element** is not simply a static attenuation though. As discussed in the *Tape Compression from AC Bias* section, Jensen and Pramanik [10] provided a solution to a ”problem” in analog tape recording, where a flat frequency response was not achievable due to high frequencies in the au‑ dio signal acting as additional bias signals. This prob‑ lem was discovered through a process used to calibrate tape recorders in an assembly line: a multi‑tone signal with four frequencies (as shown in Figure 17) is applied to the cassette recorder. The equalization of the cas‑ sette recorder is set to obtain the flattest response for the multi‑tone signal. The result should be a cassette recorder with a substantially flat frequency response.

Unfortunately, this method of calibration does not work. After running a sine sweep on the cassette recorder, where the input signal contains only one tone instead of four, the resulting frequency response measurement showed that high frequencies had a higher magnitude than lower frequencies. The multi‑tone signal showed a relatively flat response, but the sine sweep had a high frequency boost.

![Image: image_029](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_029.png)

**Figure 17.** Multi‑tone Filter Test Input Signal

From this, we see contradictory frequency response re‑ sults depending on the method used to calibrate or mea‑ sure a tape machine. This was determined to be a side effect of biasing a magnetic medium. High frequencies of the audio signal act as additional bias signals which sum with our intentional ultrasonic bias signal. This provides a source dependent level increase to lower fre‑ quencies. We described this phenomenon in greater de‑ tail in the *Tape Compression from AC Bias* section.

The multi‑tone signal test was performed on the **Tape Element** used in the **T805** as shown in Figure 18. The multi‑tone signal is comprised of 100 Hz, 6.3 kHz, 16 kHz, and 20 kHz, where all frequencies have the same level (Figure 17). The **T805** is calibrated for the level of 20 kHz to equal 100 Hz (Figure 18a). Next, a sine sweep frequency response is collected for the **T805** (Fig‑ ure 18b). We find that although the multi‑tone signal shows that the level of 20 kHz equals 100 Hz, the sine sweep test shows that 20 kHz is about 2 dB greater than 100 Hz.

If we instead calibrate the **Tape Element** with a single‑ tone, where 100 Hz and 20 kHz are set to the same level at a different times, we see similar results. The multi‑tone signal shows 20 kHz attenuated by about 2 dB (Figure 18c), and the sine sweep shows 20 kHz is equal to 100 Hz (Figure 18d). These charts mirror data collected in analog tape recorders [10], but this test was performed with a **Tape Element** instead of analog tape.

This demonstrates that a **Tape Element** provides the source dependent frequency response of analog tape recording, commonly known as *tape compression*, which is desired for its ability to naturally soften transients. This is especially useful for sources with significant high frequencies such as vocal sibilance, cymbals, or steel string guitar picking. To be frank, the results of this test are remarkable: one of the most sought‑after aspects of analog tape has been recreated in a new form requiring no moving parts: the **Tape Element**.

## Distortion

As noted by McKnight [9], applying AC bias to a mag‑ netic medium affects the distortion spectrum: overall distortion is reduced, and even‑order distortion in the bias signal will create even‑order distortion in the audio signal.2

Figure 19 shows the distortion spectrum for harmonics 2 to 7 with the **H**‑field level set for the THD at 1 kHz to equal 1%.

When no bias signal is used (Figure 19a), the **Tape El‑ ement** creates significant odd‑order distortion in har‑ monics 3, 5, 7 and distortion at high frequencies is sim‑ ilar to low frequencies. With a 100 kHz bias signal ad‑ justed for maximum sensitivity (Figure 19b), even‑order distortion becomes more prominent. The rise in even‑ order distortion reflects harmonic components in the bias signal itself, which are transferred to the audio sig‑ nal through the nonlinearity of the magnetic core. Ad‑ ditionally, odd‑order distortion has been significantly reduced such that the 7th harmonic is below the noise floor, and the amount of distortion is greater at low fre‑ quencies than high frequencies.

Referring to Figure 20, we present these measurements in a standard 1 kHz 1% THD chart. With no bias (Fig‑ ure 20a), we see odd distortion extending to the high‑ est audio frequencies. Conversely, when a 100 kHz bias signal is set for maximum sensitivity (Figure 20b), odd harmonics are reduced significantly such that the high‑ est measurable odd harmonic is the 5th harmonic, and even harmonics become dominant.

The distortion measurements show that the **Tape El‑ ement** exhibits the same kind of bias dependent dis‑ tortion behavior previously measured in analog tape recording [9]. These changes to the distortion spectrum are the result of applying a 100 kHz bias signal to the **Tape Element** as shown in Figure 15. By providing this bias control, a recording engineer is empowered to di‑ rectly control the transfer characteristics of a magnetic material in real‑time.

2Even‑order distortion is not commonly dominant in tape recorders, because even harmonics in the bias signal were seen as a ”bad effect”, leading to bias oscillator designs that minimize even‑ order distortion.

![Image: image_030](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_030.png) ![Image: image_031](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_031.png)

**(a)** Multi‑tone Output (Multi‑tone Calibration) **(b)** Sine Sweep Frequency Response (Multi‑tone Calibration)

![Image: image_032](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_032.png)![Image: image_033](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_033.png)

**(c)** Multi‑tone Output (Single‑tone Calibration) **(d)** Sine Sweep Frequency Response (Single‑tone Calibration)

**Figure 18.** Multi‑tone Output Signal and Sine Sweep Frequency Responses, demonstrating the dynamic frequency response of a Tape Element.

![Image: image_034](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_034.png) ![Image: image_035](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_035.png)

**(a)** Without AC Bias **(b)** With AC Bias

**Figure 19.** T805 Tape Element Distortion Spectrum (1% THD at 1 kHz)

![Image: image_036](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_036.png)![Image: image_037](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_037.png)

**(a)** Without AC Bias **(b)** With AC Bias

**Figure 20.** T805 Tape Element 1 kHz 1% THD

![Image: image_038](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_038.png)

![Image: image_039](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_039.png)![Image: image_040](./fsm%20-%20full%20spectrum%20magnetization%20WORD_images/image_040.png)

**(a)** No Bias **(b)** Maximum Sensitivity Bias **(c)** +1.5 dB Bias

**Figure 21.** Tape Element 1 kHz Hysteresis Loop

## Hysteresis Loops

To further illustrate the effect of AC bias on a **Tape Ele‑ ment**, we provide the 1 kHz hysteresis loop measured from a **Tape Element** in Figure 21. This hysteresis loop was measured with an oscilloscope in XY mode where the X probe is connected to the input of the **T805** and the Y probe is connected to the output.

When no AC bias is applied, the major hysteresis loop of the magnetic material is measured (Figure 21a). The width of the hysteresis loop shows the substantial rela‑ tive coercivity **Hr** of the material. When the bias signal is adjusted for maximum sensitivity (Figure 21b), the width of the hysteresis loop is reduced, yielding a re‑ duction to the effective coercivity. As bias is increased past maximum sensitivity, the hysteresis loop width is reduced further, yielding a narrow hysteresis loop with marginal levels of effective coercivity (Figure 21c).

These measurements are taken directly from the **Tape Element** of the **T805**. This demonstrates the reduction in effective coercivity from shifting the zero‑crossing of a 1 kHz sine wave with a 100 kHz bias signal, as repre‑ sented in Figure 15. This further mirrors the behavior of analog tape recording [3].

Additionally, these hysteresis measurements demon‑ strate that the **T805** is able to generate an **H**‑field capable of spanning the full range of non‑linear hysteresis in the **Tape Element**.

# Conclusion

From the foregoing discussion, it is evident that the magnetic process of analog tape recording can be ap‑ plied to a **Tape Element** designed with **FSM**, yielding re‑ sults remarkably similar to analog tape without requir‑ ing moving parts.

By providing controls for the bias level, the **H**‑field level,

and high frequency equalization, the recording engi‑ neer is empowered to fine tune a **Tape Element**’s mag‑ netic behavior at the deepest level.

Our physics‑based approach with **FSM** enables the cre‑ ation of maintenance‑free tape machines, from recre‑ ations of classic recorders of the past to designs yet‑to‑ come. The first device to utilize our patent pending **Full Spectrum Magnetization** is the **T805**.

The **T805** contains a recreation of the Record Driver and Bias Oscillator circuitry from a 1970s Swiss tape machine—the same machine used to record *The Dark Side of the Moon*, which inspired this investigation and invention.

With **Full Spectrum Magnetization**, the ”*sound of tape*” is no longer a relic of the past.

# References

## [1] Jay McKnight. Calculating the Wavelength Response of the Magnetic Recording and Reproducing Processes – a His‑ torical Review. 2017.
## [2] Jay McKnight. Some Popular Misconceptions About Magnetic Recording History and Theory. 2012.
## [3] Charles E. Lowman. Magnetic Recording. McGraw‑Hill Inc., 1972.
## [4] Delos A. Eilers. 3M Audio Open Reel Tapes, in SI Units. 2010.
## [5] David R. Mills. “THE NEW GENERATION OF HIGH ENERGY RECORDING TAPE”. In: Recording Engineer Producer Magazine 5.6 (1974).
## [6] Fair‑Rite Products Corp. 61 Material Data Sheet. url: https://fair-rite.com/61-material-data-sheet/.
## [7] Stan Zurek. “Qualitative analysis of Px and Py components of rotational power loss”. In: IEEE Transactions on Magnetics (2014).
## [8] Ampex Engineering Department. Basic Concepts of Magnetic Tape Recording. Tech. rep. Ampex Corporation, 1960.
## [9] John G. McKnight. “BIASING IN MAGNETIC TAPE RECORDING”. In: Electronics World (1967).
## [10] J. Selmer Jensen and S.K. Pramanik. DYNAMIC BIAS CONTROL WITH HX PROFESSIONAL. 1984.
## [11] Neal Bertram. “Wavelength Response in AC Biased Recording”. In: IEEE Transactions on Magnetics (1975),

pp. 1176–1178.

## [12] Jack Endino. The Unpredictable Joys of Analog Recording. 2000. url: https://www.endino.com/graphs/.
## [13] Bill Whitlock. Handbook For Sound Engineers 3rd Edition. Ed. by Glen M. Ballou. Focal Press, 2001. Chap. Audio Transformers.

# REVISION HISTORY

- July 20, 2025: typo fixes; minor formatting changes; replaced ”retentivity” with ”remanence”; updated the footer publication month to July to reflect the actual publication month.
- July 11, 2025: initial release.