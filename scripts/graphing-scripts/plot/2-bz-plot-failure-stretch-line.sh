#!/bin/bash

# Just experimenting for getting smooth results

dir=$1

#y=`ls $dir/hr/faces-all`

i=0;

for face in 2 3 all
do

gnuplot<<EOF
set terminal pngcairo enhanced font 'Verdana-Bold,12' size 900,500 crop
set pointsize 0.6
set output "$dir/$face-failure-stretch.png"
set xlabel "Time (s)"
set ylabel "Delay Stretch"
set xrange [0:2700]
set ytics 0.5
#set mytics 0.1
set yrange [0.9:5]
set logscale y
set style func linespoints
#set title "Failure Stretch - 2 faces"

plot '$dir/f$face-failure-95th.stretch' title "95th percentile" fc rgb "black" lt -1 pi -4 pt 6 ps 0.5 smooth bezier w lp, \
     '$dir/f$face-failure-75th.stretch' title "75th percentile" fc rgb "black" lt -1 pi -3 pt 7 ps 0.5 smooth bezier w lp, \
     '$dir/f$face-failure-median.stretch' title "median" fc rgb "black" lt -1 pi -5 pt 4 ps 0.5 smooth bezier w lp
EOF

done
echo "complete"
