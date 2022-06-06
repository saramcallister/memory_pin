#include <algorithm>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_set>
#include <vector>

#define PAGEMAP_FILE "/proc/pid/pagemap"

std::unordered_set<uint64_t> pages;

struct PagemapEntry {
    uint64_t pfn : 54;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
};

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr) {
	size_t nread;
	ssize_t ret;
	uint64_t data;

	nread = 0;
	while (nread < sizeof(data)) {
		ret = pread(pagemap_fd, &data, sizeof(data),
						(vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
		nread += ret;
		if (ret <= 0) {
			return 1;
		}
	}
	entry->pfn = data & (((uint64_t)1 << 54) - 1);
	entry->soft_dirty = (data >> 54) & 1;
	entry->file_page = (data >> 61) & 1;
	entry->swapped = (data >> 62) & 1;
	entry->present = (data >> 63) & 1;
	return 0;
}

void handle_virtual_range(int pagemap, uint64_t start_address, uint64_t end_address) {
	//printf("Start: %lx, end: %lx\n", start_address, end_address);
	for (uint64_t i = start_address; i < end_address; i += 0x1000) {
		PagemapEntry pe;
		int ret = pagemap_get_entry(&pe, pagemap, i);
		if (ret) {
			break;
		}
		if (pe.present) {
			pages.insert(i);
		}
	}
}

void parse_maps(const char *maps_file, const char *pagemap_file) {
	int maps = open(maps_file, O_RDONLY);
	if (maps < 0) return;

	int pagemap = open(pagemap_file, O_RDONLY);
	if (pagemap < 0) {
		close(maps);
		return;
	}

	char buffer[BUFSIZ];
	int offset = 0;

	for (;;) {
		ssize_t length = read(maps, buffer + offset, sizeof(buffer) - offset);
		if (length <= 0) break;

		length += offset;

		for (size_t i = offset; i < (size_t) length; i++) {
			uint64_t low = 0, high = 0;
			if (buffer[i] == '\n' && i) {
				size_t x = i - 1;
				while(x && buffer[x] != '\n') x--;
				if (buffer[x] == '\n') x++;

				// parse lower bound hex
				while(buffer[x] != '-' && x+1 < sizeof(buffer)) {
					char c = buffer[x++];
					low *= 16;
					if (c >= '0' && c <= '9') {
						low += c - '0';
					} else if (c >= 'a' && c <= 'f') {
						low += c - 'a' + 10;
					} else break;
				}

				while(buffer[x] != '-' && x + 1 < sizeof(buffer)) x++;
				if(buffer[x]== '-') x++;

				// parse upper bound hex
				while(buffer[x] != ' ' && x + 1 < sizeof(buffer)) {
					char c = buffer[x++];
					high *= 16;
					if (c >= '0' && c <= '9') {
						high += c - '0';
					} else if (c >= 'a' && c <= 'f') {
						high += c - 'a' + 10;
					} else {
						break;
					}
				}

				handle_virtual_range(pagemap, low, high);
			}
		}
		
	}
	close(maps);
	close(pagemap);

}

void process_pid(pid_t pid) {
	char maps_file[BUFSIZ];
	char pagemap_file[BUFSIZ];
	snprintf(maps_file, sizeof(maps_file), "/proc/%lu/maps", (uint64_t)pid);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%lu/pagemap", (uint64_t)pid);

	parse_maps(maps_file, pagemap_file);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s pid1 outfile\n", argv[0]);
		return 1;
	}

	pages.reserve(200000);

	pid_t pid = (pid_t)strtoul(argv[1], NULL, 0);

	FILE* outfile = fopen(argv[2], "w");
	if (outfile == NULL) {
	    printf("Failed to open %s", argv[2]);
	    return 1;
	}


	int i = 0;
	while (0 == kill(pid, 0)) {
		process_pid(pid);
		i++;
	}
	printf("== Valid pages for pid %d\n", (int) pid);
	printf("Ran %d times\n", i);
	printf("Found %ld pages\n", pages.size());

	unsigned long s = pages.size();
	
	/* move to vector */
	std::srand(unsigned(std::time(0)));
	std::vector<unsigned long> v(pages.begin(), pages.end());
	std::random_shuffle(v.begin(), v.end());

	fwrite(&s, sizeof(unsigned long), 1, outfile);
	for (auto val: v) {
	    unsigned long shifted = val >> 12;
	    fwrite(&shifted, sizeof(unsigned long), 1, outfile);
	}
	fclose(outfile);

}
