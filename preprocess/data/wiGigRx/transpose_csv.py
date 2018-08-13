import sys
# import csv
# from itertools import izip
import transposer

if len(sys.argc) < 3:
    print("Usage : python transpose_csv.py [ input_file ] [ output_file ]")
else:
    transposer.transpose(i=sys.argv[1], o=sys.argv[2])
    # rows = izip(*csv.reader(open(sys.argv[1], 'rb')))
    # csv.writer(open(sys.argv[2], 'wb')).writerows(rows)
