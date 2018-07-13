#!/bin/bash

FILES=*.txt
for file in $FILES
do
	python preprocess.py $file
done

python merge_source_file.py
rm state_*

python transpose_csv.py "output/Rx_Tx_rssi.csv" "output/Rx_Tx_rssi_Transpose.csv"
