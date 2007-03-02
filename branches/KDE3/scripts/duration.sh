#!/bin/sh
FFMPEG_OUT=`ffmpeg -i "$1" 2>&1`
RESULT=$?
DURATIO=`echo $FFMPEG_OUT | grep "Duration" | cut -d"," -f 1 | sed "s/.*: //g"`
echo $DURATION
exit $RESULT