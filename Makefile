pcb_test: pcb.c pcb.h pcb_t.c scunit/scunit.h scunit/scunit.c
	gcc -Wall -std=c11 -O3 -g pcb.c pcb_t.c scunit/scunit.c -o $@

test: pcb_test
	./pcb_test

valgrind: pcb_test
	valgrind ./pcb_test

callgrind: pcb_test
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./pcb_test

clean:
	rm -f pcb_test callgrind.out

.PHONY: test valgrind callgrind clean