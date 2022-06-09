import matplotlib
matplotlib.use("Agg")

import matplotlib.pyplot as plt
import os
import numpy as np
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs="+")
parser.add_argument('graph_name', type=str)
args = parser.parse_args()

def graph(data, names):
    fig, ax = plt.subplots(nrows=1,ncols=1)
    plt.ylabel("percent accuracy")
    plt.xlabel("time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(len(data)):
        plt.plot(data[i], color=colors[i], label=names[i])

    plt.legend(loc="upper right")
#     plt.ylim(0,1)


    plt.savefig(args.graph_name)

def main():

    data = [[] for i in range(len(args.input_files))]
    names = []
    index = 0
    
    for fname in args.input_files:
        f=open(fname)
        aggregate = 0

        lines = f.readlines()

        for i in range(0, len(lines)):
            line = lines[i].split(' ')

            if(line[0] == 'hotness' and line[1] == 'mechanism'):
                aggregate+= float(line[4])
                data[index].append(float(line[4]))

        print("average error for %s is %5.2f" % (fname, (aggregate/len(lines))))
        names.append(fname[0:3])
        index+=1
#         graph(data, fname)

    graph(data, names)

main()


