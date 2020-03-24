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

    y=`ls -l $DIR/ls/faces-$face | grep '^d' | awk '{print $9}' | grep -v second`
    for type in hr ls
    do
    for node in $y
    do
        numTimeout=`cat $DIR/$type/faces-$face/$node/ping-data/*.txt | grep -c "timeout\|nack"`
        numContent=`cat $DIR/$type/faces-$face/$node/ping-data/*.txt | grep -c "content"`
        div=`echo "$numTimeout + $numContent" | bc -l`
        if [ "$div" != 0 ]; then
            pc_timeout=`echo "scale=5; ($numTimeout/$div)*100" | bc -l`
        fi
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
set ytics 4 
set xrange[1:22]
set yrange[0:20]  #0:12
set key top right

set output "$DIR/hr/percent.png"

plot "$DIR/hr/faces-4/f4.percentT" using (\$0 + 1):2 title "HR" fc rgb "black" with linespoints, \
     "$DIR/ls/faces-4/f4.percentT" using (\$0 + 1):2 title "LS" fc rgb "black" with linespoints
EOF
