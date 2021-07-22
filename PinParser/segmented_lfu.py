#!/usr/bin/python3  

from collections import defaultdict, namedtuple

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

Point = namedtuple('Point', ['local_pages', 'segment_size', 'hits', 'misses'])

def parse_file(filename):
    # file format: num local pages, 4k pages per segments, hits, misses
    print(f"Parsing {filename}")

    output = []
    with open(filename, 'r') as f:
        for line in f:
            split_line = line.split()
            output.append(Point(*map(int, split_line)))
    return output

def lines(output):
    # ret: {seg size KiB: [(local MB, miss ratio)]}
    ret = defaultdict(list)
    for p in output:
        ret[p.segment_size * 4].append((p.local_pages * 4 / 1024, p.misses / (p.hits + p.misses))) 
    return ret

def graph(grouping_to_mr, savefile):
    for seg_size, data in grouping_to_mr.items():
        x, y = zip(*sorted(data))
        plt.plot(x, y, linestyle='-', marker='x', linewidth=2, label=f'{seg_size} KB')
    plt.yscale('log')
    plt.legend()
    plt.ylabel('Miss Ratio')
    plt.xlabel('Size local Mem (MB)')
    plt.savefig(savefile)
    print(f'Saved to {savefile}')

parsed = parse_file('657.segmented_lfu')
mrs = lines(parsed)
graph(mrs, 'segmented_lfu.pdf')
