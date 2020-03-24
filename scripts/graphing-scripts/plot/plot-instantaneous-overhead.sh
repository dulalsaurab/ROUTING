#!/bin/bash

DIR=$1
FIRST_LS_PING=$2
FIRST_HR_PING=$3

gnuplot<<EOF

set terminal pngcairo enhanced font 'Verdana-Bold,15' size 900,500 crop
set output "testbed-seq-failure-instantaneous-overhead.png"

set termoption dashed

set xrange [0:2675]
set xtics 240
#set mxtics 60

set ylabel "Per-Node Messages Overhead\n(Packets Per Second)"
set xlabel "Time (s)"

set grid

plot "$DIR/overhead/ls-overhead-by-time.txt" using (\$1 - "$FIRST_LS_PING"):(\$2 / 2 / 22) title "LS" lc rgb "black" lt 3 with lines, \
     "$DIR/overhead/hr-overhead-by-time.txt" using (\$1 - "$FIRST_HR_PING"):(\$2 / 2 / 22) title "HR" lc rgb "black" lt 1 lw 2.5 with lines

EOF
