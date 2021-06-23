#!/usr/bin/python3  
import json

def parse_line(line):
    if len(line) <= 13:
        return 0, []
    line_dict = json.loads("{ " + line[:-6] + "] }")
    time_str = list(line_dict.keys())[0]
    address_to_accesses = line_dict[time_str]
    accesses = [tuple(d.items())[0] for d in address_to_accesses] 
    return float(time_str), accesses

# return list of timestamps, list of lists of (str(page #), # accesses)
# timestamps line up
def parse_input(filename, num_lines=float('inf'), end_time=float('inf')):
    timestamps = []
    accesses = []
    with open(filename, 'r') as f:
        for i, line in enumerate(f):
            if i > num_lines:
                break
            t, a = parse_line(line)
            if t == 0:
                continue
            if t > end_time:
                break
            timestamps.append(t)
            accesses.append(a)
    return timestamps, accesses

#t_parsed, a_parsed = parse_input("/scratch/cpu2017pin/627.cam4_r", end_time=100)
#print(t_parsed)
#print(a_parsed)
