import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import argparse
from num2words import num2words

parser = argparse.ArgumentParser()
parser.add_argument('input_file', type=str, default="", nargs="+")
parser.add_argument('graph_file', type=str, default="")

args = parser.parse_args()

def graph(data, times,names, graph_name):

    fig, ax = plt.subplots(nrows=1, ncols=1)
#     plt.ylabel("fraction accuracy")
#     plt.xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

    for i in range(len(data)):
        ax.plot(times[i], data[i], color=colors[i], label=names[i])
    plt.legend(loc="upper right")

    ax.set_ylim(0,1)
#     ax[1].scatter(times,data)
#     ax[1].set_ylim(0,1)

    fig.set_size_inches(14.5, 7.5)
    fig.text(0.5, 0.06, 'Time', fontsize = 15,  ha='center', va='center')
    fig.text(0.08, 0.5, 'percent accuracy', fontsize = 15, ha='center', va='center',
    rotation='vertical')

    plt.savefig(graph_name) 

def main(): 
    
    data = [[] for i in range(len(args.input_file))]
    times = [[] for i in range(len(args.input_file))]
    names = []
#     data=[]
#     times=[]

    index = 0

    for fname in args.input_file:
        print("opening " + fname)
        f = open(fname)
        lines = f.readlines() 
        names.append(fname[0:3])

        for i in range(0, len(lines)):
            line = lines[i].split(' ')
            if line[0] == 'time:' :
                next_line = lines[i+1].split(' ')

                data[index].append(float(next_line[3]))
                times[index].append(float(line[1]))

        index+=1
        print(fname + " done")
#     print(args.input_file + " " + str(np.average(data)) + " " + str(np.var(data)))

    print("graphing...")
    graph(data,times, names, args.graph_file)


main()
