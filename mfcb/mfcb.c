/** My first critbit tree - Implementation.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#include "mfcb.h"
#include <limits.h>
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


/** Reads a given bit from a string.
 *
 *  \param s String.
 *  \param s_len Length of \a s.
 *  \param bit_pos Bit position.
 *  \return Bit value.
 */
static int _get_bit( const char *s, size_t s_len, size_t bit_pos )
{
    return bit_pos / CHAR_BIT >= s_len ? 0 : s[bit_pos >> 3] & (1 << (7 - (bit_pos & 7)));
}


/** Gets the direction in a string lookup process.
 *
 *  \param p Pointer to an internal node.
 *  \param s String that is being looked up.
 *  \param s_len Length of \a s.
 *  \return 1 means right, 0 means left.
 */
static int _get_direction(const mfcb_node_t *p, const char *s, size_t s_len)
{
    return _get_bit(s, s_len, p->critbit_pos) != 0;
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
    while (((s1[critbit_pos >> 3] ^ s2[critbit_pos >> 3]) & (1 << (7 - (critbit_pos & 7)))) == 0)
        critbit_pos++;
    return critbit_pos;
}


/** Recursively traverses a subtree, executing a callback over every external node.
 *
 *  \param r Root node.
 *  \param cb Callback to be executed over every external node.
 *  \param ctx Context for \a cb.
 *  \return 1 if the iteration was completed successfully, 0 otherwise.
 *  \note The iteration is interrupted if the callback returns 0.
 */
static int _rec_traverse(intptr_t r, int (*cb)(const char *s, void *ctx), void *ctx)
{
    /* if it's an external node, just executes the calllback */
    if (!_points_to_int_node(r))
        return cb((const char *)r, ctx);

    /* otherwise, just executes recursively over the children */
    mfcb_node_t *rn = _get_node_ptr(r);
    if (!_rec_traverse(rn->children[0], cb, ctx))
        return 0;
    if (!_rec_traverse(rn->children[1], cb, ctx))
        return 0;
    return 1;
}



/** Recursively frees this node and all its descendants.
 *
 *  \param p Tagged pointer to the node.
 */
static void _rec_clear(intptr_t p)
{
    /* if it's an internal node, we have to do the recursion before freeing */
    if (_points_to_int_node(p))
    {
        mfcb_node_t *pi = _get_node_ptr(p);
        _rec_clear(pi->children[0]);
        _rec_clear(pi->children[1]);
        free(pi);
    }
    /* otherwise, we just free the node */
    else
    {
        free((char *)p);
    }
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

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* main loop */
    intptr_t p = t->root;
    while (_points_to_int_node(p))
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s, s_len)];

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

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* search loop */
    intptr_t p = t->root;
    while (_points_to_int_node(p))
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s, s_len)];

    /* if it matches, it cannot be added */
    if (strcmp((const char *)p, s) == 0)
        return 0;

    /* if it doesn't match, we have a critical bit that differs */
    size_t critbit_pos = _get_critbit_pos((const char *)p, (const char *)s);

    /* redoes the search to see which pointer to update */
    intptr_t *pp = &t->root;
    while (_points_to_int_node(*pp) &&
           _get_node_ptr(*pp)->critbit_pos < critbit_pos)
        pp = &_get_node_ptr(*pp)->children[_get_direction(_get_node_ptr(*pp), s, s_len)];

    /* allocates a node */
    mfcb_node_t *n = malloc(sizeof(mfcb_node_t));
    n->critbit_pos = critbit_pos;
    n->children[_get_bit(s, s_len, critbit_pos) != 0] = (intptr_t)_strdup(s);
    n->children[_get_bit(s, s_len, critbit_pos) == 0] = *pp;

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
    /* if it's empty, we cannot remove anything */
    if (t->root == 0)
        return 0;

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* search loop */
    intptr_t *p = &t->root;
    intptr_t *q = NULL;
    while (_points_to_int_node(*p))
    {
        q = p;
        p = &_get_node_ptr(*p)->children[_get_direction(_get_node_ptr(*p), s, s_len)];
    }

    /* if it doesn't match, it cannot be removed */
    if (strcmp((const char *)*p, s) != 0)
        return 0;

    /* checks if the node has a sibling */
    if (q != NULL)
    {
        /* gets the sibling node */
        intptr_t r = _get_node_ptr(*q)->children[_get_node_ptr(*q)->children[0] == *p];

        /* removes the node */
        free((char *)*p);

        /* removes the parent internal node */
        free(_get_node_ptr(*q));

        /* replaces it with the sibling */
        *q = r;
    }
    else
    {
        /* no siblings, it's root */
        /* releases it, setting the pointer to 0 */
        free((char *)*p);
        *p = 0;
    }

    /* success */
    return 1;
}


/** Returns the lexicographically smallest string in \a t that is greater than \a s.
 *
 *  \param t Critbit to be searched.
 *  \param s Reference string.
 *  \return Lexicographically smallest string in \a t that is greater than \a s or \c NULL if there is none.
 */
const char *mfcb_find(const mfcb_t *t, const char *s)
{
    /* if it's empty, returns NULL */
    if (t->root == 0)
        return NULL;

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* search loop for p */
    intptr_t p = t->root;
    intptr_t q = 0;
    while (_points_to_int_node(p))
    {
        int dir = _get_direction(_get_node_ptr(p), s, s_len);
        if (dir == 0)
            q = _get_node_ptr(p)->children[1];
        p = _get_node_ptr(p)->children[dir];
    }

    /* if p is bigger, it's the answer */
    if (strcmp((const char *)p, s) > 0)
        return (const char *)p;

    /* if q is still 0, there is no answer */
    if (q == 0)
        return NULL;

    /* search loop for the minimal value in the subtree of q */
    while (_points_to_int_node(q))
        q = _get_node_ptr(q)->children[0];

    /* success */
    return (const char *)q;
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
    /* if it's empty, it "succeeded" */
    if (t->root == 0)
        return 1;

    /* gets the required critical bit position */
    size_t s_len = strlen(s);
    size_t critbit_pos = CHAR_BIT * s_len;

    /* search loop for the critical node */
    intptr_t p = t->root;
    while (_points_to_int_node(p) && _get_node_ptr(p)->critbit_pos < critbit_pos)
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s, s_len)];
    intptr_t q = p;

    /* checking the prefix existence */
    while (_points_to_int_node(p))
        p = _get_node_ptr(p)->children[_get_direction(_get_node_ptr(p), s, s_len)];
    if (memcmp((const char *)p, s, critbit_pos / 8) != 0)
        return 1;

    /* recursive traverse starting from the node */
    return _rec_traverse(q, cb, ctx);
}


/** Clears \a t.
 *
 *  \param t Critbit to be cleared.
 */
void mfcb_clear(mfcb_t *t)
{
    /* recursively clears the critbit */
    _rec_clear(t->root);

    /* sets the root to 0 */
    t->root = 0;
}
