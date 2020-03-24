#!/bin/bash

if [ $# -lt 1 ] ; then
echo "Usage: $0 <test folder>"
exit
fi
dir=$1
y=`ls $dir/ls/faces-2`

for k in $y
do
gnuplot << EOF
reset
fontsize = 14
set terminal pngcairo enhanced font 'Verdana,10' size 900,500 crop
set boxwidth 1.0
set style fill solid 0.5 border 0
set style histogram clustered gap 3 errorbars lw 1
set style data histograms
set datafile missing "-"

set yrange[0:700]
set grid ytics
set xtics rotate
set key outside
set key right center
set key vertical
set size 1,0.7
set ylabel "Round Trip Time (ms)"

set output "$dir/med-${k}.png"
set title "Med RTT from $k to other sites"

plot '$dir/ls/faces-1/$k/ping-data.all' u 2:3:4:xtic(1) title "ls-f1" fc rgb "red",\
	     '$dir/ls/faces-2/$k/ping-data.all' u 2:3:4 title "ls-f2" fc rgb "blue",\
	     '$dir/ls/faces-all/$k/ping-data.all' u 2:3:4 title "ls-all" fc rgb "green"
EOF
done	     
