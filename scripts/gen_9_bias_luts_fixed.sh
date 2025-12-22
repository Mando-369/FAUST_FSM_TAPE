#!/bin/bash
cd "$(dirname "$0")"
OUTPUT_DIR="../faust/test/test_fixed_bias_cascaded_LUT"

# Generate 9 LUTs with bias levels 0.1, 0.2, 0.3 ... 0.9
for i in 1 2 3 4 5 6 7 8 9; do
  level="0.${i}"
  padded=$(printf "%02d" $i)
  echo "=== Generating K29 bias $level ==="
  python3 generate_ja_lut.py --mode K29 --bias-level $level --h-range -40 40 --h-size 513 --m-size 129 --interpolation bilinear --output-dir "$OUTPUT_DIR"

  # Rename file and all function names inline
  outfile="$OUTPUT_DIR/ja_lut_k29_bias_${padded}.lib"
  mv "$OUTPUT_DIR/ja_lut_k29.lib" "$outfile"
  sed -i '' "s/ja_lut_k29_/ja_lut_k29_bias_${padded}_/g" "$outfile"
  sed -i '' "s/ja_lookup_m_end_k29/ja_lookup_m_end_k29_bias_${padded}/g" "$outfile"
  sed -i '' "s/ja_lookup_sum_m_rest_k29/ja_lookup_sum_m_rest_k29_bias_${padded}/g" "$outfile"

  rm -f "$OUTPUT_DIR/JAHysteresisLUT_K29.h" 2>/dev/null
  echo "  -> ja_lut_k29_bias_${padded}.lib (bias=$level)"
done

echo ""
echo "=== All 9 LUTs generated with CORRECT bias levels ==="
ls -la "$OUTPUT_DIR"/ja_lut_k29_bias_*.lib
