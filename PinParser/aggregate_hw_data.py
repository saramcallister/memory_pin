import matplotlib
matplotlib.use("Agg")

import matplotlib.pyplot as plt
import os
import numpy as np
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('input_files', type=str, nargs="+")
args = parser.parse_args()

def graph(data, fname):
    fig, ax = plt.subplots(nrows=1,ncols=1)
    plt.ylabel("percent hot pages identified")
    plt.xlabel("intervals")

    plt.plot(data)

    plt.savefig(os.path.basename(fname))

def main():
    
    for fname in args.input_files:
        f=open(fname)
        aggregate = 0
        data = []

        lines = f.readlines()

        for i in range(0, len(lines)):
            line = lines[i].split(' ')

            if(line[0] == 'hotness' and line[1] == 'mechanism'):
                aggregate+= float(line[4])
                data.append(float(line[4]))

        print("average error for %s is %5.2f" % (fname, (aggregate/len(lines))))
        graph(data, fname)

main()


