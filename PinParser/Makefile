CXX=g++
CXXFLAGS=-g -Wall -std=c++17
SRC=./src

all: lfu \
	dynamic_lfu \
	dynamic_lfu_debug \
	dynamic_lfu_random \
	dynamic_lfu_fifo \
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
    ad_gen \
    pebs_gen \
    lfu_pebs \
    random_pebs \
    first_seen_pebs \
    dynamic_lfu_pebs \
    dynamic_lfu_fifo_pebs \
    dynamic_lfu_random_pebs \
    count_pages \
    dynamic_lfu_ad \
    dynamic_lfu_random_ad \
    dynamic_lfu_fifo_ad \
    hw_hotness \
    dynamic_lfu_hw \
    lru \
    lru_pebs \
    lru_hw


lfu: parser.o lfu.o 
	$(CXX) -o lfu lfu.o parser.o

lru: parser.o lru.o 
	$(CXX) -o lru lru.o parser.o

lru_hw: parser.o lru_hw.o 
	$(CXX) -o lru_hw lru_hw.o parser.o

lru_pebs: parser.o lru_pebs.o 
	$(CXX) -o lru_pebs lru_pebs.o parser.o

count_pages: parser.o count_pages.o 
	$(CXX) -o count_pages count_pages.o parser.o

lfu_pebs: parser.o lfu_pebs.o 
	$(CXX) -o lfu_pebs lfu_pebs.o parser.o

pebs: parser.o pebs.o 
	$(CXX) -o pebs pebs.o parser.o
    
hw_hotness: parser.o hw_hotness.o 
	$(CXX) -o hw_hotness hw_hotness.o parser.o

pebs_gen: parser.o pebs_gen.o 
	$(CXX) -o pebs_gen pebs_gen.o parser.o

ad: parser.o ad.o 
	$(CXX) -o ad ad.o parser.o

ad_gen: parser.o ad_gen.o 
	$(CXX) -o ad_gen ad_gen.o parser.o

goanna0: parser.o goanna0.o 
	$(CXX) -o goanna0 goanna0.o parser.o

first_seen: parser.o first_seen.o 
	$(CXX) -o first_seen first_seen.o parser.o

first_seen_pebs: parser.o first_seen_pebs.o 
	$(CXX) -o first_seen_pebs first_seen_pebs.o parser.o

random: parser.o random.o 
	$(CXX) -o random random.o parser.o

random_pebs: parser.o random_pebs.o 
	$(CXX) -o random_pebs random_pebs.o parser.o

hfa: parser.o hotness_vs_first_access.o
	$(CXX) -o hfa hotness_vs_first_access.o parser.o

dynamic_lfu: parser.o dynamic_lfu.o 
	$(CXX) -o dynamic_lfu dynamic_lfu.o parser.o

dynamic_lfu_debug: parser.o dynamic_lfu_debug.o 
	$(CXX) -o dynamic_lfu_debug dynamic_lfu_debug.o parser.o

dynamic_lfu_random: parser.o dynamic_lfu_random.o 
	$(CXX) -o dynamic_lfu_random dynamic_lfu_random.o parser.o

dynamic_lfu_fifo: parser.o dynamic_lfu_fifo.o 
	$(CXX) -o dynamic_lfu_fifo dynamic_lfu_fifo.o parser.o

dynamic_lfu_pebs: parser.o dynamic_lfu_pebs.o 
	$(CXX) -o dynamic_lfu_pebs dynamic_lfu_pebs.o parser.o

dynamic_lfu_hw: parser.o dynamic_lfu_hw.o 
	$(CXX) -o dynamic_lfu_hw dynamic_lfu_hw.o parser.o

dynamic_lfu_ad: parser.o dynamic_lfu_ad.o 
	$(CXX) -o dynamic_lfu_ad dynamic_lfu_ad.o parser.o

dynamic_lfu_random_ad: parser.o dynamic_lfu_random_ad.o 
	$(CXX) -o dynamic_lfu_random_ad dynamic_lfu_random_ad.o parser.o

dynamic_lfu_fifo_ad: parser.o dynamic_lfu_fifo_ad.o 
	$(CXX) -o dynamic_lfu_fifo_ad dynamic_lfu_fifo_ad.o parser.o

dynamic_lfu_random_pebs: parser.o dynamic_lfu_random_pebs.o 
	$(CXX) -o dynamic_lfu_random_pebs dynamic_lfu_random_pebs.o parser.o

dynamic_lfu_fifo_pebs: parser.o dynamic_lfu_fifo_pebs.o 
	$(CXX) -o dynamic_lfu_fifo_pebs dynamic_lfu_fifo_pebs.o parser.o

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

lru.o: lru.cpp parser.hpp

lru_pebs.o: lru_pebs.cpp parser.hpp

lru_hw.o: lru_hw.cpp parser.hpp

count_pages.o: count_pages.cpp parser.hpp

lfu_pebs.o: lfu_pebs.cpp parser.hpp

pebs.o: pebs.cpp parser.hpp

hw_hotness.o: hw_hotness.cpp parser.hpp

pebs_gen.o: pebs_gen.cpp parser.hpp

ad.o: ad.cpp parser.hpp

ad_gen.o: ad_gen.cpp parser.hpp

goanna0.o: goanna0.cpp parser.hpp

first_seen.o: first_seen.cpp parser.hpp

first_seen_pebs.o: first_seen_pebs.cpp parser.hpp

random.o: random.cpp parser.hpp

random_pebs.o: random_pebs.cpp parser.hpp

dynamic_lfu.o: dynamic_lfu.cpp parser.hpp

dynamic_lfu_random.o: dynamic_lfu_random.cpp parser.hpp

dynamic_lfu_fifo.o: dynamic_lfu_fifo.cpp parser.hpp

dynamic_lfu_pebs.o: dynamic_lfu_pebs.cpp parser.hpp

dynamic_lfu_hw.o: dynamic_lfu_hw.cpp parser.hpp

dynamic_lfu_ad.o: dynamic_lfu_ad.cpp parser.hpp

dynamic_lfu_fifo_ad.o: dynamic_lfu_fifo_ad.cpp parser.hpp

dynamic_lfu_random_ad.o: dynamic_lfu_random_ad.cpp parser.hpp

dynamic_lfu_random_pebs.o: dynamic_lfu_random_pebs.cpp parser.hpp

dynamic_lfu_fifo_pebs.o: dynamic_lfu_fifo_pebs.cpp parser.hpp

working_set_size.o: working_set_size.cpp parser.hpp

hotness_vs_first_access.o: hotness_vs_first_access.cpp parser.hpp

segmented_lfu.o: segmented_lfu.cpp parser.hpp

create_slfu_file.o: create_slfu_file.cpp parser.hpp

parser.o: parser.cpp parser.hpp

compare.o: compare.cpp parser.hpp

pagemap_read.o: pagemap_read.cpp
