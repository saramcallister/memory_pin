#!/usr/bin/python3  

from collections import defaultdict

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

def parse_file(filename):
    all_lines = []
    with open(filename, 'r') as f:
        for line in f:
            split_line = line.split()
            all_lines.append((int(split_line[0]), int(split_line[1])))
    return all_lines

def cumulative(all_lines):
    sorted_lines = sorted(all_lines)
    cum_count = 0
    cum_lines = []
    for page_num, count in sorted_lines:
        cum_lines.append((page_num, count + cum_count))
        cum_count += count
    return cum_lines

def graph(counts, savefile):
    x, y = zip(*counts)
    y_norm = [c / y[-1] for c in y]
    x_formatted = [p * 4096 / (1024 * 1024) for p in x]
    plt.plot(x_formatted, y_norm, linestyle='-', linewidth=2)
    #plt.xscale('log')
    #plt.yscale('log')
    plt.ylabel('CDF')
    plt.xlabel('MB')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')

parsed = parse_file('519.size_dist')
counts = cumulative(parsed)
graph(counts, 'working_set.pdf')
