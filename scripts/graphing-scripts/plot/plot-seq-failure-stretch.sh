
#!/bin/bash
dir=$1

#y=`ls $dir/hr/faces-3`

i=0;

for face in 2 3 4 all
do

gnuplot<<EOF
set terminal pngcairo enhanced font 'Verdana-Bold,15' size 900,500 crop
#set terminal latex
set output "$dir/$face-failure-stretch.png"
set xlabel "Time (s)"
set ylabel "Delay Stretch"

set xrange [0:2675]
set xtics 240

set ytics add ("0.1" 0.1, "0.25" 0.25, "0.5" 0.5, "0.75" 0.75, "1" 1, "1.25" 1.25, "1.5" 1.5, "1.75" 1.75, "2" 2, "2.5" 2.5, "3" 3, "3.5" 3.5, "4" 4)

#set ytics 0.5
#set mytics 0.1
set yrange [0.9:6]
set logscale y
#set style fill transparent
#set title "Failure Stretch - 2 faces"
set grid

#plot '$dir/f$face-failure-95th.stretch' title "2 Faces             95th percentile" fc rgb "black" pt 1 ps 1.2, \
      #'$dir/f$face-failure-75th.stretch' title "75th percentile" fc rgb "black" pt 2  ps 1, \
      # '$dir/f$face-failure-median.stretch' title "median" fc rgb "black" pt 4 ps 1

plot '$dir/f$face-failure-95th.stretch' title "95th percentile" fc rgb "black" lt -1 pi -4 pt 8 ps 1.8 w lp, \
     '$dir/f$face-failure-75th.stretch' title "75th percentile" fc rgb "black" lt -1 pi -4 pt 7 ps 1 w lp, \
     '$dir/f$face-failure-median.stretch' title "median" fc rgb "black" lt -1 pi -4 pt 4 ps 1 w lp

EOF

done
echo "complete"
