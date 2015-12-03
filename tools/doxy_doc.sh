#!/bin/bash

in_file=doc/hacking.txt
out_file=doxy_main.h

function makeM4() {
sed -e "s/ ==*$/', \`/" \
    -e 's/^== /MAINPAGE(`/' \
    -e "s/^= \([^ ]*\)/') \\
PAGE(\`\\1 \\1/" \
    $in_file
echo "')"
}

function convertM4_Doxygen() {
  m4 -DBEGIN_CPP_COMMENT='/*!' -DEND_CPP_COMMENT='*/' -DMAINPAGE='BEGIN_CPP_COMMENT \mainpage $1
$2
END_CPP_COMMENT' -DPAGE='BEGIN_CPP_COMMENT \page $1
$2
END_CPP_COMMENT' -D_='<tt>$1</tt>' -
}

makeM4 | convertM4_Doxygen
