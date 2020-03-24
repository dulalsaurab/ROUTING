#!/bin/bash

DIR=$1

if [ -z "$DIR" ]
then
  echo "Usage: $0 [DIR]"
  exit
fi

overheadDir="$DIR/overhead"
mkdir -p "$overheadDir"

# HR
grep ping $DIR/hr/faces-4/*/dump* | grep INTEREST | grep -v Nonce=0 | sort -k2 -t':' -n > "$overheadDir/hr-probes-sorted.txt"
cat "$overheadDir/hr-probes-sorted.txt" | cut -d' ' -f 1 | cut -d ':' -f2 > "$overheadDir/hr-probes-timestamps-sorted.txt"

hrFirstPing=$(grep -m1 ping $DIR/hr/faces-4/*/dump* | sort -k2 -t':' -n | head -1 | cut -d ':' -f 2 | cut -d '.' -f 1)
awk -vbegin="$hrFirstPing" 'BEGIN{last = 0; count = 0}{current = int($1); if (last == 0){last = current;} if (current > last){print current - begin " " count; count = 0; last = current;} ++count;}' "$overheadDir/hr-probes-timestamps-sorted.txt" > "$overheadDir/hr-overhead-by-time-raw.txt"

awk 'BEGIN{last=0}{current=$1; if (last == 0){last = current} if (current > last){for (i = last + 1; i < current; ++i){print i" 0"}} print $1" "$2; last = current;}' "$overheadDir/hr-overhead-by-time-raw.txt" > "$overheadDir/hr-overhead-by-time.txt" 

# Link-State
grep "sync\|name\|adjacency" $DIR/ls/faces-2/*/dump* | sort -k2 -t':' -n > "$overheadDir/ls-overhead-sorted.txt"
cat "$overheadDir/ls-overhead-sorted.txt" | cut -d' ' -f 1 | cut -d ':' -f2 > "$overheadDir/ls-overhead-timestamps-sorted.txt"

lsFirstPing=$(grep -m1 ping $DIR/ls/faces-4/*/dump* | sort -k2 -t':' -n | head -1 | cut -d ':' -f 2 | cut -d '.' -f 1)

awk -vbegin="$lsFirstPing" 'BEGIN{last = 0; count = 0}{current = int($1); if (last == 0){last = current;} if (current > last){print current - begin " " count; count = 0; last = current;} ++count;}' "$overheadDir/ls-overhead-timestamps-sorted.txt" > "$overheadDir/ls-overhead-by-time-raw.txt"

awk 'BEGIN{last=0}{current=$1; if (last == 0){last = current} if (current > last){for (i = last + 1; i < current; ++i){print i" 0"}} print $1" "$2; last = current;}' "$overheadDir/ls-overhead-by-time-raw.txt" > "$overheadDir/ls-overhead-by-time.txt"
