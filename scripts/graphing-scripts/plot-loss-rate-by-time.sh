#!/bin/bash
# Script will work only after running process/convergedata-bytime-new-ping.sh

if [ $# -lt 1 ]; then
        echo "--"
        echo "Usage: $0 <dir name>"
        exit
fi

DIR=$1

for face in 4 3 2 all
do
    if [ ! -d "$DIR/hr/faces-$face" ]
    then
        continue
    fi
    for type in hr ls
    do
        # " 0" is timeout
        cat $DIR/$type/"faces-"$face/*/ping-data/*.out | grep " 0" | awk '{print int($1)}' | sort -n | uniq -c | awk '{print $2 " " $1}' > $DIR/$type/"faces-"$face/loss-rate-xtime.txt
    done

gnuplot << EOF
set terminal pngcairo enhanced font 'Verdana-Bold,17' size 900,500 crop
set pointsize 0.5
set xlabel "Time (s)"
set ylabel "Number of timeouts"
set xtics 50
set yrange[0:]
set xrange[0:]
set key top right

set output "$DIR/hr/losstime.png"

plot "$DIR/hr/faces-$face/loss-rate-xtime.txt" title "HR $face Faces" fc rgb "black" pt 21 ps 1, \
     "$DIR/ls/faces-$face/loss-rate-xtime.txt" title "LS $face Faces" fc rgb "black" pt 17 ps 1

EOF
done
