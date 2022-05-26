import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import argparse
from num2words import num2words

parser = argparse.ArgumentParser()
parser.add_argument('input_file', type=str, default="")
parser.add_argument('graph_file', type=str, default="")

args = parser.parse_args()

def graph(data, times,graph_name):

    fig, ax = plt.subplots(nrows=1, ncols=2)
    plt.ylabel("fraction accuracy")
    plt.xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    ax[0].plot(times, data)
    ax[1].scatter(times,data)

    fig.set_size_inches(18.5, 10.5)

    plt.savefig(graph_name) 

def main(): 
    
#     data = [[] for i in range(len(args.input_files))]
#     times = [[] for i in range(len(args.input_files))]
    data=[]
    times=[]

    f = open(args.input_file)
    lines = f.readlines() 

    for i in range(0, len(lines)):
        line = lines[i].split(' ')
        if line[0] == 'time:' :
            next_line = lines[i+1].split(' ')

            data.append(float(next_line[3]))
            times.append(float(line[1]))

    graph(data,times, args.graph_file)


main()
