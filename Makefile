PCB_INC = .
PCB_HDR = pcb.h
PCB_SRC = pcb.c
PCB_TST = pcb_t.c
SCUNIT_HDR = scunit/scunit.h
SCUNIT_SRC = scunit/scunit.c
MFCB_INC = mfcb
MFCB_HDR = mfcb/mfcb.h
MFCB_SRC = mfcb/mfcb.c
BLT_INC = third-party/blt
BLT_HDR = third-party/blt/blt.h
BLT_SRC = third-party/blt/blt.c
BENCHMARK_HDR = benchmark.inc
BENCHMARK_SRC = benchmarks.c

pcb_test: $(PCB_HDR) $(PCB_SRC) $(PCB_TST) $(SCUNIT_HDR) $(SCUNIT_SRC)
	gcc -Wall -std=c11 -g -O3 $(PCB_SRC) $(PCB_TST) $(SCUNIT_SRC) -o $@

test: pcb_test
	./pcb_test

valgrind: pcb_test
	valgrind ./pcb_test

mfcb.benchmark: $(MFCB_HDR) $(MFCB_SRC) $(BENCHMARK_HDR) $(BENCHMARK_SRC)
	gcc -Wall -DBENCH_MFCB=1 -std=c11 -O3 -I$(MFCB_INC) $(MFCB_SRC) $(BENCHMARK_SRC) -o $@

blt.benchmark: $(BLT_HDR) $(BLT_SRC) $(BENCHMARK_HDR) $(BENCHMARK_SRC)
	gcc -Wall -DBENCH_BLT=1 -std=gnu11 -O3 -I$(BLT_INC) $(BLT_SRC) $(BENCHMARK_SRC) -o $@

pcb.benchmark: $(PCB_HDR) $(PCB_SRC) $(BENCHMARK_HDR) $(BENCHMARK_SRC)
	gcc -Wall -DBENCH_PCB=1 -std=c11 -O3 -I$(PCB_INC) $(PCB_SRC) $(BENCHMARK_SRC) -o $@

benchmark: mfcb.benchmark blt.benchmark pcb.benchmark
	./mfcb.benchmark
	./blt.benchmark
	./pcb.benchmark

callgrind: benchmark_exec
	valgrind --tool=callgrind --dump-instr=yes --trace-jump=yes --callgrind-out-file=callgrind.out ./benchmark_exec

clean:
	rm -f pcb_test callgrind.out *.benchmark

.PHONY: test valgrind callgrind benchmark clean