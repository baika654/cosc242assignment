#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include "mylib.h"
#include <string.h>

#define MAX 113





/*
  htable struct, contains variables for:
  The number of keys currently in the table, the capcity of the table,
  the contents of the table (an array of strings), the frequencies of each key,
  a record of how many collisions occur per insertion, and an enum
  type which dictates the type of hashing method.
 */
struct htablerec{
    int numKeys;
    int capacity;
    char **items;
    int *frequencies;
    int *stats;
    hashing_t method;
};


/**
 * Prints out a line of data from the hash table to reflect the state
 * the table was in when it was a certain percentage full.
 * Note: If the hashtable is less full than percent_full then no data
 * will be printed.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 * @param percent_full - the point at which to show the data from.
 */
   static void print_stats_line(htable h, FILE *stream, int percent_full) {
   int current_entries = h->capacity * percent_full / 100;
   double average_collisions = 0.0;
   int at_home = 0;
   int max_collisions = 0;
   int i = 0;

   if (current_entries > 0 && current_entries <= h->numKeys) {
      for (i = 0; i < current_entries; i++) {
         if (h->stats[i] == 0) {
            at_home++;
         } 
         if (h->stats[i] > max_collisions) {
            max_collisions = h->stats[i];
         }
         average_collisions += h->stats[i];
      }
    
      fprintf(stream, "%4d %10d %11.1f %10.2f %11d\n", percent_full, 
              current_entries, at_home * 100.0 / current_entries,
              average_collisions / current_entries, max_collisions);
   }
}


/**
 * Prints out a table showing what the following attributes were like
 * at regular intervals (as determined by num_stats) while the
 * hashtable was being built.
 *
 * @li Percent At Home - how many keys were placed without a collision
 * occurring.
 * @li Average Collisions - how many collisions have occurred on
 *  average while placing all of the keys so far.
 * @li Maximum Collisions - the most collisions that have occurred
 * while placing a key.
 *
 * @param h the hashtable to print statistics summary from.
 * @param stream the stream to send output to.
 * @param num_stats the maximum number of statistical snapshots to print.
 */
void htable_print_stats(htable h, FILE *stream, int num_stats) {
   int i;

   fprintf(stream, "\n%s\n\n", 
           h->method == LINEAR_P ? "Linear Probing" : "Double Hashing"); 
   fprintf(stream, "Percent   Current    Percent    Average      Maximum\n");
   fprintf(stream, " Full     Entries    At Home   Collisions   Collisions\n");
   fprintf(stream, "------------------------------------------------------\n");
   for (i = 1; i <= num_stats; i++) {
      print_stats_line(h, stream, 100 * i / num_stats);
   }
   fprintf(stream, "------------------------------------------------------\n\n");
}





/*
  This method creates and returns a new htable struct.
  It sets all the variables of the new htable struct to their default values
  (NULL or 0),  and allocates memory to all the arrays and the object itself.

  The hashing_t method paramater is to determine what hashing method to use
  for the new table.
 */
htable htable_new(int size, hashing_t method){
    int i; 
    htable result = emalloc(sizeof result);
    result->numKeys = 0;
    result->capacity = size;
    result->method = method;
    result->items = emalloc(size * (sizeof result->items[0]));
    result->frequencies = emalloc(size * sizeof result->frequencies[0]);
    result->stats = emalloc(size * sizeof result->stats[0]);
    for(i = 0; i < size; i++){
        result->items[i] = NULL;
        result->frequencies[i] = 0;
        result->stats[i] = 0;
    }
    return result;
}


/*
  This method first frees all the keys in the keys array of the htable.
  Then it frees all the arrays in the htable, and finally frees the object
  itself.

  //NOTE: Currently if free is called on h->items, it results in an error
  message: double free or corruption (out)
           Aborted (core dumped)
  Should probably address this as it may cause a memory leak.
  
  Currently the free call on h->items is commented out.
 */
void htable_free(htable h){
    int i;
    for(i = 0; i < h->capacity; i++){
        free(h->items[i]);
    }
    free(h->frequencies);
    free(h->stats);
    /*free(h->items);*/
    free(h);
}

/*
  This method prints all the keys of the htable to a given output stream.
 */
void htable_print(htable h, FILE *stream){
    int i;
    for(i = 0; i < h->capacity; i++){
        if(h->items[i] != NULL){
            fprintf(stream, "%d    %s\n", h->frequencies[i], h->items[i]);
        }
    }
}

/*
  This static method converts a string to an unsigned int.
  This is used for inserting and searching indeces in the htable.
 */
static unsigned int wordToInt(char *word){
    unsigned int result = 0;
    while(*word != '\0'){
        result = (*word++ + 31 * result);
    }
    return result;
}

/*
  This static method allocates memory for a string and inserts it
  at a given index in an htable.
  This method makes the insertion code A LOT cleaner.
 */
static void htableInsertAt(htable h, char *word, int key){
    h->items[key] = emalloc(sizeof h->items[key][0] * strlen(word)+1);
    strcpy(h->items[key], word);
    h->frequencies[key]++;
}


