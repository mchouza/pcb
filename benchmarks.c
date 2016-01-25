/** Pooled CritBit Benchmark - General test.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#define _GNU_SOURCE

#include "mfcb/mfcb.h"
#include "third-party/blt/blt.h"
#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/** Number of iterations to do. */
#ifndef NUM_ITERS
    #define NUM_ITERS 3
#endif

/** Whether to benchmark MFCB. */
#ifndef BENCH_MFCB
    #define BENCH_MFCB 0
#endif

/** Whether to benchmark BLT. */
#ifndef BENCH_BLT
    #define BENCH_BLT 0
#endif

/** Whether to benchmark PCB. */
#ifndef BENCH_PCB
    #define BENCH_PCB 0
#endif


/** BLT suite number of sequential keys. */
#define BLT_SUITE_NUM_SEQ_KEYS 2000000


/** Gets a timestamp of the process time.
 *
 *  \return Timestamp of the process time.
 */
static unsigned long long _get_timestamp(void)
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return t.tv_sec * 1000000000ull + t.tv_nsec;
}


/** Initializes the BLT suite keys.
 *
 *  \param blt_suite_num_keys Number of BLT suite keys (output).
 *  \param blt_suite_keys BLT suite keys (output).
 */
static void _init_blt_suite_keys(size_t *blt_suite_num_keys, char ***blt_suite_keys)
{
    /* loads the keys */
    char key[256];
    *blt_suite_num_keys = BLT_SUITE_NUM_SEQ_KEYS;
    *blt_suite_keys = malloc(BLT_SUITE_NUM_SEQ_KEYS * sizeof(char *));
    for (size_t i = 0; i < BLT_SUITE_NUM_SEQ_KEYS; i++)
    {
        snprintf(key, sizeof(key), "%zu", i);
        (*blt_suite_keys)[i] = malloc(strlen(key) + 1);
        strcpy((*blt_suite_keys)[i], key);
    }

    /* shuffles them using Fisher-Yates:
       -- To shuffle an array a of n elements (indices 0..n-1):
       for i from 0 to n−2 do
         j ← random integer such that 0 ≤ j < n-i
         exchange a[i] and a[i+j] */
    srand(1234);
    for (size_t i = 0; i < BLT_SUITE_NUM_SEQ_KEYS - 1; i++)
    {
        size_t j = rand() % (BLT_SUITE_NUM_SEQ_KEYS - i);
        char *tmp = (*blt_suite_keys)[i];
        (*blt_suite_keys)[i] = (*blt_suite_keys)[i + j];
        (*blt_suite_keys)[i + j] = tmp;
    }
}


/** Releases the BLT suite keys.
 *
 *  \param blt_suite_num_keys Number of BLT suite keys (input/output).
 *  \param blt_suite_keys BLT suite keys (input/output).
 */
static void _release_blt_suite_keys(size_t *blt_suite_num_keys, char ***blt_suite_keys)
{
    for (size_t i = 0; i < *blt_suite_num_keys; i++)
        free((*blt_suite_keys)[i]);
    free(*blt_suite_keys);
    *blt_suite_keys = NULL;
    *blt_suite_num_keys = 0;
}


/** Generic callback (it cannot read parameters).
 *
 *  \return Always 1, to keep the iteration.
 */
static int _gen_cb(void)
{
    return 1;
}


/** Benchmark entry point.
 *
 *  \return 0 if successful, 1 otherwise.
 */
