import matplotlib 

matplotlib.use("Agg") # to use over ssh

import matplotlib.pyplot as plt
import numpy as np 
import sys
import os
import argparse
from num2words import num2words

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs="+")
parser.add_argument('file_name', type=str, default="")
# parser.add_argument('input_file', type=str, default="")

args = parser.parse_args()

def scatter_data(data_503, data_505, data_519, data_557, data_602, graph_name):

    fig, ax = plt.subplots(nrows=1, ncols=5, sharex=True, sharey=True)

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

#     for i in range(rows):
#         for j in range(cols):
#             ax[i,j].scatter(range(len(data[(i*cols)+j])),data[(i*cols)+j], color=colors[(i*cols)+j], label=num2words((i*cols)+j+1))
#             ax[i,j].legend(loc="upper left")
#             ax[i,j].set_ylabel("fraction accuracy")
#             ax[i,j].set_xlabel("Time")
#             ax[i,j].set_ylim(0,1)
#     plt.grid(False)
#     plt.ylabel("fraction accuracy")
#     plt.xlabel("Time")
    
    for i in range(5):
        ax[i].scatter(range(len(data_503[i])), data_503[i], color=colors[0], label="503")
        ax[i].scatter(range(len(data_505[i])), data_505[i], color=colors[1], label="505")
        ax[i].scatter(range(len(data_519[i])), data_519[i], color=colors[2], label="519")
        ax[i].scatter(range(len(data_557[i])), data_557[i], color=colors[3], label="557")
        ax[i].scatter(range(len(data_602[i])), data_602[i], color=colors[4], label="602")
        ax[i].legend(loc="upper right")
#         ax[i,j].set_ylabel("fraction accuracy")
        ax[i].set_xlabel(str(i+1) + " bits")
        ax[i].set_ylim(0,1)

    fig.set_size_inches(18.5, 6)
    fig.text(0.5, 0.01, 'Time', ha='center', va='center')
    fig.text(0.1, 0.5, 'percent accuracy', ha='center', va='center',
    rotation='vertical')

    plt.savefig(graph_name) 

def main(): 
    
    data_503 = [[] for i in range(5)]
    data_505 = [[] for i in range(5)]
    data_519 = [[] for i in range(5)]
    data_557 = [[] for i in range(5)]
    data_602 = [[] for i in range(5)]


#     print(args)
    for fname in args.input_files:
        f = open(fname)
        lines = f.readlines() 
        index = int(fname[-1]) -1

        name = os.path.basename(fname)


        for i in range(0, len(lines)):
            line = lines[i].split(' ')
            if line[0] == 'ad' and line[1] == 'could':
                if(name[0:3] == '503'):
                    data_503[index].append(float(line[3]))
                if(name[0:3] == '505'):
                    data_505[index].append(float(line[3]))
                if(name[0:3] == '519'):
                    data_519[index].append(float(line[3]))
                if(name[0:3] == '557'):
                    data_557[index].append(float(line[3]))
                if(name[0:3] == '602'):
                    data_602[index].append(float(line[3]))



        index+=1

    scatter_data(data_503, data_505, data_519, data_557, data_602, args.file_name)



main()
