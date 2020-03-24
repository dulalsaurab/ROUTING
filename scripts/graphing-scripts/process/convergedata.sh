#!/bin/bash
#generate data for creating convergence graphs

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
r=$1

y=`ls $r/hr/faces-2`

for j in $y
do
        name=$j
        rm $r/hr/faces-2/$name/ping-data/*.out
        d=`ls $r/hr/faces-2/$name/ping-data`

        dir=$r/hr/faces-2/$name/ping-data
        for k in $d
        do
                if [ ${k:(-3)} == "txt" ]
                        then
                                echo "-->Generating data for $dir/$k"
                                cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
                                a=`wc $dir/$k.out | awk '{print $1}'`
#                                a2=$(($a-4))
#                                sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed -i -e 's/^$/9999/' $dir/$k.out
                fi
        done
done

y=`ls $r/hr/faces-3`

for j in $y
do
	name=$j
	rm $r/hr/faces-3/$name/ping-data/*.out
	d=`ls $r/hr/faces-3/$name/ping-data`

	dir=$r/hr/faces-3/$name/ping-data
	for k in $d
	do
                if [ ${k:(-3)} == "txt" ]
                        then
                		echo "-->Generating data for $dir/$k"
                		cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
#                		a=`wc $dir/$k.out | awk '{print $1 }'`
#                		a2=$(($a-4))
#                		sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                		sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                		sed -i -e 's/^$/9999/' $dir/$k.out
                fi
	done
done

y=`ls $r/hr/faces-all`

for j in $y
do
        name=$j
        rm $r/hr/faces-all/$name/ping-data/*.out
        d=`ls $r/hr/faces-all/$name/ping-data`

        dir=$r/hr/faces-all/$name/ping-data
        for k in $d
        do
                if [ ${k:(-3)} == "txt" ]
                        then
                                echo "-->Generating data for $dir/$k"
                                cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
#                                a=`wc $dir/$k.out | awk '{print $1 }'`
#                                a2=$(($a-4))
#                                sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed -i -e 's/^$/9999/' $dir/$k.out
                fi
        done
done

y=`ls $r/ls/faces-2`

for j in $y
do
        name=$j
        rm $r/ls/faces-2/$name/ping-data/*.out
        d=`ls $r/ls/faces-2/$name/ping-data`

        dir=$r/ls/faces-2/$name/ping-data
        for k in $d
        do
                if [ ${k:(-3)} == "txt" ]
                        then
                                echo "-->Generating data for $dir/$k"
                                cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
#                                a=`wc $dir/$k.out | awk '{print $1 }'`
#                                a2=$(($a-4))
#                                sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed -i -e 's/^$/9999/' $dir/$k.out
                fi
        done
done

y=`ls $r/ls/faces-3`

for j in $y
do
        name=$j
        rm $r/ls/faces-3/$name/ping-data/*.out
        d=`ls $r/ls/faces-3/$name/ping-data`

        dir=$r/ls/faces-3/$name/ping-data
        for k in $d
        do
                if [ ${k:(-3)} == "txt" ]
                        then
                                echo "-->Generating data for $dir/$k"
                                cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
#                                a=`wc $dir/$k.out | awk '{print $1 }'`
#                                a2=$(($a-4))
#                                sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed -i -e 's/^$/9999/' $dir/$k.out
                fi
        done
done

y=`ls $r/ls/faces-all`

for j in $y
do
        name=$j
        rm $r/ls/faces-all/$name/ping-data/*.out
        d=`ls $r/ls/faces-all/$name/ping-data`

        dir=$r/ls/faces-all/$name/ping-data
        for k in $d
        do
                if [ ${k:(-3)} == "txt" ]
                        then
                                echo "-->Generating data for $dir/$k"
                                cat $dir/$k |grep "Ping Reference"  | awk '{if (NF > 10) print $(NF-1); else print "9999"}' > $dir/$k.out
#                                a=`wc $dir/$k.out | awk '{print $1 }'`
#                                a2=$(($a-4))
#                                sed "$a2"','"$a"'d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed '1d' $dir/$k.out > $dir/tmp && mv $dir/tmp $dir/$k.out
#                                sed -i -e 's/^$/9999/' $dir/$k.out
                fi
        done
done
