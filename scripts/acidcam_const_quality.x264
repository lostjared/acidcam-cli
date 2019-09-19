#!/bin/sh

if [ $# -ne 2 ]
then
echo "Requires two  arguments:"
echo "input.mov output.mov"
exit -1
fi

ffmpeg -i $1 -c:v libx264 -crf 22 $2
