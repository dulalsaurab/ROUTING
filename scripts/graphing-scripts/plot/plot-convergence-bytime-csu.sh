#!/bin/bash

if [ $# -lt 2 ] ; then
        echo "Usage: $0 <test folder> <pkts: for 1 minu 120, for 5 min set it to 180>"
        exit
fi
dir=$1
pkts=$2

y=`ls $dir/ls/faces-2`

i=0;

for k in $y
do
  [ $k == "csu" ] && continue

  r=`ls $dir/hr/faces-2/$k/ping-data`

  for f in $r
  do
    if [ ${f:(-3)} == "out" ]
      then
      IFS='.' read -r dst rem <<< "$f"
      echo "plotting $k to $dst"

gnuplot<<EOF
set terminal pngcairo enhanced font 'Verdana,10' size 900,500 crop
set output "$dir/$k-$dst-converge.png"
set xlabel "Time seconds"
set ylabel "RTT"
set xrange [1:360]
set xtics 30
set yrange [1:600]
#set logscale y

set label "CSU\nnode down" at 55,graph(0.9,0.9) center font ",10"
set label "CSU\nnode up" at 175,graph(0.9,0.9) center font ",10"

set arrow from 65,graph(0.8,0.8) to 85,graph(0.7,0.7)
set arrow from 185,graph(0.8,0.8) to ${pkts},graph(0.7,0.7)

set arrow from 85,graph(0,0) to 85,graph(1,1) nohead
set arrow from $pkts,graph(0,0) to ${pkts},graph(1,1) nohead

#set multiplot layout 3, 2 title "$k to $dst"
set multiplot layout 2, 1 title "$k to $dst"

set title "Hyperbolic Routing - 2 faces"
unset key
plot '$dir/hr/faces-2/$k/ping-data/$f' fc rgb "red"

set title "Link State Routing - 2 faces"
unset key
plot '$dir/ls/faces-2/$k/ping-data/$f' fc rgb "blue"

unset multiplot
EOF
    fi
  done
done

echo "complete"
