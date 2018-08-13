# 1. read the ap state file
# 2. determine the distance between the two aps' reference signal strength indicator in euclidean space
# 3. get the closest distance and the state index
# 4. read the wigig rssi file
# 5. get the optimal zoom by the state index
# 6. return the zoom range to Rx
# 7. trasmit the zoom range to the specific ap by RJ-45 to the ap


import os, sys, glob, csv, math
import numpy as np

def det_euclidean_distance(coordinate, obs_point):
    print("[ obs coordinate ]")
    print("( " + str(obs_point[0][0].strip()) + ", " + str(obs_point[0][1].strip()) + " )")
    print("[ coordinate ]")
    print("( " + str(coordinate[2].strip()) + ", " + str(coordinate[3].strip()) + " )")
    return math.sqrt((float(obs_point[0][0]) - float(coordinate[2]))**2 + (float(obs_point[0][1]) - float(coordinate[3]))**2)

def read_state(filename):
    min_distance = 1000
    min_state = 1
    fd_obs = open(filename)
    obs_coordinate = []
    for rows in fd_obs:
        obs_coordinate.append(rows.split(","))

    ap_file = "C:/Users/user/Desktop/preprocess/data/ap_state/output/ap_state.csv"
    fd = open(ap_file)
    iter = 0
    next(fd)
    for coordinate in fd:
        distance = det_euclidean_distance(coordinate.split(","), obs_coordinate)
        print("euclidean distance : " + str(distance))
        iter = iter + 1
        if min_distance > distance:
            min_distance = distance
            min_state = iter
    return min_state, min_distance

def which_state(filename):
    min_state, min_distance = read_state(filename)
    print("minimum state : " + str(min_state))
    print("minimum distance : " + str(min_distance))
    return min_state


def csv_to_list(data_set, file):
    rx = 0
    tx = 0
    for rows in file:
        # print(rows.split(","))
        for col in rows.split(","):
            if col.strip() is not "":
                data_set[rx][tx] = float(col)
                tx = tx + 1
        rx = rx + 1
        tx = 0


def get_rssi_table(filename):
    index = which_state(filename) - 1
    rssi_file = "C:/Users/user/Desktop/preprocess/module/final_table.csv"
    fd = open(rssi_file)
    rx_range = 7
    tx_range = 200
    table = np.zeros([rx_range, tx_range])
    csv_to_list(table, fd)
    print("\n[ q table ]\n")
    print(table)
    rssi_index = table[index].tolist().index(max(table[index][np.nonzero(table[index])].tolist()))
    print("\nmax rssi index : " + str(rssi_index))
    Rx = (rssi_index / 20) + 1
    Tx = rssi_index%20
    print("\n[ output ]\n")
    print("[ Rx - Tx ] : [ " + str(Rx) + " - " + str(Tx - 1) + " ]\n")
    print("[ Rx - Tx ] : [ " + str(Rx) + " - " + str(Tx) + " ]\n")
    print("[ Rx - Tx ] : [ " + str(Rx) + " - " + str(Tx + 1) + " ]\n")
    return Tx - 1, Tx, Tx + 1



def get_zoom(filename):
    Tx1, Tx2, Tx3 = get_rssi_table(filename)
#   /
#  /
# /
# \
#  \
#   \
    print("[ zoom ]")
    print("            " + str(Tx1))
    print("           /")
    print("          /")
    print("   Tx    /")
    print("  Sector \\")
    print("          \\")
    print("           \\")
    print("            " + str(Tx3))
#
#
# def transmit_zoom():
#
#
#


test_file = "C:/Users/user/Desktop/preprocess/testing/test.csv"
get_zoom(test_file)











#
