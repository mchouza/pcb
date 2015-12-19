#include "mfcb.h"
#include <assert.h>
#include <stdio.h>

static int _is_prime(unsigned a)
{
    if (a != 2 && a % 2 == 0)
        return 0;
    for (unsigned d = 3; d * d <= a; d += 2)
        if (a % d == 0)
            return 0;
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
}

static void _prime_tests(void)
{
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
}

int main(void)
{
    _basic_tests();
    _prime_tests();
    return 0;
}