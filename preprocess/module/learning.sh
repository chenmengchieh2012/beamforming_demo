#!/bin/bash

# ROOT="C:\Users\user\Desktop\preprocess"
# AP_STATE_PATH="\data\ap_state\output"
# AP_STATE_FILENAME="\ap_state.csv"
#
# TEST_PATH="\module"
# TEST_FILENAME="\test.txt"
#
# REWARD="1"
#
# if[ "$1" == "1" ] then
#   python qlearning.py "${1}" "${ROOT}${AP_STATE_PATH}${AP_STATE_FILENAME}" "${REWARD}"
# elif[ "$1" == "2" ] then
#   python qlearning.py "${1}" "${ROOT}${TEST_PATH}${TEST_FILENAME}" "${REWARD}"

python "C:/Users/user/Desktop/preprocess/module/"qlearning.py $1

rm "C:/Users/user/Desktop/preprocess/module/"temp.csv
#
