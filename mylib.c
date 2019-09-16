#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


/** 
 * This function allocates memory on the heap.
 * @param s - the amount of memory in bytes needed.
 * @return - a pointer to a chunk of memory.
 */

void *emalloc(size_t s){
    void *result = malloc(s);
    if(result == NULL){
        fprintf(stderr, "Memory alloc failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

/** 
 * This function resizes a chunk of memory pointed by a variable.
 * @param obj - a pointer to an allocated chunk of memory.
 * @param s - the new amount of memory requested in bytes.
 * @return - a new chunk of memory with data from pointer obj copied in.
 */

void *erealloc(void *obj, size_t s){
    void *result = realloc(obj, s);
    if(result == NULL){
        fprintf(stderr, "Memory alloc failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

/** This function extracts a word from a stdin filestream.
 * @param s - a pointer to a character string. The word is copied into this
 *           string.
 * @param limit - the size of the character string.
 * @param stream - a pointer to the filestream that provides the the words.
 * @return - the word size in bytes or EOF.
 */


int getword(char *s, int limit, FILE *stream) {
    int c;
    char *w = s;
    assert(limit > 0 && s != NULL && stream != NULL);

    /* skip to the start of the word */
    while (!isalnum(c = getc(stream)) && EOF != c);

    if (EOF == c) {
        return EOF;
    } else if (--limit > 0) { /* reduce limit by 1 to allow for the \0 */
        *w++ = tolower(c);
    }
    while (--limit > 0) {
        if (isalnum(c = getc(stream))) {
            *w++ = tolower(c);
        } else if ('\'' == c) {
            limit++;
        } else {
            break;
        }
    }
    *w = '\0';
    return w - s;
}

