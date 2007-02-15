#!/bin/sh
DURATION=`ffmpeg -i "$1" 2>&1 | grep "Duration" | cut -d"," -f 1 |\
  sed "s/.*: //g"`
RESULT=$?
echo $DURATION
exit $RESULT