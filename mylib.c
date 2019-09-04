#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"

void *emalloc(size_t s){
    void *result = malloc(s);
    if(result == NULL){
        fprintf(stderr, "Memory alloc failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

void *erealloc(void *obj, size_t s){
    void *result = realloc(obj, s);
    if(result == NULL){
        fprintf(stderr, "Memory alloc failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

