# memory_pin

MemoryTrace:
- download Intel pin to top level dir
- from MemoryTrace dir: `make PIN_ROOT=../$(PIN_DIR) obj-intel64/memtrace.so`
- `/path/to/memory_pin/$(PIN_DIR)/pin -t $(OUTPUT_FILE) -- $(PROGRAM)`


PinParser: `make`
