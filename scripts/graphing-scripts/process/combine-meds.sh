#!/bin/bash
#this script is called by cdf-med.sh

if [ $# -lt 1 ] ; then
        echo "Usage: $0 <test folder>"
        exit
fi
DIR=$1


y=`ls $DIR/hr/faces-2`
for j in $y
do
  echo "Name: " $j
  name=$j
  x=`find ./$DIR -name "ping-data"|grep $name`
  for i in $x
  do
#    echo "x: $i"
    cat $i/*.med|grep -v ^${name}  > $i.all
  done
done

