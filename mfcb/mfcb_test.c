#include "mfcb.h"
#include <stdio.h>

int main(void)
{
    mfcb_t cbt = { 0 };
    mfcb_add(&cbt, "AAA");
    mfcb_add(&cbt, "AAB");
    printf("%d\n", mfcb_contains(&cbt, "AAA"));
    printf("%d\n", mfcb_contains(&cbt, "AAC"));
    printf("%d\n", mfcb_contains(&cbt, "AAB"));
    return 0;
}