#!/bin/bash

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
DIR=$1


gnuplot << EOF
reset
set terminal pngcairo enhanced font 'Helvetica,10' size 1000,400 crop
set output "$DIR/cdfs-timeout.png"
set key inside bottom right

set yrange [0.0:1.05]
set xrange [0.95:]
set grid xtics
set grid ytics

plot "$DIR/rtt-f2-hr.cdf" u 1:2 w lp ps 1.5 pi 15 title "2 Faces hr", \
     "$DIR/rtt-f2-ls.cdf" u 1:2 w lp ps 1.5 pi 15 title "2 Faces ls", \
     "$DIR/rtt-f3-hr.cdf" u 1:2 w lp ps 1.5 pi 15 title "3 Faces hr", \
     "$DIR/rtt-f3-ls.cdf" u 1:2 w lp ps 1.5 pi 15 title "3 Faces ls", \
     "$DIR/rtt-fall-hr.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces hr", \
     "$DIR/rtt-fall-ls.cdf" u 1:2 w lp ps 1.5 pi 15 title "All Faces ls"

set terminal postscript eps enhanced color font 'Helvetica,22' lw 3
set output "$DIR/cdfs-timeout.eps"
plot "$DIR/rtt-f2-hr.cdf" u 1:2 w lp ps 2 pi 15  title "2 Faces hr", \
     "$DIR/rtt-f2-ls.cdf" u 1:2 w lp ps 2 pi 15  title "2 Faces ls", \
     "$DIR/rtt-f3-hr.cdf" u 1:2 w lp ps 2 pi 15  title "3 Faces hr", \
     "$DIR/rtt-f3-ls.cdf" u 1:2 w lp ps 2 pi 15  title "3 Faces ls", \
     "$DIR/rtt-fall-hr.cdf" u 1:2 w lp ps 2 pi 15 title "All Faces hr", \
     "$DIR/rtt-fall-ls.cdf" u 1:2 w lp ps 2 pi 15 title "All Faces ls"
EOF
