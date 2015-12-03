#!/bin/bash

cat <<EOF
<html>
<body>
<h4>side textures of style001.g24 - slightly wrong palette</h4>
EOF

for idx in `seq 0 50`; do
  clut=$(./g24 STYLE001.G24 $idx style1_side${idx}.bmp | grep clut-idx)
  convert style1_side${idx}.bmp style1_side${idx}.png
  rm style1_side${idx}.bmp
  echo "<p><img src=\"style1_side${idx}.png\"><br> style 001, side $idx ; found $clut</p>"
done

cat <<EOF
</body>
</html>
