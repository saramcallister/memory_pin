#!/usr/bin/python3  

from collections import defaultdict

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

def convert_to_page(mem_addr):
    # shift by 2^12 = 4KB
    return mem_addr >> 12

def parse_input(filename, lines=float('inf'), endtime=float('inf')):
    timestamps = []
    count = -1
    num_accesses = {}
    with open(filename, 'r') as f:
        for i, line in enumerate(f):
            if i > lines:
                break
            if 'Seconds' in line:
                ts = float(line.split()[1])
                if last_timestamp > endtime:
                    break
                timestamps.append(ts)
                count += 1
                for page in num_accesses.keys():
                    num_accesses[page].append(0)
                continue
            
            # otherwise input is "ipc: W/R addr"
            # addr is 0xffffffffffff for 64 byte loads
            address = line.split()[-1]
            page = convert_to_page(int(address, 0))

            if page not in num_accesses:
                new_l = [0] * count
                new_l[-1] += 1
                num_accesses[page] = new_l
            else:
                num_accesses[page][-1] += 1
    print(timestamps[-1], "s")
    return timestamps, num_accesses

def get_matrix(num_accesses):
    sa = sorted(num_accesses.items(), key=lambda x: x[0])
    pages, counts = zip(*sa)
    return pages, counts

def graph(counts, savefile):
    x = list(range(len(counts)))
    plt.plot(x, counts, linestyle='-', linewidth=2)
    plt.xscale('log')
    plt.yscale('log')
    plt.ylabel('Count')
    plt.xlabel('Popularity')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')


timestamps, accesses = parse_input("/scratch/cpu2017pin/519.lbm_r", endtime=6000)
pages, counts = get_matrix(accesses)
print(len(pages))
graph(counts, 'acccesses.pdf')
