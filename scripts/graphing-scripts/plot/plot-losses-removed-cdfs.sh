#!/bin/bash

if [ $# -lt 4 ] ; then
        echo "Usage: $0 <CAIDA folder> <UCLA folder> <tag1> <tag2>"
        exit
fi
DIR1=$1
DIR2=$2
TAG1=$3
TAG2=$4


gnuplot << EOF
reset
set terminal pngcairo enhanced font 'Helvetica,10' size 1000,400 crop
set output "$DIR1/losses-removed-rtt-cdf.png"
set key inside bottom right

set yrange [0:1.05]
#set xrange [0.95:]
set grid xtics
set grid ytics

plot "$DIR1/ls-losses-removed.cdf" u 1:2 w l title "LS with $TAG1", \
     "$DIR1/hr-losses-removed.cdf" u 1:2 w l title "HR with $TAG1", \
     "$DIR2/ls-losses-removed.cdf" u 1:2 w l title "LS with $TAG2", \
     "$DIR2/hr-losses-removed.cdf" u 1:2 w l title "HR with $TAG2"

#plot "$DIR1/ls-losses-removed.cdf" u 1:2 w lp ps 1.5 pi 15 title "LS with $TAG1", \
 #    "$DIR1/hr-losses-removed.cdf" u 1:2 w lp ps 1.5 pi 15 title "HR with $TAG1", \
  #   "$DIR2/ls-losses-removed.cdf" u 1:2 w lp ps 1.5 pi 15 title "LS with $TAG2", \
   #  "$DIR2/hr-losses-removed.cdf" u 1:2 w lp ps 1.5 pi 15 title "HR with $TAG2"

set terminal postscript eps enhanced color font 'Helvetica,22' lw 3
set output "$DIR1/losses-removed-rtt-cdf.eps"
plot "$DIR1/ls-losses-removed.cdf" u 1:2 w l title "LS with $TAG1", \
     "$DIR1/hr-losses-removed.cdf" u 1:2 w l title "HR with $TAG1", \
     "$DIR2/ls-losses-removed.cdf" u 1:2 w l title "LS with $TAG2", \
     "$DIR2/hr-losses-removed.cdf" u 1:2 w l title "HR with $TAG2"
EOF
