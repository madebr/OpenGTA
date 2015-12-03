#!/bin/bash

function print_header() {
cat <<EOF
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="$WIDTH" height="$HEIGHT" version="1.1"
  xmlns="http://www.w3.org/2000/svg">
EOF
}

function print_rect() {
  local x=$1
  local y=$2
  local w=$3
  local h=$4
  local col=$5
cat <<EOF
  <rect x="$x" y="$y" width="$w" height="$h"
    style="fill:rgb($col);stroke-width:1;
    stroke:rgb(0,0,0)"/>
EOF
}

WIDTH=200
HEIGHT=200

function print_rnd_col() {
  foo=$(echo "$RANDOM % 4" | bc)
  case $foo in
    0)
      echo "0,0,255";;
    1)
      echo "255,0,0";;
    2)
      echo "0,255,0";;
    3)
      echo "255,255,0";;
  esac
}

if [ $# -gt 0 ]; then
  print_header
fi
while [ $# -gt 0 ]; do
  col=$(print_rnd_col)
  print_rect $1 $2 $3 $4 $col
  shift;shift;shift;shift
done

echo '</svg>'
