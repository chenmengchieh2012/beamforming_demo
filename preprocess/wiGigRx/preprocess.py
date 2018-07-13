# status:1
# RSSI(dBm):0
# Tx Sector:240
# Rx Sector:1
# message size : 933

# The time cost of each Rx Sector:1006

import re
import os
import sys, getopt
import pandas as pd

rssi_set = []
tx_sector_set = []
rx_sector_set = []

global isfound
isfound = False

rx = re.compile(r'status:(\d+|)(\r\n|\r|\n)RSSI\(dBm\):(-?\d+|)(\r\n|\r|\n)Tx Sector:(\d+|)(\r\n|\r|\n)Rx Sector:(\d+|)(\r\n|\r|\n)message size : (\d+|)')

def save_to_csv(f, name, value):
    f.write(str(name[0]) + "_" + str(name[1]) + "," + str(value) + "\n")

def re_filter(line):
    m = rx.match(line)
    if m is not None and not m.group(3) == '':
        return (abs(int(m.group(1))), abs(int(m.group(3))), abs(int(m.group(5))), abs(int(m.group(7))), abs(int(m.group(9))))
    else:
        return (0, 0, 0, 0, 0)

def gather_info(status, rssi, tx_sector, rx_sector, msg_size):
    if status == 1 and rssi != 0 and msg_size != 2 and msg_size < 1500 and tx_sector < 20:
        rssi_set.append( - rssi)
        tx_sector_set.append(tx_sector)
        rx_sector_set.append(rx_sector)

def form_dataframe(f):
    data = {'Tx_Sector' : tx_sector_set, 'Rx_Sector' : rx_sector_set, 'RSSI(dBm)' : rssi_set}
    dataframe = pd.DataFrame(data = data)
    dataframe2 = dataframe.groupby(['Rx_Sector', 'Tx_Sector'], sort = True)['RSSI(dBm)']
    for name, group in dataframe2:
        print ("( Rx, Tx ) => " + str(name))
        print (" [ RSSI ]  => " + str(group.mean()) + " (dBm)")
        print
        save_to_csv(f, name, group.mean())

def rm_space(line):
    return line.strip()

def isblank(line):
    return len(line)

def find_the_begining(line):
    global isfound
    if 'status' in line:
        isfound = True

def readfile(file):
    temp = ''
    iter = 0
    global isfound
    for line in file.readlines():
        line = rm_space(line)

        if not isfound: find_the_begining(line)

        if not 'time' in line:
            if isfound and iter != 5:
                iter = iter + 1
                temp = temp + line + '\n'
                if iter == 5:
                    status,rssi,tx_sector,rx_sector,msg_size = re_filter(temp)
                    gather_info(status, rssi, tx_sector, rx_sector, msg_size)
                    iter = 0
                    temp = ''
                    continue

def openfile(filename):
    return open(filename, 'r')

def get_filename():
    return sys.argv[1]

def get_output_file():
    return open('state_' + os.path.splitext(sys.argv[1])[0] + '.csv', "a")

def close_files(f, output):
    f.close()
    output.close()

filename = get_filename()
f = openfile(filename)   # sys.argv[0] = "python filename"
readfile(f)
output = get_output_file()
form_dataframe(output)
close_files(f, output)
