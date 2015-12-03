#!/bin/bash

sed_term="$1"
shift
while [ $# -gt 0 ]; do
  cp $1 $1.bak
  sed -e "$sed_term" $1.bak >${1}
  rm $1.bak
  shift
done
