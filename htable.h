#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>

typedef struct htablerec *htable;

typedef enum hashing_e { LINEAR_P, DOUBLE_H} hashing_t;

extern htable htable_new(hashing_t method);
extern int htable_insert(htable h, char *item);
extern int htable_search(htable h, char *item);
extern void htable_print(htable h, FILE *stream);
extern void htable_free(htable h);
extern void htable_print_entire_table(htable h);
extern void htable_print_stats(htable h, FILE *stream, int num_stats);

#endif