/*
  This method uses linear hashing to insert a key into an htable.
  It iterates based on the linear probing algorithm until it finds either
  a free cell, a matching string, or has iterated through the entire table.

  If it finds a free cell, it inserts the given string, increases the numKeys
  variable of the htable, and sets the stats[numKeys] to the number of
  collisions that occured during insertion.

  If it finds a matching string, it increases the key's matching frequency.

  If it iterates through through the whole table and doesn't find either
  stopping case, it returns -1 (insertion fail).
*/
static int linearInsert(htable h, char *word){
 
    int collisions = 0;
    int key = wordToInt(word) % h->capacity;

  
    if(h->items[key] == NULL){
        htableInsertAt(h, word, key);
        h->stats[h->numKeys++] = collisions;
        return key;
    }else if(strcmp(h->items[key], word) == 0){
        h->frequencies[key]++;
        return key;
    }
    collisions = 1;
    while(collisions < h->capacity + 1){
      key = ((key + 1) % h->capacity);
        if(h->items[key] == NULL){
            htableInsertAt(h, word, key);
            h->stats[h->numKeys++] = collisions;
            return key;
        }else if(strcmp(h->items[key], word) == 0){
            h->frequencies[key]++;
            return key;
        }else{
            collisions++;
        }
    }
    
    return -1;
}

/*
  This method calculates the next step for double hashing
*/
static unsigned int htable_step(htable h, unsigned int i_key){
    return 1 + (i_key % (h->capacity - 1));
}

/*
  This method uses double hashing to insert a key into a hash table.
  It iterates based on the double hashing algorithm until it finds either
  a free cell, a matching string, or has iterated through the entire table.

  If it finds a free cell, it inserts the given string, increases the numKeys
  variable of the htable, and sets the stats[numKeys] to the number of
  collisions that occured during insertion.

  If it finds a matching string, it increases the key's matching frequency.

  If it iterates through through the whole table and doesn't find either
  stopping case, it returns -1 (insertion fail).
 */
static int doubleInsert(htable h, char *word){
    int collisions = 0;
    int key = wordToInt(word) % h->capacity;
    if(h->items[key] == NULL){
        htableInsertAt(h, word, key);
        h->stats[h->numKeys++] = collisions;
        return key;
    }else if(strcmp(h->items[key], word) == 0){
        h->frequencies[key]++;
        return key;
    }
    collisions = 1;
    while(collisions < h->capacity + 1){
        key = htable_step(h, key);
        if(h->items[key] == NULL){
            htableInsertAt(h, word, key);
            h->stats[h->numKeys++] = collisions;
            return key;
        }else if(strcmp(h->items[key], word) == 0){
            h->frequencies[key]++;
            return key;
        }else{
            collisions++;
        }
    }
    
    return -1;
}


/*
  This method uses double hashing to search for a key into a hash table.
  It iterates based on the double hashing algorithm until it finds either
  a free cell, a matching string, or has iterated through the entire table.

  If it finds a free cell, it returns a 0, indicating that the
  word is not in the table.

  If it finds a matching string, it increases the key's matching frequency.

  If it iterates through through the whole table and doesn't find either
  stopping case, it returns 0 (word is not in the table).
*/
static int doubleSearch(htable h, char *word){
    int key = wordToInt(word) % h->capacity;
    int collisions = 0;
    while(strcmp(h->items[key], word) != 0 && collisions <= h->capacity){
        collisions++;
        key = htable_step(h, key) % h->capacity;
        if(h->items[key] == NULL){
            return 0;
        }
    }
    if(collisions >= h->capacity){
        return 0;
    }else{
        return h->frequencies[key];
    }
}

/*
  This method uses double hashing to search for a key into a hash table.
  It iterates based on the linear probing algorithm until it finds either
  a free cell, a matching string, or has iterated through the entire table.

  If it finds a free cell, it returns a 0, indicating that the
  word is not in the table.

  If it finds a matching string, it increases the key's matching frequency.

  If it iterates through through the whole table and doesn't find either
  stopping case, it returns 0 (word is not in the table).
*/
static int linearSearch(htable h, char *key){
    int collisions = 0;
    int pos = wordToInt(key) % h->capacity;

    /* printf("Searching for  %s\n", key); */
    while(h->items[pos] != NULL && (strcmp(key, h->items[pos]) != 0) &&  pos < h->capacity){
      pos = ((pos + 1) % h->capacity);
        collisions++;
       
    }
    if(collisions == h->capacity){
        return 0;
    }else{
        return h->frequencies[pos];
    }
    return 0;
}


/*
  This method uses either the linearInsert method or the doubleInsert method
  to insert a word into a given hash table h, based on that hash table's
  method variable (LINEAR_P for linear probing, DOUBLE_H for double hashing).
 */
int htable_insert(htable h, char *word){
    if(h->method == LINEAR_P){
        return linearInsert(h, word);
    }else{
        return doubleInsert(h, word);
    }
}

/*
  This method uses either the linearInsert method or the doubleInsert method
  to search for a word into a given hash table h, based on that hash table's
  method variable (LINEAR_P for linear probing, DOUBLE_H for double hashing).
*/
int htable_search(htable h, char *word){
    if(h->method == LINEAR_P){
        return linearSearch(h, word);
    }else{
        return doubleSearch(h, word);
    }
}

/*
 *This function prints the entire contents of the hash table. Each element of 
 *the has table is printed one line at a time. 
 *
 * @param h the hashtable to print entire contents from.
 *
 */

void htable_print_entire_table(htable h) {
  int i;
  printf("  Pos  Freq  Stats  Word\n");
  printf("----------------------------------------\n");
  for (i=0; i<h->capacity; i++) {
    
    printf("%5d %5d %5d", i, h->frequencies[i], h->stats[i]);
    if (h->items[i]==NULL) {
      printf("\n"); 
    } else {
      printf("   %s\n",h->items[i]);
    }  
  }

}
