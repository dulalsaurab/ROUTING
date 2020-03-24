#!/bin/bash
dir=$1

i=0;
xrange=300
for face in 4 2 3 all
do
    #if [ ! -d "$dir/hr/faces-$face" ] || [ ! -d "$dir/ls/faces-$face" ]
    #then
    #    continue
    #fi

    y=`ls $dir/hr/faces-$face | grep -v percentT | grep -v README`
    for first in $y
    do
       for second in $y
       do
          if [ "$first" != "$second" ] && [ -f $dir/hr/faces-$face/$first/ping-data/$second.txt ]; then
             xrange=`wc $dir/hr/faces-$face/$first/ping-data/$second.txt | awk '{print $1}'`
             xrange=`echo "$xrange-5" | bc`
             break
          else
             continue
          fi
       done
       break
    done

echo $xrange
gnuplot<<EOF
set terminal pngcairo enhanced font 'Verdana-Bold,15' size 900,500 crop
#set terminal latex
set output "$dir/$face-failure-stretch.png"
set xlabel "Time (s)"
set ylabel "Delay Stretch"
set xrange [0:"$xrange"]
set grid
set ytics add ("0.1" 0.1, "0.25" 0.25, "0.5" 0.5, "0.75" 0.75, "1" 1, "1.25" 1.25, "1.5" 1.5, "1.75" 1.75, "2" 2, "2.5" 2.5, "3" 3, "3.5" 3.5, "4" 4, "4.5" 4.5, "5" 5, "5.5" 5.5)
set mytics 20
set yrange [0.75:5.5]
set logscale y
#set style fill transparent
#set title "Failure Stretch - 2 faces"

#plot '$dir/f$face-failure-95th.stretch' title "2 Faces             95th percentile" fc rgb "black" pt 1 ps 1.2, \
      #'$dir/f$face-failure-75th.stretch' title "75th percentile" fc rgb "black" pt 2  ps 1, \
      # '$dir/f$face-failure-median.stretch' title "median" fc rgb "black" pt 4 ps 1

plot '$dir/f$face-failure-95th.stretch' title '$face next hops            95th percentile' lt -1 pi -4 pt 8 ps 1.8 w lp, \
     '$dir/f$face-failure-75th.stretch' title '75th percentile' lt -1 pi -4 pt 7 ps 1.3 w lp, \
     '$dir/f$face-failure-median.stretch' title 'median' lt -1 pi -4 pt 4 ps 1.3 w lp

EOF

done
echo "Completed."
