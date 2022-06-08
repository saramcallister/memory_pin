import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import argparse
from num2words import num2words

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs="+")
parser.add_argument('file_name', type=str, default="")
# parser.add_argument('input_file', type=str, default="")

args = parser.parse_args()

def scatter(data, graph_name, rows, cols):

    fig, ax = plt.subplots(nrows=rows, ncols=cols)
    plt.ylabel("fraction accuracy")
    plt.xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(rows):
        for j in range(cols):
            ax[i,j].scatter(range(len(data[(i*cols)+j])),data[(i*cols)+j], color=colors[(i*cols)+j], label=num2words((i*cols)+j+1))
            ax[i,j].legend(loc="upper left")
            ax[i,j].set_ylabel("fraction accuracy")
            ax[i,j].set_xlabel("Time")
            ax[i,j].set_ylim(0,1)

    fig.set_size_inches(18.5, 10.5)

    plt.savefig(graph_name) 

def main(): 
    
    data = [[] for i in range(len(args.input_files))]
    times = [[] for i in range(len(args.input_files))]


#     print(args)
    for fname in args.input_files:
        f = open(fname)
        lines = f.readlines() 
        index = int(fname[-1])
        if(index == 0):
            index = 10

#         print(fname)

        for i in range(0, len(lines)):
            line = lines[i].split(' ')
            if line[0] == 'ad' and line[1] == 'could':
                data[index-1].append(float(line[3]))
#                 print("appending " + line[3])

    scatter(data,args.file_name, 2, int(len(args.input_files)/2))



main()
