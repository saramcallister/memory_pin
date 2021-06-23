#!/usr/bin/python3  

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

import numpy as np
from collections import defaultdict

import parser

def sorted_keys(num_accesses):
    page_to_accesses = defaultdict(int)
    for timeperiod in num_accesses:
        for entry in timeperiod:
            page_to_accesses[entry[0]] += entry[1]
    sa = sorted(page_to_accesses.items(), key=lambda x: -x[1])
    pages, count = zip(*sa)
    return pages

def get_sorted_matrix(pages, accesses, x_binning):
    m = np.zeros((len(pages), x_binning + 1))
    for i, timeperiod in enumerate(accesses):
        for entry in timeperiod:
            page_order = pages.index(entry[0])
            m[page_order][int( (i / len(accesses)) * x_binning)] += entry[1]
    return m

def get_matrix(num_accesses):
    binning = 1 << 14
    max_value = max([int(item[0]) for timeperiod in accesses for item in timeperiod])
    min_value = min([int(item[0]) for timeperiod in accesses for item in timeperiod])
    print(max_value)
    print(min_value)
    num_buckets = (max_value - min_value) // binning + 1
    m = np.zeros((num_buckets, len(num_accesses))) 
    for i, timeperiod in enumerate(num_accesses):
        for entry in timeperiod:
            m[(int(entry[0]) - min_value) // binning][i] += entry[1]
    return m

def graph(m, savefile):
    plt.imshow(m, cmap='viridis', aspect='auto')
    plt.colorbar()
    plt.savefig(savefile)
    print(f'Saved to {savefile}')


t, a = parser.parse_input("/scratch/cpu2017pin/627.cam4_r", end_time=60)
m = get_sorted_matrix(sorted_keys(a), a, 60)
print(f"Execution time: {t[-1]}")
graph(m, 'acccesses.pdf')
