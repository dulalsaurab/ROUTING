#!/bin/bash
if [ $# -lt 1 ]; then
        echo "--"
        echo "Usage: $0 <dir name>"
        exit
fi

DIR=$1

rm $DIR/*/*/*.percentT
for face in 4 2 3 all
do
    if [ ! -d "$DIR/hr/faces-$face" ]
    then
        continue
    fi

    y=`ls -l $DIR/ls/faces-$face | grep -v second | grep '^d' | awk '{print $9}'`
    for type in hr ls
    do
    for node in $y
    do
        numTimeout=`cat $DIR/$type/faces-$face/$node/ping-data/*.txt | grep "timeout" | wc -l`
        numContent=`cat $DIR/$type/faces-$face/$node/ping-data/*.txt | grep "content" | wc -l`
        div=`echo "$numTimeout + $numContent" | bc -l`
        pc_timeout=`echo "scale=5; ($numTimeout/$div)*100" | bc -l`
        echo "$node $pc_timeout" >> $DIR/$type/faces-$face/f$face.percentT
    done
    sort -nk2 -o $DIR/$type/faces-$face/f$face.percentT $DIR/$type/faces-$face/f$face.percentT
   done
done


gnuplot << EOF
set terminal pngcairo enhanced font 'Verdana-Bold,17' size 900,500 crop
set pointsize 4
set xlabel "Node index"
set ylabel "Ping loss rate"
set xtics 1
set yrange[:]
set key top right

set output "$DIR/hr/percent.png"

plot "$DIR/hr/faces-4/f4.percentT" using 2 title "HR 4 Faces" fc rgb "black" with linespoints, \
     "$DIR/ls/faces-4/f4.percentT" using 2 title "LS 4 Faces" fc rgb "black" with linespoints
EOF
