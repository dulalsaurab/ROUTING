#!/bin/bash

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
DIR=$1


gnuplot << EOF
reset
set terminal pngcairo enhanced font 'Helvetica,10' size 1000,400 crop
set output "$DIR/cdfs-rtt.png"
set key inside bottom right

set yrange [0.01:1.05]
set xrange [0.9:]
set logscale x
set grid xtics
set grid ytics
set xlabel "RTT Stretch (HR/LS)"
set ylabel "Cumulative percentage"

plot "$DIR/rtt-f2.cdf" u 1:2 w lp ps 1.5 pi 15 title "2 Faces", \
     "$DIR/rtt-f3.cdf" u 1:2 w lp ps 1.5 pi 15 title "3 Faces", \
     "$DIR/rtt-fall.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces"

set terminal postscript eps enhanced color font 'Helvetica,22' lw 3
set output "$DIR/cdfs-rtt.eps"
plot "$DIR/rtt-f2.cdf" u 1:2 w lp ps 2 pi 15  title "2 Faces", \
     "$DIR/rtt-f3.cdf" u 1:2 w lp ps 2 pi 15  title "3 Faces", \
     "$DIR/rtt-fall.cdf" u 1:2 w lp ps 2 pi 15 title "All Faces"
EOF
