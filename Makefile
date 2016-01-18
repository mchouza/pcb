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
	gcc -Wall -std=c11 -O3 $(PCB_SRC) $(PCB_TST) $(SCUNIT_SRC) -o $@

test: pcb_test
	./pcb_test

valgrind: pcb_test
	valgrind ./pcb_test

callgrind: pcb_test
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./pcb_test

benchmark_exec: $(MFCB_HDR) $(MFCB_SRC) $(BLT_HDR) $(BLT_SRC) $(BENCHMARK_HDR) $(BENCHMARK_SRC)
	gcc -Wall -std=gnu11 -O3 -I$(MFCB_INC) -I$(BLT_INC) $(MFCB_SRC) $(BLT_SRC) $(BENCHMARK_SRC) -o $@

benchmark: benchmark_exec
	./benchmark_exec

clean:
	rm -f pcb_test callgrind.out benchmark_exec

.PHONY: test valgrind callgrind benchmark clean