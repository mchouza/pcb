/** Pooled CritBit - Implementation.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#include "pcb.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#ifndef PCB_INITIAL_NUM_NODES
    /** Initial number of PCB nodes. */
    #define PCB_INITIAL_NUM_NODES 1024
#endif


#ifndef PCB_BLOCK_SIZE
    /** Block size in bytes. */
    #define PCB_BLOCK_SIZE 8
#endif


/** Pooled CritBit node type. */
typedef union
{
    /** Used node. */
    struct
    {
        /** CritBit position. */
        size_t cb_pos;

        /** Children. */
        uintptr_t children[2];

    } used;

    /** Free node. */
    struct
    {
        /** Next free node. */
        size_t next_free_node;

    } free;

} pcb_node_t;


/** Pooled CritBit type. */
struct pcb_t
{
    /** Root. */
    uintptr_t root;

    /** Number of used PCB nodes. */
    size_t num_used_nodes;

    /** Number of nodes. */
    size_t num_total_nodes;

    /** First free node. */
    size_t first_free_node;

    /** Nodes. */
    pcb_node_t nodes[];

};


/** Calculates the required memory for the PCB pool.
 *
 *  \param num_node Number of nodes.
 *  \return Required memory in bytes.
 */
static size_t _calc_req_mem(size_t num_nodes)
{
    return offsetof(pcb_t, nodes) + num_nodes * sizeof(pcb_node_t);
}


/** Creates a string node.
 *
 *  \param s String contents.
 *  \param s_len Length of \a s as C string.
 *  \return Pointer to string node.
 */
static char *_create_string_node(const char *s, size_t s_len)
{
    size_t num_blocks = (s_len + 1 - 1) / PCB_BLOCK_SIZE + 1;
    char *sn = malloc(num_blocks * PCB_BLOCK_SIZE);
    memset(sn + (num_blocks - 1) * PCB_BLOCK_SIZE, 0, PCB_BLOCK_SIZE);
    memcpy(sn, s, s_len);
    return sn;
}


/** Gets a free PCB node from the pool, increasing its size if needed.
 *
 *  \param tt Pointer to a critbit tree.
 *  \return Free PCB node or \c NULL in case of error.
 */
static pcb_node_t *_get_free_pcb_node(pcb_t **tt)
{
    /* gets a common pointer to makes common tasks easier */
    pcb_t *t = *tt;

    /* checks if we don't have a free node */
    if (t->num_used_nodes == t->num_total_nodes)
    {
        /* tries to reallocate the PCB */
        pcb_t *nt = realloc(t, _calc_req_mem(t->num_total_nodes * 2));
        if (nt == NULL)
            return NULL;

        /* reallocation successful, extends the free list */
        t = *tt = nt;
        for (size_t i = t->num_total_nodes; i < t->num_total_nodes * 2 - 1; i++)
            t->nodes[i].free.next_free_node = i + 1;
        t->nodes[t->num_total_nodes * 2 - 1].free.next_free_node = SIZE_MAX;
        t->first_free_node = t->num_total_nodes;
        t->num_total_nodes *= 2;
    }

    /* gets the first free node and updates the list */
    pcb_node_t *n = &t->nodes[t->first_free_node];
    t->first_free_node = n->free.next_free_node;

    /* updates the number of used nodes */
    t->num_used_nodes++;

    /* returns the (no longer) free node */
    return n;
}


/** Releases a PCB node to the pool.
 *
 *  \param t Critbit tree.
 *  \param n PCB node to be freed.
 */
static void _release_pcb_node(pcb_t *t, pcb_node_t *n)
{
    /* sets it as first free node */
    n->free.next_free_node = t->first_free_node;
    t->first_free_node = n - t->nodes;
}


/** Reads a given bit from a string.
 *
 *  \param s String.
 *  \param s_len Length of \a s.
 *  \param bit_pos Bit position.
 *  \return Bit value.
 */
static int _get_bit(const char *s, size_t s_len, size_t bit_pos)
{
    return (bit_pos >> 3) >= s_len ? 0 : s[bit_pos >> 3] & (1 << (7 - (bit_pos & 7)));
}


/** Gets the direction in a string lookup process.
 *
 *  \param n PCB node.
 *  \param s String that is being looked up.
 *  \param s_len Length of \a s.
 *  \return 1 means right, 0 means left.
 */
static int _get_direction(const pcb_node_t *n, const char *s, size_t s_len)
{
    return _get_bit(s, s_len, n->used.cb_pos) != 0;
}


/** Checks if pointer points to a node.
 *
 *  \param p Pointer to check.
 *  \return 1 if true, 0 otherwise.
 */
static int _is_node_ptr(uintptr_t p)
{
    return p & 1;
}


/** Gets the node pointer.
 *
 *  \param t Critbit tree.
 *  \param p Base pointer.
 *  \return Node pointer.
 */
