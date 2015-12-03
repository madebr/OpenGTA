#!/bin/bash
copyright_file=licenses/zlib_header.txt
if [ $# -gt 0 ]; then
  input=$1
  if [ $# -eq 2 ]; then
    copyright_file=$2
  fi
fi
sed '1{h; r $1
  D; }
2{x; G; }' $copyright_file $input >${input}.copy
mv ${input}.copy $input
