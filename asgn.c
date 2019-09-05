#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "htable.h"
#include "mylib.h"


int main(int argc, char *argv[]) {


  /* Setup variables and any defaults that are needed for this program */ 
  htable h;
  char word[256];
  enum hashing_e hashtype;
  hashtype = LINEAR_P;

 
  /* Deal with all the flags that are added at the end of the app when
     started from the command line interface   */
  
  
  const char *optstring = "c:deps:t:h";
  char option;
  while ((option = getopt(argc, argv, optstring)) != EOF) {
    switch (option) {
    case 'c':
      /* do something */
    case 'd':
      /* set to double hashing. Single hashing is the default.  */
      hashtype = DOUBLE_H;
      break;
    case 'e':
      /* do something else */
    case 'p':
      /* do something else */
    case 's':
      /* do something else */
    case 't':
      /* do something else */
    case 'h':
      /* call for help options */
      printf("Help\n");
      break;
    default:
      break;
      /* if an unknown option is given */
    }
  }

  /* Read in the dictionary that is piped to this program */
  
  h=htable_new(hashtype);
  while (getword(word, sizeof word, stdin) != EOF) {
    printf("%s\n",word);
    htable_insert(h, word);
  }

  
  return (EXIT_SUCCESS);
}

