#!/bin/bash

INPUT=slope1_data.h

function fetch_data () {
  local k=$1
  sed -n "/{ \/\/ slope: $k$/,/^\},/p" $INPUT
}

fetch_data 0
fetch_data 3
fetch_data 4
fetch_data 1
fetch_data 2
for i in `seq 5 8`; do
  fetch_data $i
done
for i in `seq 17 24`; do
  fetch_data $i
done
for i in `seq 9 16`; do
  fetch_data $i
done
for i in `seq 25 40`; do
  fetch_data $i
done
fetch_data 42
fetch_data 41
fetch_data 43
fetch_data 44
