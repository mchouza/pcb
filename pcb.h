/** Pooled CritBit - Interface.
 *
 *  \author Mariano M. Chouza
 *  \copyright MIT License.
 */

#ifndef PCB_H
#define PCB_H


/* Pooled CritBit type (forward declaration). */
struct pcb_t;
typedef struct pcb_t pcb_t;

/** Pooled CritBit initializer. */
#define PCB_INIT ({ 0 })


/* prototypes */
int pcb_add(pcb_t *t, const char *s);
int pcb_rem(pcb_t *t, const char *s);
void pcb_clear(pcb_t *t);
int pcb_in(const pcb_t* t, const char *s);
const char *pcb_find_next(const pcb_t *t, const char *s);
int pcb_find_suffixes(const pcb_t *t, const char *s, int (*cb)(const char *s, void *ctx), void *ctx);


#endif
