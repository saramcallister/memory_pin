#!/usr/bin/python3  

from collections import defaultdict

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

import parser

def sum_counts(accesses):
    page_to_accesses = defaultdict(int)
    for timeperiod in accesses:
        for entry in timeperiod:
            page_to_accesses[entry[0]] += entry[1]
    sa = sorted(page_to_accesses.items(), key=lambda x: -x[1])
    pages, count = zip(*sa)
    return count

def graph(counts, savefile):
    x = list(range(len(counts)))
    plt.plot(x, counts, linestyle='-', linewidth=2)
    #plt.xscale('log')
    plt.yscale('log')
    plt.ylabel('Count')
    plt.xlabel('Popularity')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')


t, accesses = parser.parse_input("/scratch/cpu2017pin/627.cam4_r", end_time=10000)
counts = sum_counts(accesses)
print(f"Execution time: {t[-1]}, Accessed: {len(counts)} pages, Total Mem Accesses: {sum(counts)}")
graph(counts, 'pop.pdf')
