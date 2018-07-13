import pandas as pd
import glob
import os
from functools import reduce


allFiles = glob.glob("*.csv")
frame = pd.DataFrame()
lists = []

def extract_filename_extension(_file):
    filename_and_extension = os.path.splitext(_file)
    return filename_and_extension[0]

for _file in allFiles:
    filename = extract_filename_extension(_file)
    dataframe = pd.read_csv(_file,index_col=None, header=None,names=['Rx_Tx',filename+'_rssi(dBm)'])
    lists.append(dataframe)

df_final = reduce(lambda left,right: pd.merge(left,right,on='Rx_Tx'), lists)
df_final.to_csv('output/Rx_Tx_rssi.csv',sep=',')
