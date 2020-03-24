#!/bin/bash
if [ $# -lt 1 ] ; then
        echo "--"
        echo "Usage: $0 <dir name>"
        exit
fi

DIR=$1
rm $DIR/hr/*/*.percentT
for face in 4 2 3 all
do
    if [ ! -d "$DIR/hr/faces-$face" ]
    then
        continue
    fi

    y=`ls -l $DIR/hr/faces-$face | grep '^d' | awk '{print $9}' | grep -v second`
    for node in $y
    do
        numTimeout=`cat $DIR/hr/faces-$face/$node/ping-data/*.txt | grep -c "timeout"`
        numContent=`cat $DIR/hr/faces-$face/$node/ping-data/*.txt | grep -c "content"`
        div=`echo "$numTimeout + $numContent" | bc -l`
        if [ "$div" != 0 ]; then
            pc_timeout=`echo "scale=5; ($numTimeout/$div)*100" | bc -l`   # for 22 node
        fi
        echo "$node $pc_timeout" >> $DIR/hr/faces-$face/f$face.percentT
    done
    sort -nk2 -o $DIR/hr/faces-$face/f$face.percentT $DIR/hr/faces-$face/f$face.percentT
done


gnuplot << EOF
set terminal pngcairo enhanced font 'Verdana-Bold,17' size 900,500 crop
set pointsize 3     # for the size of keys
set xlabel "Node index"
set ylabel "Ping loss rate"
set xtics 1
set xtics font ", 8"
#set yrange[-1:]
set key top left    # to align title

set output "$DIR/hr/percent.png"

#plot "$DIR/hr/faces-2/f2.percentT" using 2 title "2 Next Hops" fc rgb "black" w lp, \
#     "$DIR/hr/faces-3/f3.percentT" using 2 title "HR 3 Next Hops" fc rgb "black" w lp, \
#     "$DIR/hr/faces-all/fall.percentT" using 2 title "All Next Hops" fc rgb "black" w lp, \
#     "$DIR/ls/faces-3/f3.percentT" using 2 title "LS 3 Next Hops" fc rgb "black" w lp, \
#plot "$DIR/ls/faces-4/f4.percentT" using 2 title "LS 4 Next Hops" fc rgb "black" w lp, \
#     "$DIR/hr/faces-4/f4.percentT" using 2 title "HR 4 Next Hops" fc rgb "black" w lp

plot "$DIR/hr/faces-2/f2.percentT" using 2 title "2 Next Hops" fc rgb "black" w lp, \
     "$DIR/hr/faces-3/f3.percentT" using 2 title "3 Next Hops" fc rgb "black" w lp, \
     "$DIR/hr/faces-4/f4.percentT" using 2 title "4 Next Hops" fc rgb "black" pt 8 w lp, \
     "$DIR/hr/faces-all/fall.percentT" using 2 title "All Next Hops" fc rgb "black" w lp

#plot "$DIR/hr/faces-4/f4.percentT" using 2 title "4 Next Hops" fc rgb "black" pt 8 w lp
#plot "$DIR/ls/faces-4/f4.percentT" using 2 title "4 Next Hops" fc rgb "black" pt 8 w lp

EOF
