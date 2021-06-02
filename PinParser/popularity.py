#!/usr/bin/python3  

from collections import defaultdict

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

def convert_to_page(mem_addr):
    # shift by 2^12 = 4KB
    return mem_addr >> 12

def parse_input(filename, lines=float('inf'), endtime=float('inf')):
    last_timestamp = 0
    num_accesses = defaultdict(int)
    with open(filename, 'r') as f:
        for i, line in enumerate(f):
            if i > lines:
                break
            if 'Seconds' in line:
                last_timestamp = float(line.split()[1])
                if last_timestamp > endtime:
                    break
                continue
            
            # otherwise input is "ipc: W/R addr"
            # addr is 0xffffffffffff for 64 byte loads
            address = line.split()[-1]
            page = convert_to_page(int(address, 0))
            num_accesses[page] += 1
    print(last_timestamp)
    return num_accesses

def get_counts(num_accesses):
    sa = sorted(num_accesses.items(), key=lambda x: -x[1])
    pages, count = zip(*sa)
    return count

def graph(counts, savefile):
    x = list(range(len(counts)))
    plt.plot(x, counts, linestyle='-', linewidth=2)
    plt.xscale('log')
    plt.yscale('log')
    plt.ylabel('Count')
    plt.xlabel('Popularity')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')


accesses = parse_input("/scratch/cpu2017pin/519.lbm_r", endtime=6000)
counts = get_counts(accesses)
print(len(counts))
graph(counts, 'acccesses.pdf')
