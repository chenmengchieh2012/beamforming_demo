import pandas as pd
import glob
import os
from functools import reduce

allFiles = glob.glob("*.csv")
frame = pd.DataFrame()
lists = []

def extract_filename_extension():
    filename_and_extension = os.path.basename(_file)
    return os.path.splitext(filename_and_extension)

for _file in allFiles:
    filename, extension = extract_filename_extension()
    dataframe = pd.read_csv(_file,index_col=None, header=None,names=['state',filename+'_Signal_Strength'])
    lists.append(dataframe)
    print(dataframe)



df_final = reduce(lambda left,right: pd.merge(left,right,on='state'), lists)
df_final.to_csv('output/ap_state.csv',sep=',')
