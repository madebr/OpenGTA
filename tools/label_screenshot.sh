#!/bin/bash

in=$1

out=$1.new


convert $in -fill white  -box '#00000080' \
  -gravity North -annotate +0+10 ' ~Scumbag City~ by Pennywise ' \
    -pointsize 20 $out
