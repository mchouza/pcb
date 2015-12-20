/** Simple C Unit Test Framework - Implementation.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#define SCUNIT_C
#include "scunit.h"
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* protects agains redefinition of main */
#ifdef main
    #undef main
#endif


/** Test node type. */
typedef struct scunit_test_node_t
{
    /** Test function. */
    scunit_test_func_t *test_func;

    /** Test name. */
    const char *test_name;

    /** Test suite name. */
    const char *suite_name;

    /** Next node. */
    struct scunit_test_node_t *next;

} scunit_test_node_t;


/** Tests list. */
static scunit_test_node_t *_tests_list = NULL;


/** Test node comparison function.
 *
 *  \param p1 First node.
 *  \param p2 Second node.
 *  \return Comparison result.
 */
static int _cmp_test_node(const void *p1, const void *p2)
{
    /* gets the node pointers */
    const scunit_test_node_t *n1 = *(const scunit_test_node_t **)p1;
    const scunit_test_node_t *n2 = *(const scunit_test_node_t **)p2;

    /* compares the suite names and test names */
    int suite_names_cmp = strcmp(n1->suite_name, n2->suite_name);
    int test_names_cmp = strcmp(n1->test_name, n2->test_name);

    /* returns the comparison result */
    return (suite_names_cmp != 0) ? suite_names_cmp : test_names_cmp;
}


/** Gets sorted test array.
 *
 *  \return Sorted, \c NULL terminated, test array.
 */
static scunit_test_node_t **_get_test_arr(void)
{
    /* counts the number of tests */
    size_t num_tests = 0;
    for (scunit_test_node_t *n = _tests_list; n != NULL; n = n->next)
        num_tests++;

    /* creates an array of the proper size and fills it */
    size_t i = 0;
    scunit_test_node_t **ret = calloc(num_tests + 1, sizeof(scunit_test_node_t *));
    for (scunit_test_node_t *n = _tests_list; n != NULL; n = n->next)
        ret[i++] = n;

    /* sorts the array */
    qsort(ret, num_tests, sizeof(scunit_test_node_t *), _cmp_test_node);

    /* returns it */
    return ret;
}


/** Runs all tests.
 *
 *  \return 0 if all tests succeeded, 1 otherwise.
 */
static int _run_all_tests(void)
{
    /* error code */
    int error_code = 0;

    /* gets the test array */
    scunit_test_node_t **test_arr = _get_test_arr();

    /* goes over all the tests */
    for (size_t i = 0; test_arr[i] != NULL; i++)
    {
        printf("Executing %s.%s...\n", test_arr[i]->suite_name, test_arr[i]->test_name);
        int test_err_code = 0;
        test_arr[i]->test_func(&test_err_code);
        if (test_err_code != 0)
        {
            printf("Error!!!\n");
            error_code = 1;
        }
        else
        {
            printf("Success.\n");
        }
    }

    /* releases the test array */
    free(test_arr);

    /* returns the error code */
    return error_code;
}


/** Releases the tests list.
 *
 */
static void _release_tests_list(void)
{
    for (scunit_test_node_t *n = _tests_list; n != NULL; )
    {
        scunit_test_node_t *t = n;
        n = n->next;
        free(t);
    }
}


/** Registers a test to be executed.
 *
 *  \param test_func Test function.
 *  \param test_name Test name.
 *  \param suite_name Suite name.
 */
void scunit_register(scunit_test_func_t *test_func, const char *test_name, const char *suite_name)
{
    /* creates a new node */
    scunit_test_node_t *n = malloc(sizeof(scunit_test_node_t));
    n->test_func = test_func;
    n->test_name = test_name;
    n->suite_name = suite_name;

    /* puts it at the beginning of the list */
    n->next = _tests_list;
    _tests_list = n;
}


/** Framework entry point.
 *
 *  \return Program exit code.
 */
int main(void)
{
    /* just runs all tests */
    int exit_code = _run_all_tests();

    /* releases the tests list */
    _release_tests_list();

    /* returns the exit code */
    return exit_code;
}
