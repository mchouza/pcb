scunit_test: ../scunit.c ../scunit.h scunit_1_t.c scunit_2_t.c
	gcc -Wall -std=c11 -O3 -I ../.. ../scunit.c scunit_1_t.c scunit_2_t.c -o $@

test: scunit_test
	./scunit_test

valgrind: scunit_test
	valgrind ./scunit_test

clean:
	rm -f scunit_test

.PHONY: test valgrind clean