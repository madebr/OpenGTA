#!/bin/bash
while read a b c d; do
  b=$(echo "$b / 1024" | bc -l)
  d=$(echo "$d / 1000" | bc -l)
  echo "$a $b $c $d"
done