static pcb_node_t* _get_node_ptr(pcb_t *t, uintptr_t p)
{
    return &t->nodes[(size_t)(p >> 1)];
}


/** Gets the constant node pointer.
 *
 *  \param t Critbit tree.
 *  \param p Base pointer.
 *  \return Node pointer.
 */
static const pcb_node_t* _get_const_node_ptr(const pcb_t *t, uintptr_t p)
{
    return &t->nodes[(size_t)(p >> 1)];
}


/** Gets the base pointer from a node pointer.
 *
 *  \param t Critbit tree.
 *  \param n Node pointer.
 *  \return Base pointer associated with \a n.
 */
static uintptr_t _get_base_ptr(pcb_t *t, const pcb_node_t *n)
{
    return ((uintptr_t)(n - t->nodes) << 1) | 1;
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
 *  \param t Critbit tree.
 *  \param r Root node.
 *  \param cb Callback to be executed over every external node.
 *  \param ctx Context for \a cb.
 *  \return 1 if the iteration was completed successfully, 0 otherwise.
 *  \note The iteration is interrupted if the callback returns 0.
 */
static int _rec_traverse(const pcb_t *t, uintptr_t r, int (*cb)(const char *s, void *ctx), void *ctx)
{
    /* if it's an external node, just executes the calllback */
    if (!_is_node_ptr(r))
        return cb((const char *)r, ctx);

    /* otherwise, just executes recursively over the children */
    const pcb_node_t *n = _get_const_node_ptr(t, r);
    if (!_rec_traverse(t, n->used.children[0], cb, ctx))
        return 0;
    if (!_rec_traverse(t, n->used.children[1], cb, ctx))
        return 0;
    return 1;
}


/** Recursively walks until finding the string nodes to be freed.
 *
 *  \param t Critbit tree.
 *  \param p Tagged pointer to the node.
 */
static void _rec_clear(pcb_t *t, uintptr_t p)
{
    /* if it's an internal node, we have to recurse */
    if (_is_node_ptr(p))
    {
        pcb_node_t *n = _get_node_ptr(t, p);
        _rec_clear(t, n->used.children[0]);
        _rec_clear(t, n->used.children[1]);
    }
    /* otherwise, we just free the node */
    else
    {
        free((char *)p);
    }
}


/** Creates a critbit.
 *
 *  \return Newly created critbit.
 */
pcb_t *pcb_create(void)
{
    /* allocates the memory for the PCB */
    pcb_t *t = malloc(_calc_req_mem(PCB_INITIAL_NUM_NODES));
    if (t == NULL)
        return t;

    /* the root starts cleared */
    t->root = 0;

    /* initializes the numbers */
    t->num_used_nodes = 0;
    t->num_total_nodes = PCB_INITIAL_NUM_NODES;
    t->first_free_node = 0;

    /* initializes the free list */
    for (size_t i = 0; i < t->num_total_nodes - 1; i++)
        t->nodes[i].free.next_free_node = i + 1;
    t->nodes[t->num_total_nodes - 1].free.next_free_node = SIZE_MAX;

    /* returns the PCB */
    return t;
}


/** Destroys a critbit.
 *
 *  \param t Critbit tree to be destroyed.
 */
void pcb_destroy(pcb_t *t)
{
    /* first clears it */
    pcb_clear(t);

    /* then releases the memory */
    free(t);
}


/** Adds a string to the critbit.
 *
 *  \param tt Pointer to a critbit tree.
 *  \param s String to be added.
 *  \return 1 if successful, 0 otherwise.
 */
int pcb_add(pcb_t **tt, const char *s)
{
    /* gets a simple pointer to make common tasks easier */
    pcb_t *t = *tt;

    /* gets the length of the string */
    size_t s_len = strlen(s);

    /* if it's empty, just gets a string node */
    if (t->root == 0)
    {
        /* string node */
        const char *sn = _create_string_node(s, s_len);
        if (sn == NULL)
            return 0;

        /* sets as root */
        t->root = (uintptr_t)sn;
        return 1;
    }

    /* search loop */
    uintptr_t p = t->root;
    while (_is_node_ptr(p))
        p = _get_node_ptr(t, p)->used.children[_get_direction(_get_node_ptr(t, p), s, s_len)];

    /* if it matches, it cannot be added */
    if (strcmp((const char *)p, s) == 0)
        return 0;

    /* if it doesn't match, we have a critical bit that differs */
    size_t cb_pos = _get_critbit_pos((const char *)p, s);

    /* gets nodes */
    pcb_node_t *n = _get_free_pcb_node(tt);
    if (n == NULL)
        return 0;
    const char *sn = _create_string_node(s, s_len);
    if (sn == NULL)
        return 0;

    /* the pool could have changed position */
    t = *tt;

    /* redoes the search to see which pointer to update */
    uintptr_t *pp = &t->root;
    while (_is_node_ptr(*pp) &&
           _get_node_ptr(t, *pp)->used.cb_pos < cb_pos)
        pp = &_get_node_ptr(t, *pp)->used.children[_get_direction(_get_node_ptr(t, *pp), s, s_len)];

    /* loads the new PCB node */
    n->used.cb_pos = cb_pos;
    n->used.children[_get_bit(s, s_len, cb_pos) != 0] = (uintptr_t)sn;
    n->used.children[_get_bit(s, s_len, cb_pos) == 0] = *pp;

    /* connects it */
    *pp = _get_base_ptr(t, n);

    /* success */
    return 1;
}


/** Removes a string from the critbit.
 *
 *  \param t Critbit tree.
 *  \param s String to be removed.
 *  \return 1 if successful, 0 otherwise.
 */
int pcb_rem(pcb_t *t, const char *s)
{
    /* if it's empty, we cannot remove anything */
    if (t->root == 0)
        return 0;

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* search loop */
    uintptr_t *p = &t->root;
    uintptr_t *q = NULL;
    while (_is_node_ptr(*p))
    {
        q = p;
        p = &_get_node_ptr(t, *p)->used.children[_get_direction(_get_node_ptr(t, *p), s, s_len)];
    }

    /* if it doesn't match, it cannot be removed */
    if (strcmp((const char *)*p, s) != 0)
        return 0;

    /* checks if the node has a sibling */
    if (q != NULL)
    {
        /* gets the sibling node */
        uintptr_t r = _get_node_ptr(t, *q)->used.children[_get_node_ptr(t, *q)->used.children[0] == *p];

        /* removes the node */
        free((char *)*p);

        /* removes the parent internal node */
        _release_pcb_node(t, _get_node_ptr(t, *q));

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


/** Clears the critbit.
 *
 *  \param t Critbit tree.
 *  \note Releases all allocated memory.
 */
void pcb_clear(pcb_t *t)
{
    /* if it's empty, there is nothing to clear */
    if (t->root == 0)
        return;

    /* recursively clears all string nodes */
    _rec_clear(t, t->root);

    /* no nodes are used */
    t->num_used_nodes = 0;

    /* initializes the free list */
    for (size_t i = 0; i < t->num_total_nodes - 1; i++)
        t->nodes[i].free.next_free_node = i + 1;
    t->nodes[t->num_total_nodes - 1].free.next_free_node = SIZE_MAX;

    /* sets the first free node */
    t->first_free_node = 0;
}


/** Checks if a string is in the critbit.
 *
 *  \param t Critbit tree.
 *  \param s String to be searched for.
 *  \return 1 if the string was found, 0 otherwise.
 */
int pcb_in(const pcb_t* t, const char *s)
{
    /* exits on an empty critbit tree */
    if (t->root == 0)
        return 0;

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* main loop */
    uintptr_t p = t->root;
    while (_is_node_ptr(p))
        p = _get_const_node_ptr(t, p)->used.children[_get_direction(_get_const_node_ptr(t, p), s, s_len)];

    /* final check */
    return strcmp((const char *)p, s) == 0;
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
    /* if it's empty, returns NULL */
    if (t->root == 0)
        return NULL;

    /* gets the length of s */
    size_t s_len = strlen(s);

    /* search loop for p */
    intptr_t p = t->root;
    intptr_t q = 0;
    while (_is_node_ptr(p))
    {
        int dir = _get_direction(_get_const_node_ptr(t, p), s, s_len);
        if (dir == 0)
            q = _get_const_node_ptr(t, p)->used.children[1];
        p = _get_const_node_ptr(t, p)->used.children[dir];
    }

    /* if p is bigger, it's the answer */
    if (strcmp((const char *)p, s) > 0)
        return (const char *)p;

    /* if q is still 0, there is no answer */
    if (q == 0)
        return NULL;

    /* search loop for the minimal value in the subtree of q */
    while (_is_node_ptr(q))
        q = _get_const_node_ptr(t, q)->used.children[0];

    /* success */
    return (const char *)q;
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
    /* if it's empty, it "succeeded" */
    if (t->root == 0)
        return 1;

    /* gets the required critical bit position */
    size_t s_len = strlen(s);
    size_t cb_pos = s_len << 3;

    /* search loop for the critical node */
    uintptr_t p = t->root;
    while (_is_node_ptr(p) && _get_const_node_ptr(t, p)->used.cb_pos < cb_pos)
        p = _get_const_node_ptr(t, p)->used.children[_get_direction(_get_const_node_ptr(t, p), s, s_len)];
    uintptr_t q = p;

    /* checking the prefix existence */
    while (_is_node_ptr(p))
        p = _get_const_node_ptr(t, p)->used.children[_get_direction(_get_const_node_ptr(t, p), s, s_len)];
    if (memcmp((const char *)p, s, cb_pos >> 3) != 0)
        return 1;

    /* recursive traverse starting from the node */
    return _rec_traverse(t, q, cb, ctx);
}
