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

def scatter(data, times,graph_name, rows, cols):

    fig, ax = plt.subplots(nrows=rows, ncols=cols)
    plt.ylabel("fraction accuracy")
    plt.xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(rows):
        for j in range(cols):
            ax[i,j].scatter(times[(i*cols)+j], data[(i*cols)+j],
            color=colors[(i*cols)+j], label=num2words((i*cols)+j+1))
            ax[i,j].legend(loc="upper left")
            ax[i,j].set_ylabel("fraction accuracy")
            ax[i,j].set_xlabel("Time")

    fig.set_size_inches(18.5, 10.5)

    plt.savefig(graph_name) 

def main(): 
    
    data = [[] for i in range(len(args.input_files))]
    times = [[] for i in range(len(args.input_files))]


#     print(args)
    for fname in args.input_files:
        f = open(fname)
        lines = f.readlines() 

        for i in range(0, len(lines)):
            line = lines[i].split(' ')
            if line[0] == 'bit' and line[1] == 'number:':
                index = int(line[2]) - 1
                next_line = lines[i+1].split(' ')

                data[index].append(float(next_line[3]))
                times[index].append(float(line[4]))

    scatter(data,times, args.file_name, 2, int(len(args.input_files)/2))



main()
