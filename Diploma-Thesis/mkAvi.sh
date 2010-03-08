#!/bin/sh

cd pngs
mencoder mf://*.jpg -mf w=800:h=600:fps=25:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -vf scale=800:600 -oac copy -o output.avi

