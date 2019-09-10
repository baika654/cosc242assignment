#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"
#include <time.h>
     
#define DEFAULT_TABLE_SIZE 113   

void static help() {
  printf("Usage: ./sample-asgn [OPTION]... <STDIN>\n\n\
Perform various operations using a hash table.  By default, words are\n\
read from stdin and added to the hash table, before being printed out\n\
alongside their frequencies to stdout.\n\n");
  printf(" -c FILENAME  Check spelling of words in FILENAME using words\n\
              from stdin as dictionary.  Print unknown words to\n\
              stdout, timing info & count to stderr (ignore -p)\n\
 -d           Use double hashing (linear probing is the default)\n\
 -e           Display entire contents of hash table on stderr\n\
 -p           Print stats info instead of frequencies & words\n\
 -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is used)\n\
 -t TABLESIZE Use the first prime >= TABLESIZE as htable size\n \n\
 -h           Display this message\n \n");  
}

int is_prime (candidate) {
  int n;
  for (n=2; n < candidate;n++) {
    if ((candidate %  n)==0) return 0;
  }
  return 1;
}

int get_next_prime(int number) {
  while (!is_prime(number)) {
     number++;
  }

return number;
}

int main(int argc, char *argv[]) {


  
  /* Setup variables and any defaults that are needed for this program */ 
  htable h;
  char word[256];
  enum hashing_e hashtype;
  hashtype = LINEAR_P;
  char text_filename[256];
  FILE *file_pointer;
  int string_size_option;
  int p_option = 0;
  int e_option = 0;
  int c_option = 0;
  clock_t start, end;
  double fill_time, search_time;
  int unknown_words=0;
  int snapshots = 10;
  int tableSize = DEFAULT_TABLE_SIZE;
 
  /* Deal with all the flags that are added at the end of the app when
     started from the command line interface   */
  

  
  
  const char *optstring = "c:deps:t:h";
  char option;
  
  while ((option = getopt(argc, argv, optstring)) != EOF) {
    switch (option) {
    case 'c':
      if (optarg!=NULL) {
	strcpy(text_filename, optarg);
	printf("%s\n",text_filename);
      }
      c_option=1;
      break;
    case 'd':
      /* set to double hashing. Single hashing is the default.  */
      hashtype = DOUBLE_H;
      break;
    case 'e':
      /* do something else */
      e_option =1;
      break;
    case 'p':
      /* check that the c option is not in the argument string. If
      it is, skip this option. */
      if (c_option==0) {
	p_option=1;
      }
      break;
    case 's':
      /* do something else */
      if (p_option) {
	int value;
	value = atoi(optarg);
	if (value>0) {
	  snapshots=value;
	}
      }
      break;
    case 't':
      /* do something else */
      if (optarg!=NULL) {
	string_size_option=atoi(optarg);
	tableSize=get_next_prime(string_size_option);
	printf("%d\n",tableSize);
      }
      break;
    case 'h':
      /* call for help options */
      help();
      return(EXIT_SUCCESS);
      break;
    default:
      help();
      return(EXIT_SUCCESS);
      break;
      /* if an unknown option is given */
    }
  }

  /* Read in the dictionary that is piped to this program */
  start = clock();
  h=htable_new(tableSize, hashtype);
  while (getword(word, sizeof word, stdin) != EOF) {
    /*printf("%s\n",word);*/
    htable_insert(h, word);
  }
  end = clock();
  fill_time = ((double) (end - start))/CLOCKS_PER_SEC;

  if (e_option) {
    htable_print_entire_table(h);
  }
  
  if (c_option==1) {
    file_pointer = fopen(text_filename, "r");
    if (file_pointer == NULL)
      {
	printf("Cannot open file.\n");
	exit(EXIT_FAILURE);
      }
  
 
  
    start = clock();
    while (getword(word, sizeof word, file_pointer) != EOF)
      {
	if (!htable_search(h,word)) {
	  printf("%s\n", word);
	  unknown_words++;
	} 
      }

    end = clock();
    search_time = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Fill time     : %2.6f\n",fill_time);
    printf("Search time   : %2.6f\n",search_time);
    printf("Unknown words = %d\n", unknown_words);
    fclose(file_pointer);
    
  } else if (p_option==1) {
    htable_print_stats(h, stdout , snapshots);
  }

  /*printf("C-option is %d and P-option is %d\n",c_option, p_option);*/
  if ((c_option==0) && (p_option==0)) {
    htable_print(h,stdout);
  }
 
 
  return (EXIT_SUCCESS);
}

