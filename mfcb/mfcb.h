/** My first critbit tree - Interface.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#ifndef MFCB_H
#define MFCB_H

#include <stdint.h>
#include <stdlib.h>


/** Critbit tree type. */
typedef struct
{
    /** Root pointer. */
    intptr_t root;

} mfcb_t;


/* prototypes */
int mfcb_contains(const mfcb_t *t, const char *s);
int mfcb_add(mfcb_t *t, const char *s);
int mfcb_rem(mfcb_t *t, const char *s);
const char *mfcb_find(const mfcb_t *t, const char *s);
int mfcb_find_suffixes(const mfcb_t *t, const char *s, int (*cb)(const char *s, void *ctx), void *ctx);


#endif
