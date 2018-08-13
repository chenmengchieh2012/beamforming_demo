#!/bin/bash

cd "C:/Users/user/Desktop/preprocess/data/wiGigRx"
bash script.sh

cd "C:/Users/user/Desktop/preprocess/module"
python merge_table.py

cd "C:/Users/user/Desktop/preprocess/testing"
python testing.py



#
