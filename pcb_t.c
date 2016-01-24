/** Pooled CritBit - Unit test.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#include "pcb.h"
#include "scunit/scunit.h"
#include <stdlib.h>
#include <string.h>

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

TEST(Basic)
{
    pcb_t *t = pcb_create();
    ASSERT_NE(NULL, t);
    ASSERT_EQ(1, pcb_add(&t, "AAA"));
    ASSERT_EQ(1, pcb_add(&t, "AAB"));
    ASSERT_EQ(0, pcb_add(&t, "AAA"));
    ASSERT_EQ(1, pcb_in(t, "AAA"));
    ASSERT_EQ(0, pcb_in(t, "AAC"));
    ASSERT_EQ(1, pcb_in(t, "AAB"));
    ASSERT_EQ(1, pcb_rem(t, "AAA"));
    ASSERT_EQ(0, pcb_rem(t, "AAA"));
    ASSERT_EQ(0, pcb_in(t, "AAA"));
    ASSERT_EQ(1, pcb_in(t, "AAB"));
    ASSERT_EQ(1, pcb_rem(t, "AAB"));
    ASSERT_EQ(0, pcb_rem(t, "AAB"));
    ASSERT_EQ(0, pcb_in(t, "AAA"));
    ASSERT_EQ(0, pcb_in(t, "AAB"));
    ASSERT_EQ(1, pcb_add(&t, "AAA"));
    ASSERT_EQ(1, pcb_add(&t, "AAB"));
    ASSERT_EQ(0, pcb_add(&t, "AAA"));
    ASSERT_EQ(0, strcmp(pcb_find_next(t, ""), "AAA"));
    ASSERT_EQ(0, strcmp(pcb_find_next(t, "AAA"), "AAB"));
    ASSERT_EQ(NULL, pcb_find_next(t, "AAB"));
    pcb_destroy(t);
}

TEST(PrimeDecimals)
{
    pcb_t *t = pcb_create();
    ASSERT_NE(NULL, t);
    for (int i = 1; i < 1000000; i++)
    {
        if (!_is_prime(i))
            continue;
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_EQ(1, pcb_add(&t, buffer));
    }
    for (int i = 1000000 - 1; i >= 1; i--)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_EQ(_is_prime(i), pcb_in(t, buffer));
    }
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_NE(_is_prime(i), pcb_add(&t, buffer));
    }
    for (int i = 1000000 - 1; i >= 1; i--)
    {
        if (!_is_prime(i))
            continue;
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_EQ(1, pcb_rem(t, buffer));
    }
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_NE(_is_prime(i), pcb_rem(t, buffer));
    }
    pcb_destroy(t);
}

TEST(LexNextTests)
{
    char **ref = malloc(1000000 * sizeof(char *));
    for (int i = 0; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        ref[i] = malloc(strlen(buffer) + 1);
        strcpy(ref[i], buffer);
    }
    qsort(ref, 1000000, sizeof(char *), _str_sort_cmp);
    pcb_t *t = pcb_create();
    ASSERT_NE(NULL, t);
    for (int i = 0; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        ASSERT_EQ(1, pcb_add(&t, buffer));
    }
    ASSERT_EQ(NULL, pcb_find_next(t, "999999"));
    for (int i = 0; i < 999999; i++)
        ASSERT_EQ(0, strcmp(pcb_find_next(t, ref[i]), ref[i+1]));
    pcb_destroy(t);
    for (int i = 0; i < 1000000; i++)
        free(ref[i]);
    free(ref);
}

TEST(ReadAfter0Tests)
{
    pcb_t *t = pcb_create();
    ASSERT_NE(NULL, t);
    ASSERT_EQ(1, pcb_add(&t, "AAA"));
    ASSERT_EQ(1, pcb_add(&t, "AAB"));
    ASSERT_EQ(1, pcb_add(&t, (char []){ 'A', 0x00, 0xff, 0xff }));
    ASSERT_EQ(0, strcmp(pcb_find_next(t, ""), "A"));
    pcb_destroy(t);
}

TEST(WalkTests)
{
    pcb_t *t = pcb_create();
    unsigned long long tgt_sum = 0;
    unsigned long long cb_sum = 0;
    for (int i = 1; i < 1000000; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d", i);
        if (buffer[0] == '2' && buffer[1] == '7')
            tgt_sum += i;
        ASSERT_EQ(1, pcb_add(&t, buffer));
    }
    ASSERT_EQ(1, pcb_find_suffixes(t, "27", _sum_cb, &cb_sum));
    ASSERT_EQ(tgt_sum, cb_sum);
    pcb_destroy(t);
}
