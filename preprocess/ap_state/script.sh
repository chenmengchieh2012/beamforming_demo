#!/bin/bash

FILES=*.txt
for file in $FILES
do
	ipython preprocess.py $file
done
ipython merge_AP_signal_strength.py
rm ap_ss*
