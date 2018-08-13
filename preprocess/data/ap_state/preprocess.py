#   ed813-ThinkPad-X220  Infra  1     54 Mbit/s  95      ▂▄▆█  WEP

import re
rx = re.compile(r'(.+) +Infra +(\d+) +(\d+) +Mbit/s +(\d+) +(.+)')

def extract_signal_strength(row):
	m = rx.match(row)
	if m is not None:
		return (str(m.group(1)),int(m.group(4)))
	else:
		return ''

Signal_strength = []
AP_name = []

import sys
if ( len(sys.argv) < 2 ):
	print("Usage : python preprocess.py [ file.txt ]\n")
	exit()
with open(sys.argv[1], encoding="utf-8") as f:
	for line in f:
		tmp_ap_name,tmp_signal_strength = extract_signal_strength(line)
		Signal_strength.append(tmp_signal_strength)
		AP_name.append(tmp_ap_name.strip())

import pandas as pd
data = {'AP_name' : AP_name, 'Signal_strength' : Signal_strength}
dataframe = pd.DataFrame(data = data)
mean = dataframe.groupby('AP_name').mean().reset_index()

file_rx = re.compile(r'(.+).txt')
state = file_rx.match(sys.argv[1]).group(1)

for name,value in mean.values:
	with open('ap_ss_' + str(name) + '.csv', "a", encoding="utf-8") as myfile:
		myfile.write(str(state) + "," + str(value) + "\n")
