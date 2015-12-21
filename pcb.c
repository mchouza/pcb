/** Pooled CritBit - Implementation.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#include "pcb.h"
#include <stdlib.h>


/** Adds a string to the critbit.
 *
 *  \param t Critbit tree.
 *  \param s String to be added.
 *  \return 1 if successful, 0 otherwise.
 */
int pcb_add(pcb_t *t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return 0;
}


/** Removes a string from the critbit.
 *
 *  \param t Critbit tree.
 *  \param s String to be removed.
 *  \return 1 if successful, 0 otherwise.
 */
int pcb_rem(pcb_t *t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return 0;
}


/** Clears the critbit.
 *
 *  \param t Critbit tree.
 *  \note Releases all allocated memory.
 */
void pcb_clear(pcb_t *t)
{
    /* FIXME: IMPLEMENT */
}


/** Checks if a string is in the critbit.
 *
 *  \param t Critbit tree.
 *  \param s String to be searched for.
 *  \return 1 if the string was found, 0 otherwise.
 */
int pcb_in(const pcb_t* t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return 0;
}


/** Finds the smallest lexicographically bigger string in the critbit.
 *
 *  \param t Critbit tree.
 *  \param s Base string.
 *  \return The smallest string in \a t that is bigger than \a s or \c NULL if
 *          there is none.
 */
const char *pcb_find_next(const pcb_t *t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return NULL;
}


/** Iterates over all the suffixes of a given string in the critbit.
 *
 *  \param t Critbit tree.
 *  \param s Base string.
 *  \param cb Callback function.
 *  \param ctx Context for the callback function.
 *  \return 1 if all the callback executions return 1, 0 otherwise.
 *  \note \a cb is executed over every string in \a t that has \a s as a
 *        prefix. The iteration is stopped if the callback returns 0.
 */
int pcb_find_suffixes(const pcb_t *t, const char *s, int (*cb)(const char *s, void *ctx), void *ctx)
{
    /* FIXME: IMPLEMENT */
    return 0;
}
