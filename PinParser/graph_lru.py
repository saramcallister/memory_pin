import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs="+")
parser.add_argument('graph_file', type=str, default="")

args = parser.parse_args()

def graph(data, times,names,graph_name):

    fig, ax = plt.subplots(nrows=1, ncols=1)
    plt.ylabel("miss ratio")
    plt.xlabel("movement frequency")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(len(data)):
        plt.plot(times[i], data[i], color=colors[i],
        label=os.path.basename(names[i]))

    plt.legend(loc="upper left")

    fig.set_size_inches(18.5, 10.5)

    plt.savefig(graph_name) 

def main(): 
    
    data = [[] for i in range(1*len(args.input_files))]
    times = [[] for i in range(1*len(args.input_files))]
    names = []


    index = 0
    for fname in args.input_files:
        f = open(fname)
        lines = f.readlines() 

#         for i in range(0, int(len(lines)/2)):
#             line = lines[i].split(' ')
# 
#             data[index].append(float(line[5]))
#             times[index].append(float(line[1]))
#         names.append(fname+"_25")
#         index += 1

        for i in range(int(len(lines)/2), len(lines)):
            line = lines[i].split(' ')

            data[index].append(float(line[5]))
            times[index].append(float(line[1]))
        names.append(fname)
        index += 1


    graph(data,times, names, args.graph_file)


main()
