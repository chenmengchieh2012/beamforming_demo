#!/bin/bash

FILES=*.txt
for file in $FILES
do
	python preprocess.py $file
done
