#!/bin/bash

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
dir=$1

#get the name of nodes
y=`ls $dir/hr/faces-2`

for k in $y
do
echo "Node: $k"

gnuplot << EOF
reset
fontsize = 14
set terminal pngcairo enhanced font 'Verdana,10' size 900,500 crop
set boxwidth 1.0
set style fill solid 0.5 border 0
set style histogram clustered gap 3 errorbars lw 1
set style data histograms
set datafile missing "-"

set yrange[0:500]
set grid ytics
set xtics rotate
set key outside
set key right center
set key vertical
set size 1,0.7
set ylabel "Round Trip Time (ms)"

set output "$dir/med-${k}.png"
set title "Med RTT from $k to other sites"
plot '$dir/hr/faces-2/$k/ping-data.all' u 2:3:4:xtic(1) title "hr-f2" fc rgb "red",\
	     '$dir/ls/faces-2/$k/ping-data.all' u 2:3:4 title "ls-f2" fc rgb "blue",\
	     '$dir/hr/faces-3/$k/ping-data.all' u 2:3:4 title "hr-f3" fc rgb "green",\
	     '$dir/ls/faces-3/$k/ping-data.all' u 2:3:4 title "ls-f3" fc rgb "magenta",\
	     '$dir/hr/faces-all/$k/ping-data.all' u 2:3:4 title "hr-all" fc rgb "pink",\
	     '$dir/ls/faces-all/$k/ping-data.all' u 2:3:4 title "ls-all" fc rgb "cyan"
EOF
done #for k in $y

