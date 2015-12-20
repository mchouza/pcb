#define _GNU_SOURCE

#include "mfcb.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef TIMING_TEST
    #define START_TIMING() unsigned long long _test_start = _get_timestamp()
    #define STOP_TIMING(msg) do { printf("%s %llu\n", (msg), _get_timestamp() - _test_start); } while(0)
    static unsigned long long _get_timestamp(void)
    {
        struct timespec t;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
        return t.tv_sec * 1000000000ull + t.tv_nsec;
    }
#else
    #define START_TIMING()
    #define STOP_TIMING(msg)
#endif

static int _is_prime(unsigned a)
{
    if (a != 2 && a % 2 == 0)
        return 0;
    for (unsigned d = 3; d * d <= a; d += 2)
        if (a % d == 0)
            return 0;
    return 1;
}

static int _str_sort_cmp(const void *p, const void *q)
{
    return strcmp(*(const char **)p, *(const char **)q);
}

static int _sum_cb(const char *s, void *ctx)
{
    unsigned long long ull = 0;
    sscanf(s, "%llu", &ull);
    *(unsigned long long *)ctx += ull;
    return 1;
}

static void _basic_tests(void)
{
    mfcb_t cbt = { 0 };
    assert(mfcb_add(&cbt, "AAA") == 1);
    assert(mfcb_add(&cbt, "AAB") == 1);
    assert(mfcb_add(&cbt, "AAA") == 0);
    assert(mfcb_contains(&cbt, "AAA") == 1);
    assert(mfcb_contains(&cbt, "AAC") == 0);
    assert(mfcb_contains(&cbt, "AAB") == 1);
    assert(mfcb_rem(&cbt, "AAA") == 1);
    assert(mfcb_rem(&cbt, "AAA") == 0);
    assert(mfcb_contains(&cbt, "AAA") == 0);
    assert(mfcb_contains(&cbt, "AAB") == 1);
    assert(mfcb_rem(&cbt, "AAB") == 1);
    assert(mfcb_rem(&cbt, "AAB") == 0);
    assert(mfcb_contains(&cbt, "AAA") == 0);
    assert(mfcb_contains(&cbt, "AAB") == 0);
    assert(mfcb_add(&cbt, "AAA") == 1);
    assert(mfcb_add(&cbt, "AAB") == 1);
    assert(mfcb_add(&cbt, "AAA") == 0);
    assert(strcmp(mfcb_find(&cbt, ""), "AAA") == 0);
    assert(strcmp(mfcb_find(&cbt, "AAA"), "AAB") == 0);
    assert(mfcb_find(&cbt, "AAB") == NULL);
    mfcb_clear(&cbt);
}

static void _prime_tests(void)
{
    START_TIMING();
    mfcb_t cbt = { 0 };
    for (int i = 1; i < 1000000; i++)
    {
        if (!_is_prime(i))
            continue;
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_add(&cbt, buffer) == 1);
    }
    for (int i = 1000000 - 1; i >= 1; i--)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_contains(&cbt, buffer) == _is_prime(i));
    }
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_add(&cbt, buffer) != _is_prime(i));
    }
    for (int i = 1000000 - 1; i >= 1; i--)
    {
        if (!_is_prime(i))
            continue;
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_rem(&cbt, buffer) == 1);
    }
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_rem(&cbt, buffer) != _is_prime(i));
    }
    STOP_TIMING("prime_tests");
    mfcb_clear(&cbt);
}

static void _lex_next_tests(void)
{
    START_TIMING();
    mfcb_t cbt = { 0 };
    char **ref = malloc(1000000 * sizeof(char *));
    for (int i = 0; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        assert(mfcb_add(&cbt, buffer) == 1);
        ref[i] = malloc(strlen(buffer) + 1);
        strcpy(ref[i], buffer);
    }
    qsort(ref, 1000000, sizeof(char *), _str_sort_cmp);
    assert(strcmp(mfcb_find(&cbt, ""), "0") == 0);
    assert(mfcb_find(&cbt, "999999") == NULL);
    for (int i = 0; i < 999999; i++)
        assert(strcmp(mfcb_find(&cbt, ref[i]), ref[i+1]) == 0);
    for (int i = 0; i < 1000000; i++)
        free(ref[i]);
    free(ref);
    STOP_TIMING("lex_next_tests");
    mfcb_clear(&cbt);
}

static void _walk_tests(void)
{
    START_TIMING();
    mfcb_t cbt = { 0 };
    unsigned long long tgt_sum = 0;
    unsigned long long cb_sum = 0;
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        if (buffer[0] == '2' && buffer[1] == '7')
            tgt_sum += i;
        assert(mfcb_add(&cbt, buffer) == 1);
    }
    assert(mfcb_find_suffixes(&cbt, "27", _sum_cb, &cb_sum) == 1);
    assert(tgt_sum == cb_sum);
    STOP_TIMING("walk_tests");
    mfcb_clear(&cbt);
}

static void _read_after_0_tests(void)
{
    mfcb_t cbt = { 0 };
    assert(mfcb_add(&cbt, "AAA") == 1);
    assert(mfcb_add(&cbt, "AAB") == 1);
    assert(mfcb_add(&cbt, (char []){ 'A', 0x00, 0xff, 0xff }) == 1);
    assert(strcmp(mfcb_find(&cbt, ""), "A") == 0);
    mfcb_clear(&cbt);
}

int main(void)
{
    _basic_tests();
    _prime_tests();
    _lex_next_tests();
    _walk_tests();
    _read_after_0_tests();
    return 0;
}
