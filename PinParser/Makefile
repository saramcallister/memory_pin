CXX=g++
CXXFLAGS=-g -Wall -std=c++17
SRC=./src

all: lfu \
	dynamic_lfu \
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
    dynamic_lfu_fifo_ad 

lfu: parser.o lfu.o 
	$(CXX) -o lfu lfu.o parser.o

count_pages: parser.o count_pages.o 
	$(CXX) -o count_pages count_pages.o parser.o

lfu_pebs: parser.o lfu_pebs.o 
	$(CXX) -o lfu_pebs lfu_pebs.o parser.o

pebs: parser.o pebs.o 
	$(CXX) -o pebs pebs.o parser.o

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

dynamic_lfu_random: parser.o dynamic_lfu_random.o 
	$(CXX) -o dynamic_lfu_random dynamic_lfu_random.o parser.o

dynamic_lfu_fifo: parser.o dynamic_lfu_fifo.o 
	$(CXX) -o dynamic_lfu_fifo dynamic_lfu_fifo.o parser.o

dynamic_lfu_pebs: parser.o dynamic_lfu_pebs.o 
	$(CXX) -o dynamic_lfu_pebs dynamic_lfu_pebs.o parser.o

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

lfu.o: $(SRC)/lfu.cpp $(SRC)/parser.hpp

count_pages.o: $(SRC)/count_pages.cpp $(SRC)/parser.hpp

lfu_pebs.o: $(SRC)/lfu_pebs.cpp $(SRC)/parser.hpp

pebs.o: $(SRC)/pebs.cpp $(SRC)/parser.hpp

pebs_gen.o: $(SRC)/pebs_gen.cpp $(SRC)/parser.hpp

ad.o: $(SRC)/ad.cpp $(SRC)/parser.hpp

ad_gen.o: $(SRC)/ad_gen.cpp $(SRC)/parser.hpp

goanna0.o: $(SRC)/goanna0.cpp $(SRC)/parser.hpp

first_seen.o: $(SRC)/first_seen.cpp $(SRC)/parser.hpp

first_seen_pebs.o: $(SRC)/first_seen_pebs.cpp $(SRC)/parser.hpp

random.o: $(SRC)/random.cpp $(SRC)/parser.hpp

random_pebs.o: $(SRC)/random_pebs.cpp $(SRC)/parser.hpp

dynamic_lfu.o: $(SRC)/dynamic_lfu.cpp $(SRC)/parser.hpp

dynamic_lfu_random.o: $(SRC)/dynamic_lfu_random.cpp $(SRC)/parser.hpp

dynamic_lfu_fifo.o: $(SRC)/dynamic_lfu_fifo.cpp $(SRC)/parser.hpp

dynamic_lfu_pebs.o: $(SRC)/dynamic_lfu_pebs.cpp $(SRC)/parser.hpp

dynamic_lfu_ad.o: $(SRC)/dynamic_lfu_ad.cpp $(SRC)/parser.hpp

dynamic_lfu_fifo_ad.o: $(SRC)/dynamic_lfu_fifo_ad.cpp $(SRC)/parser.hpp

dynamic_lfu_random_ad.o: $(SRC)/dynamic_lfu_random_ad.cpp $(SRC)/parser.hpp

dynamic_lfu_random_pebs.o: $(SRC)/dynamic_lfu_random_pebs.cpp $(SRC)/parser.hpp

dynamic_lfu_fifo_pebs.o: $(SRC)/dynamic_lfu_fifo_pebs.cpp $(SRC)/parser.hpp

working_set_size.o: $(SRC)/working_set_size.cpp $(SRC)/parser.hpp

hotness_vs_first_access.o: $(SRC)/hotness_vs_first_access.cpp $(SRC)/parser.hpp

segmented_lfu.o: $(SRC)/segmented_lfu.cpp $(SRC)/parser.hpp

create_slfu_file.o: $(SRC)/create_slfu_file.cpp $(SRC)/parser.hpp

parser.o: $(SRC)/parser.cpp $(SRC)/parser.hpp

compare.o: $(SRC)/compare.cpp $(SRC)/parser.hpp

pagemap_read.o: $(SRC)/pagemap_read.cpp
