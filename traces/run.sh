sudo  ~/page_migration/memory_pin/pin/pin -t ~/obj-intel64/memtrace.so -- 557.xz_r/run/run_base_refrate_mytest-m64.0000/xz_r_base.mytest-m64 557.xz_r/run/run_base_refrate_mytest-m64.0000/input.combined.xz 250 a841f68f38572a49d86226b7ff5baeb31bd19dc637a922a972b2e6d1257a890f6a544ecab967c313e370478c74f760eb229d4eef8a8d2836d233d3e9dd1430bf 40401484 41217675 7
mv memtrace.out memtrace-input.out
sudo ~/page_migration/memory_pin/pin/pin -t ~/obj-intel64/memtrace.so -- 519.lbm_r/run/run_base_refrate_mytest-m64.0000/lbm_r_base.mytest-m64 3000 reference.dat 0 0 519.lbm_r/run/run_base_refrate_mytest-m64.0000/100_100_130_ldc.of
mv memtrace.out memtrace-lbm.out
