#!/bin/bash

function input_data() {
  cat <<EOF
0
1
2
1
0
-1
-2
-1
0
1
2
1
0
-1
-2
-1
0
EOF
}

function transform() {
  switch=$1
  input_data | ./interpolate_test --$switch >gnuplot_data.$$
  cat <<EOF >gnuplot_script.$$
plot "gnuplot_data.$$" title "$switch"
EOF
  gnuplot gnuplot_script.$$ -
}

if [ ! -f interpolate_test ]; then
  g++ -I ../math -o interpolate_test interpolate_test.cpp
fi

transform linear
transform cubic
transform cosine
transform hermite

rm -f gnuplot_data.$$ gnuplot_script.$$
