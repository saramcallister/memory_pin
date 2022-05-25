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

# def graph(graph_input, label):
def graph(data, times,graph_name):

    fig, ax = plt.subplots(nrows=2, ncols=5)
    plt.ylabel("fraction accuracy")
    plt.xlabel("Time")

    cmap = plt.get_cmap('tab20')
    colors = [cmap(2*i) for i in range(10)]

#     for i in range(5):
#         ax[0].plot(times[i], data[i], color=colors[i], label=num2words(i+1))
#     ax[0].legend(loc="upper left")
#     for i in range(5):
#         ax[1].plot(times[i+5], data[i+5], color=colors[i+5], label=num2words(i+6))
#     ax[1].legend(loc="upper left")
    for i in range(2):
        for j in range(5):
            ax[i,j].scatter(times[(i*5)+j], data[(i*5)+j],
            color=colors[(i*5)+j], label=num2words((i*5)+j+1))
            ax[i,j].legend(loc="upper left")
#     plt.legend(loc="upper left")

    fig.set_size_inches(18.5, 10.5)

#     plt.savefig("graph_all_in_one") 
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
