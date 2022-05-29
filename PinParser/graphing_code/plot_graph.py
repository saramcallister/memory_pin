import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import argparse
from num2words import num2words

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs=10)
parser.add_argument('file_name', type=str, default="")
# parser.add_argument('input_file', type=str, default="")

args = parser.parse_args()

def graph(data, times,graph_name):


    fig, ax = plt.subplots(nrows=1, ncols=1)
    ax.set_ylabel("Percent accuracy")
    ax.set_xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(10):
        ax.plot(times[i], data[i], color=colors[i], label=num2words(i+1))
    plt.legend(loc="upper left")
    plt.set_ylim(0,1)

    plt.savefig(graph_name) 



def main(): 
    
    data = [[] for i in range(10)]
    times = [[] for i in range(10)]


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

    graph(data,times, args.file_name)


main()
