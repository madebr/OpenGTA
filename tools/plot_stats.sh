#!/bin/bash

if [ ! -e stats ]; then
  echo "start me in the archive dir after creatings 'stats'"
  exit 1
fi

sh ../tools/stats_recalc.sh < stats > stats2
gnuplot <<EOF
set timefmt "%Y-%m-%d"
set xdata time
set format x "%Y-%m-%d"

set yr[0:260]
set ytics 20
set y2tics 0,10,40
set grid ytics y2tics

plot "stats2" using 1:2 title "total size [KB]" with points 1 2
replot "stats2" using 1:2 notitle with lines 1
replot "stats2" using 1:3 title "files" with points 2 2
replot "stats2" using 1:3 notitle with lines 2
replot "stats2" using 1:4 title "code lines [K]" with points 7 2
replot "stats2" using 1:4 notitle with lines 7

set output "stats_plot.svg"
set terminal svg size 800 400

replot
EOF
../tools/replace_in_files.sh "s/black/silver/g" stats_plot.svg
inkscape -e stats_plot.png stats_plot.svg

rm stats2 stats_plot.svg
