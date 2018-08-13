import glob

allFiles = glob.glob("*.csv")
fout = open("final_table.csv","a")

for file in allFiles:
    f = open(file)
    for line in f:
        fout.write(line)
        fout.write("\n")
    f.close()

fout.close()







#
