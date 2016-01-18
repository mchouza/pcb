/** Pooled CritBit Benchmark - Per implementation test.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

/* each implementation test is in a separate block to avoid collisions */
{
    /* defines the critbit object */
    PCBB_CB_DEF(cb);

    /*
     * BLT suite tests
     */

    /* initializes the critbit */
    PCBB_TIMER_START();
    PCBB_CB_INIT(cb);
    PCBB_TIMER_END("init");

    /* loads all the keys */
    PCBB_TIMER_START();
    for (size_t i = 0; i < blt_suite_num_keys; i++ )
        PCBB_CB_ADD(cb, blt_suite_keys[i]);
    PCBB_TIMER_END("add");

    /* retrieves all the keys */
    PCBB_TIMER_START();
    for (size_t i = 0; i < blt_suite_num_keys; i++ )
        PCBB_CB_GET(cb, blt_suite_keys[i]);
    PCBB_TIMER_END("get");

    /* iterates over all keys */
    PCBB_TIMER_START();
    PCBB_CB_IT_DEF(it);
    for (it = PCBB_CB_FIRST(cb); it; it = PCBB_CB_NEXT(cb, it));
    PCBB_TIMER_END("iterate");

    /* iterates using a callback */
    PCBB_CB_CB_FUNC_DEF(my_cb, _gen_cb);
    PCBB_TIMER_START();
    PCBB_CB_ALL_SUFFIXES(cb, "", my_cb);
    PCBB_TIMER_END("all_suffixes");

    /* deletes all the keys */
    PCBB_TIMER_START();
    for (size_t i = 0; i < blt_suite_num_keys; i++ )
        PCBB_CB_DELETE(cb, blt_suite_keys[i]);
    PCBB_TIMER_END("delete");

    /* releases the critbit */
    PCBB_TIMER_START();
    PCBB_CB_RELEASE(cb);
    PCBB_TIMER_END("release");
}
