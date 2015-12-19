/** My first critbit tree - Implementation.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#include "mfcb.h"
#include <memory.h>
#include <string.h>


/** Internal node type. */
typedef struct
{
    /** Critbit position. */
    size_t critbit_pos;

    /** Children. */
    intptr_t children[2];

} mfcb_node_t;


/** Our own strdup.
 *
 *  \param s Source string.
 *  \return Copy of \a s in dynamically allocated memory.
 */
static char *_strdup(const char *s)
{
    char *ret = malloc(strlen(s) + 1);
    strcpy(ret, s);
    return ret;
}


/** Checks if a pointer is a pointer to an internal node.
 *
 *  \param p Pointer to be checked.
 *  \return 1 if \a p points to an internal node, 0 otherwise.
 */
static int _points_to_int_node(intptr_t p)
{
    return p & 1;
}


/** Gets the node pointer from a tagged pointer.
 *
 *  \param p Tagged pointer.
 *  \return Internal node pointer.
 */
static mfcb_node_t *_get_node_ptr(intptr_t p)
{
    return (mfcb_node_t *)(p & ~(intptr_t)1);
}


/** Gets the direction in a string lookup process.
 *
 *  \param p Pointer to an internal node.
 *  \param s String that is being looked up.
 *  \return 1 means right, 0 means left.
 */
static int _get_direction(const mfcb_node_t *p, const char *s)
{
    return (s[p->critbit_pos >> 3] & (1 << (p->critbit_pos & 7))) != 0;
}


/** Gets the position of the critical bit.
 *
 *  \param s1 First string to compare.
 *  \param s2 Second string to compare.
 *  \return Position of the critical bit.
 *  \note The strings must be different.
 */
static size_t _get_critbit_pos(const char *s1, const char *s2)
{
    size_t critbit_pos = 0;
    while (((s1[critbit_pos >> 3] ^ s2[critbit_pos >> 3]) & (1 << (critbit_pos & 7))) == 0)
        critbit_pos++;
    return critbit_pos;
}


/** Checks if \a s is contained in \a t.
 *
 *  \param t Critbit tree to be checked.
 *  \param s String to be searched for.
 *  \return 1 if \a s was found, 0 otherwise.
 */
int mfcb_contains(const mfcb_t *t, const char *s)
{
    /* exits on an empty critbit tree */
    if (t->root == 0)
        return 0;

    /* main loop */
    intptr_t p = t->root;
    while (_points_to_int_node(p))
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s)];

    /* final check */
    return strcmp((const char *)p, s) == 0;
}


/** Adds \a s to \a t.
 *
 *  \param t Critbit tree where \a s is going to be inserted.
 *  \param s String to be inserted.
 *  \return 1 if \a s was inserted, 0 otherwise.
 */
int mfcb_add(mfcb_t *t, const char *s)
{
    /* if it's empty, just adds it */
    if (t->root == 0)
    {
        t->root = (intptr_t)_strdup(s);
        return 1;
    }

    /* search loop */
    intptr_t p = t->root;
    while (_points_to_int_node(p))
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s)];

    /* if it matches, it cannot be added */
    if (strcmp((const char *)p, s) == 0)
        return 0;

    /* if it doesn't match, we have a critical bit that differs */
    size_t critbit_pos = _get_critbit_pos((const char *)p, (const char *)s);

    /* redoes the search to see which pointer to update */
    intptr_t *pp = &t->root;
    while (_points_to_int_node(*pp) &&
           _get_node_ptr(*pp)->critbit_pos < critbit_pos)
        pp = &_get_node_ptr(*pp)->children[_get_direction(_get_node_ptr(*pp), s)];

    /* allocates a node */
    mfcb_node_t *n = malloc(sizeof(mfcb_node_t));
    n->critbit_pos = critbit_pos;
    n->children[(s[critbit_pos >> 3] & (1 << (critbit_pos & 7))) != 0] = (intptr_t)_strdup(s);
    n->children[(s[critbit_pos >> 3] & (1 << (critbit_pos & 7))) == 0] = *pp;

    /* puts the node where it should be */
    *pp = ((intptr_t)n | 1);

    /* success */
    return 1;
}


/** Removes \a s from \a t.
 *
 *  \param t Critbit tree from where \a s is going to be removed.
 *  \param s String to be removed.
 *  \return 1 if \a s was successfully removed, 0 otherwise.
 */
int mfcb_rem(mfcb_t *t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return 0;
}


/** Returns the lexicographically smallest string in \a t that is greater than \a s.
 *
 *  \param t Critbit to be searched.
 *  \param s Reference string.
 *  \return Lexicographically smallest string in \a t that is grater than \a s or \c NULL if there is none.
 */
const char *mfcb_find(const mfcb_t *t, const char *s)
{
    /* FIXME: IMPLEMENT */
    return NULL;
}


/** Iterates over all suffixes of \a s in \a t.
 *
 *  \param t Critbit to be searched.
 *  \param s Reference string.
 *  \param cb Callback to be executed over every suffix of \a s in \a t.
 *  \param ctx Context for \a cb.
 *  \return 1 if the iteration was completed successfully, 0 otherwise.
 *  \note The iteration is interrupted if the callback returns 0.
 */
int mfcb_find_suffixes(const mfcb_t *t, const char *s, int (*cb)(const char *s, void *ctx), void *ctx)
{
    /* FIXME: IMPLEMENT */
    return 0;
}
