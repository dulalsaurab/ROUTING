#!/bin/bash
#generate data for creating convergence graphs

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
r=$1

# TO BE IMPLEMENTED: INSTEAD OF USING 971 for timeout pass it as a parameter to this script

y=`ls $r/hr/faces-2`

for face in 2 # 3 all
do
for type in hr ls
do
     # getting the smallest timestamp for each type of routing
     start=`cat $r/$type/faces-$face/*/ping-data/*.txt | grep "Ping Reference"| awk '{print substr($1,10,12)}' | sort -n | head -1`
     start=${start:0:2}*3600+${start:2:2}*60+${start:4:10}
     start=`echo "$start" | bc -l`
     for j in $y
     do
           name=$j
           rm $r/$type/faces-$face/$name/ping-data/*.out
           d=`ls $r/$type/faces-$face/$name/ping-data`

           dir=$r/$type/faces-$face/$name/ping-data
           for k in $d
           do
             if [ ${k:(-3)} == "txt" ]
             then
                 echo "-->Generating data for $dir/$k"
                 # print timestamp along with RTT
                 #stamp=`cat $dir/$k |grep "Ping Reference" | awk '{print $1}' | awk -F'T' '{print $2}'`
                 cat $dir/$k |grep "Ping Reference" | awk -vstart="$start" '{ x=substr($1,10,13); sec=substr(x,0,2)*3600+60*substr(x,3,2)+substr(x,5,10); if (NF > 10) {printf "%2f %s\n", sec-start ,$(NF-1);} else printf "%2f %s\n", sec-start, " 971"}' > $dir/$k.out
             fi
           done
      done
done
done