int main(void)
{
    /* general start timestamp */
    unsigned long long start;

    /* the timer start definition is shared */
    #define PCBB_TIMER_START() start = _get_timestamp()

    /* the generic timer end definition is shared */
    #define PCBB_TIMER_GEN_END(test_str, timer_str)\
        do { printf("%s %llu\n", test_str "_" timer_str, _get_timestamp() - start); } while(0)

    /* BLT suite keys */
    size_t blt_suite_num_keys = 0;
    char **blt_suite_keys = NULL;
    _init_blt_suite_keys(&blt_suite_num_keys, &blt_suite_keys);

    /* iterates all tests many times */
    for (size_t i = 0; i < NUM_ITERS; i++)
    {
        /* MFCB test */
        #if BENCH_MFCB
        #define PCBB_CB_DEF(id) mfcb_t id = { 0 }
        #define PCBB_CB_IT_DEF(id) const char *id = NULL
        #define PCBB_CB_CB_FUNC_DEF(id, f) int (*id)(const char *, void *) = (int (*)(const char *, void *))f
        #define PCBB_CB_INIT(id) 
        #define PCBB_CB_ADD(id, s) mfcb_add(&id, s)
        #define PCBB_CB_GET(id, s) mfcb_contains(&id, s)
        #define PCBB_CB_FIRST(id) mfcb_find(&id, "")
        #define PCBB_CB_NEXT(id, it) mfcb_find(&id, it)
        #define PCBB_CB_ALL_SUFFIXES(id, s, cb) mfcb_find_suffixes(&id, s, cb, NULL)
        #define PCBB_CB_DELETE(id, s) mfcb_rem(&id, s)
        #define PCBB_CB_RELEASE(id) mfcb_clear(&id)
        #define PCBB_TIMER_END(timer_str) PCBB_TIMER_GEN_END("mfcb", timer_str)
        #include "benchmark.inc"
        #undef PCBB_CB_DEF
        #undef PCBB_CB_IT_DEF
        #undef PCBB_CB_CB_FUNC_DEF
        #undef PCBB_CB_INIT
        #undef PCBB_CB_ADD
        #undef PCBB_CB_GET
        #undef PCBB_CB_FIRST
        #undef PCBB_CB_NEXT
        #undef PCBB_CB_ALL_SUFFIXES
        #undef PCBB_CB_DELETE
        #undef PCBB_CB_RELEASE
        #undef PCBB_TIMER_END
        #endif

        /* BLT test */
        #if BENCH_BLT
        #define PCBB_CB_DEF(id) BLT *id = NULL
        #define PCBB_CB_IT_DEF(id) BLT_IT *id = NULL
        #define PCBB_CB_CB_FUNC_DEF(id, f) int (*id)(BLT_IT *) = (int (*)(BLT_IT *))f
        #define PCBB_CB_INIT(id) id = blt_new()
        #define PCBB_CB_ADD(id, s) blt_set(id, s)
        #define PCBB_CB_GET(id, s) blt_get(id, s)
        #define PCBB_CB_FIRST(id) blt_first(id)
        #define PCBB_CB_NEXT(id, it) blt_next(id, it)
        #define PCBB_CB_ALL_SUFFIXES(id, s, cb) blt_allprefixed(id, s, cb)
        #define PCBB_CB_DELETE(id, s) blt_delete(id, s)
        #define PCBB_CB_RELEASE(id) blt_clear(id)
        #define PCBB_TIMER_END(timer_str) PCBB_TIMER_GEN_END("blt", timer_str)
        #include "benchmark.inc"
        #undef PCBB_CB_DEF
        #undef PCBB_CB_IT_DEF
        #undef PCBB_CB_CB_FUNC_DEF
        #undef PCBB_CB_INIT
        #undef PCBB_CB_ADD
        #undef PCBB_CB_GET
        #undef PCBB_CB_FIRST
        #undef PCBB_CB_NEXT
        #undef PCBB_CB_ALL_SUFFIXES
        #undef PCBB_CB_DELETE
        #undef PCBB_CB_RELEASE
        #undef PCBB_TIMER_END
        #endif

        /* PCB test */
        #if BENCH_PCB
        #define PCBB_CB_DEF(id) pcb_t *id = NULL
        #define PCBB_CB_IT_DEF(id) const char *id = NULL
        #define PCBB_CB_CB_FUNC_DEF(id, f) int (*id)(const char *, void *) = (int (*)(const char *, void *))f
        #define PCBB_CB_INIT(id) id = pcb_create()
        #define PCBB_CB_ADD(id, s) pcb_add(&id, s)
        #define PCBB_CB_GET(id, s) pcb_in(id, s)
        #define PCBB_CB_FIRST(id) pcb_find_next(id, "")
        #define PCBB_CB_NEXT(id, it) pcb_find_next(id, it)
        #define PCBB_CB_ALL_SUFFIXES(id, s, cb) pcb_find_suffixes(id, s, cb, NULL)
        #define PCBB_CB_DELETE(id, s) pcb_rem(id, s)
        #define PCBB_CB_RELEASE(id) pcb_destroy(id)
        #define PCBB_TIMER_END(timer_str) PCBB_TIMER_GEN_END("pcb", timer_str)
        #include "benchmark.inc"
        #undef PCBB_CB_DEF
        #undef PCBB_CB_IT_DEF
        #undef PCBB_CB_CB_FUNC_DEF
        #undef PCBB_CB_INIT
        #undef PCBB_CB_ADD
        #undef PCBB_CB_GET
        #undef PCBB_CB_FIRST
        #undef PCBB_CB_NEXT
        #undef PCBB_CB_ALL_SUFFIXES
        #undef PCBB_CB_DELETE
        #undef PCBB_CB_RELEASE
        #undef PCBB_TIMER_END
        #endif
    }

    /* releases the BLT suite keys */
    _release_blt_suite_keys(&blt_suite_num_keys, &blt_suite_keys);
}
