#!/bin/bash

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
DIR=$1


gnuplot << EOF
reset
set terminal pngcairo enhanced font 'Verdana,10' size 1000,400 crop
set output "$DIR/cdfs-mesgs.png"
set key inside bottom right

set yrange [-0.05:1.05]
set grid xtics
set grid ytics

plot "$DIR/mesgs-f2.cdf" u 1:2 w lp ps 1.5 pi 15 title "2 Faces", \
     "$DIR/mesgs-f3.cdf" u 1:2 w lp ps 1.5 pi 15 title "3 Faces", \
     "$DIR/mesgs-fall.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces"
EOF
