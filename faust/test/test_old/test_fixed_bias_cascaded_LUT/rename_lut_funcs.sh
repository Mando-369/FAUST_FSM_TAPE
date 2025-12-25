#!/bin/bash
cd "$(dirname "$0")"

for bias in 01 02 03 04 05 06 07 08 09; do
  file="ja_lut_k29_bias_${bias}.lib"
  echo "Renaming functions in $file..."
  sed -i '' "s/ja_lut_k29_/ja_lut_k29_bias_${bias}_/g" "$file"
  sed -i '' "s/ja_lookup_m_end_k29/ja_lookup_m_end_k29_bias_${bias}/g" "$file"
  sed -i '' "s/ja_lookup_sum_m_rest_k29/ja_lookup_sum_m_rest_k29_bias_${bias}/g" "$file"
done

echo ""
echo "Done. Verifying function names:"
for bias in 01 05 09; do
  echo "  bias_${bias}: $(grep 'ja_lookup_m_end_k29' ja_lut_k29_bias_${bias}.lib | head -1 | cut -d'(' -f1)"
done
