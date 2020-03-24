#!/bin/bash

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
DIR=$1


gnuplot << EOF
reset
set terminal pngcairo enhanced font 'Helvetica,10' size 1000,400 crop
set key inside bottom right

set yrange [-0.05:1.05]
set grid xtics
set grid ytics

set output "$DIR/cdfs-ratio-single.png"
plot "$DIR/ratio.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces"

set output "$DIR/cdfs-ratio.png"
plot "$DIR/ratio-f2.cdf" u 1:2 w lp ps 1.5 pi 15 title "2 Faces", \
     "$DIR/ratio-f3.cdf" u 1:2 w lp ps 1.5 pi 15 title "3 Faces", \
     "$DIR/ratio-fall.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces"

set terminal postscript eps enhanced color font 'Helvetica,22' lw 3
set output "$DIR/cdfs-ratio-single.eps"
plot "$DIR/ratio.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces"

set output "$DIR/cdfs-ratio.eps"
plot "$DIR/ratio-f2.cdf" u 1:2 w lp ps 2 pi 15  title "2 Faces", \
     "$DIR/ratio-f3.cdf" u 1:2 w lp ps 2 pi 15  title "3 Faces", \
     "$DIR/ratio-fall.cdf" u 1:2 w lp ps 2 pi 15 title "All Faces"
EOF
