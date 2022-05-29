#!/usr/bin/python3  

from collections import defaultdict

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

def parse_file(filename):
    time_to_accesses = []
    with open(filename, 'r') as f:
        for line in f:
            split_line = line.split()
            time_to_accesses.append((float(split_line[1]), int(split_line[0])))
    return time_to_accesses

def graph(counts, savefile):
    x, y = zip(*counts)
    print(max(x))
    plt.plot(x, y, linestyle='', marker='.')
    #plt.xscale('log')
    plt.yscale('log')
    plt.ylabel('Accesses')
    plt.xlabel('Time(s)')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')

parsed = parse_file('519.hfa')
graph(parsed, 'hfa.png')
