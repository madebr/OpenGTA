#!/bin/bash

infile=$1

if [ ! -e $infile ]; then
  echo "gimme a tar.gz"
  exit 1
fi

archive_size=$(stat -c "%s" $infile)

tmpdir=/tmp/work_`basename $0`_$$

mkdir $tmpdir

bzip2 -cd $infile | ( cd $tmpdir ; tar x)

cd $tmpdir/ogta || exit 1

num_files=$(find -type f | wc -l)

num_code_lines=$(find -name "*.cpp" -o -name "*.c" -o -name "*.hpp" -o -name "*.h" \
| xargs wc -l | tail -1 | sed -e 's/^ *//' -e 's/ total//')

filename=$(basename $infile)
filename=${filename%%.tar.bz2}
filename=${filename##ogta_src_}

echo "$filename $archive_size $num_files $num_code_lines"

rm -rf $tmpdir
