CXX=g++
CXXFLAGS=-g -Wall -std=c++17

all: lfu \
	dynamic_lfu \
	working_set_size \
	hfa \
	first_seen \
	random \
	goanna0 \
	segmented_lfu \
	create_slfu_file \
	compare \
	pagemap_read \
    pebs \
    ad \
    pebs_gen \
    lfu_pebs

lfu: parser.o lfu.o 
	$(CXX) -o lfu lfu.o parser.o

lfu_pebs: parser.o lfu_pebs.o 
	$(CXX) -o lfu_pebs lfu_pebs.o parser.o

pebs: parser.o pebs.o 
	$(CXX) -o pebs pebs.o parser.o

pebs_gen: parser.o pebs_gen.o 
	$(CXX) -o pebs_gen pebs_gen.o parser.o

ad: parser.o ad.o 
	$(CXX) -o ad ad.o parser.o

goanna0: parser.o goanna0.o 
	$(CXX) -o goanna0 goanna0.o parser.o

first_seen: parser.o first_seen.o 
	$(CXX) -o first_seen first_seen.o parser.o

random: parser.o random.o 
	$(CXX) -o random random.o parser.o

hfa: parser.o hotness_vs_first_access.o
	$(CXX) -o hfa hotness_vs_first_access.o parser.o

dynamic_lfu: parser.o dynamic_lfu.o 
	$(CXX) -o dynamic_lfu dynamic_lfu.o parser.o

working_set_size: parser.o working_set_size.o
	$(CXX) -o working_set_size working_set_size.o parser.o

segmented_lfu: parser.o segmented_lfu.o
	$(CXX) -o segmented_lfu segmented_lfu.o parser.o

create_slfu_file: parser.o create_slfu_file.o
	$(CXX) -o create_slfu_file create_slfu_file.o parser.o

compare: parser.o compare.o
	$(CXX) -o compare compare.o parser.o

pagemap_read: pagemap_read.o
	$(CXX) -o pagemap_read pagemap_read.o

lfu.o: lfu.cpp parser.hpp

lfu_pebs.o: lfu_pebs.cpp parser.hpp

pebs.o: pebs.cpp parser.hpp

pebs_gen.o: pebs_gen.cpp parser.hpp

ad.o: ad.cpp parser.hpp

goanna0.o: goanna0.cpp parser.hpp

first_seen.o: first_seen.cpp parser.hpp

random.o: random.cpp parser.hpp

dynamic_lfu.o: dynamic_lfu.cpp parser.hpp

working_set_size.o: working_set_size.cpp parser.hpp

hotness_vs_first_access.o: hotness_vs_first_access.cpp parser.hpp

segmented_lfu.o: segmented_lfu.cpp parser.hpp

create_slfu_file.o: create_slfu_file.cpp parser.hpp

parser.o: parser.cpp parser.hpp

compare.o: compare.cpp parser.hpp

pagemap_read.o: pagemap_read.cpp
