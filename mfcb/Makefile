mfcb_test: mfcb.c mfcb.h mfcb_test.c
	gcc -Wall -std=c11 -O3 mfcb.c mfcb_test.c -o $@

test: mfcb_test
	./mfcb_test

clean:
	rm -f mfcb_test

.PHONY: test clean