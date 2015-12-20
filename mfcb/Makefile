mfcb_test: mfcb.c mfcb.h mfcb_test.c
	gcc -Wall -std=c11 -O3 -g mfcb.c mfcb_test.c -o $@

mfcb_timing_test: mfcb.c mfcb.h mfcb_test.c
	gcc -Wall -std=c11 -O3 -DTIMING_TEST mfcb.c mfcb_test.c -o $@

test: mfcb_test
	./mfcb_test

timing: mfcb_timing_test
	./mfcb_timing_test

valgrind: mfcb_test
	valgrind ./mfcb_test

callgrind: mfcb_test
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./mfcb_test

clean:
	rm -f mfcb_test mfcb_timing_test callgrind.out

.PHONY: test timing valgrind callgrind clean