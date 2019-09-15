#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"
#include <time.h>
     
#define DEFAULT_TABLE_SIZE 113   

static void help(FILE *stream) {
  fprintf(stream,"Usage: ./sample-asgn [OPTION]... <STDIN>\n\n\
Perform various operations using a hash table.  By default, words are\n\
read from stdin and added to the hash table, before being printed out\n\
alongside their frequencies to stdout.\n\n");
  fprintf(stream," -c FILENAME  Check spelling of words in FILENAME using \
words\n              from stdin as dictionary.  Print unknown words to\n\
              stdout, timing info & count to stderr (ignore -p)\n\
 -d           Use double hashing (linear probing is the default)\n"
           );
  fprintf(stream," -e           Display entire contents of hash table on \
stderr\n -p           Print stats info instead of frequencies & words\n\
 -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is used)\n\
 -t TABLESIZE Use the first prime >= TABLESIZE as htable size\n\n\
 -h           Display this message\n\n");  
}

static int is_prime(int candidate) {
    int n;
    for (n=2; n < candidate;n++) {
        if ((candidate %  n)==0) return 0;
    }
    return 1;
}

static int get_next_prime(int number) {
    if (number == 1 || number == 0){
        return 2;
    }
    while (!is_prime(number)) {
        number++;
    }

    return number;
}

static void print_info(int freq, char *word) {
  printf("%-4d %s\n", freq, word);
}

static void print_textfile_info(double fill_time, double search_time,
				int unknown_words) {
  printf("Fill time     : %2.6f\n",fill_time);
  printf("Search time   : %2.6f\n",search_time);
  printf("Unknown words = %d\n", unknown_words);
}

/**
 * This function deals with all the command line arguments that follow
 * the program when it is invoked. Just about all the parameters are 
 * references to variables declared in the main section of this program.
 * Because everything is passed by reference there is no need for a return
 * parameter.
 * 
 */

void readflags(int *p_option, int *e_option, int *c_option, int *tableSize,
	       hashing_t* hashtype, int argc, char *argv[],
	       char *text_filename, int *snapshots) {
  
    const char *optstring = "c:deps:t:h";
    char option;
    int string_size_option;
  
    while ((option = getopt(argc, argv, optstring)) != EOF) {
        switch (option) {
            case 'c':
                if (optarg!=NULL) {
                    strcpy(text_filename, optarg);
                }
                *c_option=1; 
                break;
            case 'd':
                /* set to double hashing. Single hashing is the default.  */
                *hashtype = DOUBLE_H;
                break;
            case 'e':
                /* do something else */
                *e_option =1;
                break;
            case 'p':
                /* check that the c option is not in the argument string. If
                   it is, skip this option. */
                if (*c_option==0) {
                    *p_option=1;
                }
                break;
            case 's':
                /* do something else */
                if (*p_option) {
                    int value;
                    value = atoi(optarg);
                    if (value>0) {
                        *snapshots=value;
                    }
                }
                break;
            case 't':
                /* do something else */
                if (optarg!=NULL) {
                    string_size_option=atoi(optarg);
                    *tableSize=get_next_prime(string_size_option);
                }
                break;
            case 'h':
                /* call for help options */
                help(stderr);
                exit(EXIT_SUCCESS);
                break;
            default:
                help(stderr);
                exit(EXIT_FAILURE);
                break;
                /* if an unknown option is given */
        }
    }

}

void process_txtfile(htable h, char *text_filename, double fill_time ) {

  int start;
  int end;
  double search_time;
  int unknown_words=0;
  char word[256];
   

  
  FILE *file_pointer;
  
  file_pointer = fopen(text_filename, "r");
  if (file_pointer == NULL)
    {
      printf("Cannot open file '%s' using mode r.\n", text_filename);
      exit(EXIT_FAILURE);
    }
  
 
  
  start = clock();
  while (getword(word, sizeof word, file_pointer) != EOF)
    {
      if (!htable_search(h,word)) {
	printf("%s\n",word);
	unknown_words++;
      } 
    }
  end = clock();
  search_time = ((double)(end-start))/CLOCKS_PER_SEC;
  print_textfile_info(fill_time, search_time, unknown_words);

  fclose(file_pointer);
}


int main(int argc, char *argv[]) {


    htable h;
    char word[256];
    hashing_t hashtype = LINEAR_P;    
    char text_filename[256];
    int p_option=0;
    int e_option=0;
    int c_option=0;
    clock_t start, end;
    double fill_time;
    int snapshots = 10;
    int tableSize = DEFAULT_TABLE_SIZE;
 
   
    readflags(&p_option, &e_option, &c_option, &tableSize, &hashtype,
	      argc, argv,text_filename, &snapshots );

    /* The following instruction creates a new hashing table. The 
       parameters have default values but these may changed depending on
       the program arguments. */
    
    h=htable_new(tableSize, hashtype);

    /* This section reads in words from the dictionary file that is 
       directed to this program from stdin. The time taken to read in
       the dictionary is determined using two clock functions and
       the value is put into the variable fill_time. */
    
    start = clock();
    while ((getword(word, sizeof word, stdin) != EOF) &&
	   (htable_insert(h,word)!=-1)) {
    }
    end = clock();
    fill_time = ((double) (end - start))/CLOCKS_PER_SEC;

    
    if (e_option) {
      htable_print_entire_table(h, stderr);
    }
  
    if (c_option==0) {
      if (p_option==0) {
	htable_print(h, print_info);
      } else {
	htable_print_stats(h, stdout, snapshots);
      }
    } else {
      process_txtfile(h, text_filename,fill_time);
    }
    htable_free(h);
 
 
    return (EXIT_SUCCESS);
}
