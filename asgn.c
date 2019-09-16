#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"
#include <time.h>
     
#define DEFAULT_TABLE_SIZE 113   

/**
 * This static function prints out the help information when either -h
 * or some other incorrect command line arguement is used.
 *
 * @param stream - a stream to print the data to.
 * 
 */

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

/**
 * This static function tests whether a number is a prime or not.
 *
 * @param candidate - the number that is checked.
 *
 * @return a boolean (as an int) showing if the number is a prime.
 */

static int is_prime(int candidate) {
  int n;
  for (n=2; n < candidate;n++) {
    if ((candidate %  n)==0) return 0;
  }
  return 1;
}

/**
 * This static function creates a prime number greater than 'number'.
 *
 * @param number - any integer.
 *
 * @return  a prime number.
 */

static int get_next_prime(int number) {
  if (number == 1 || number == 0){
    return 2;
  }
  while (!is_prime(number)) {
    number++;
  }

  return number;
}

/**
 * This static function prints to stdout an integer and string
 * using formatting. This function is passed as a parameter
 * in another function.
 *
 * @param freq - the integer that needs to be printed.
 *
 * @param word - the string that needs to be printed.
 *
 */

static void print_info(int freq, char *word) {
  printf("%-4d %s\n", freq, word);
}

/**
 * When finished checking text document for unknown words, this 
 * function prints timing information and unknown word count 
 * to stderr.
 * @param fill_time - the time taken to fill the hasttable. 
 * @param search_time - the time taken to process the document
 *                      and compare with keys in the hashtable.
 * @param unknown_words - words that appear in the document, but
 *                        not the hash table.
 *
 *******************************************************************/
 
static void print_textfile_info(double fill_time, double search_time,
				int unknown_words) {
  fprintf(stderr,"Fill time     : %2.6f\n",fill_time);
  fprintf(stderr,"Search time   : %2.6f\n",search_time);
  fprintf(stderr,"Unknown words = %d\n", unknown_words);
}

/**
 * This function deals with all the command line arguments that follow
 * the program when it is invoked. Just about all the parameters are 
 * references to variables declared in the main section of this program.
 * Because everything is passed by reference there is no need for a return
 * parameter. This is probably not the best way of implementing a function but
 * it is convenient. It breaks up main() into smaller components where this
 * component deals entirely with the command line arguments.
 *
 * @param *p_option - a reference to p_option defined in main. Used as a flag.
 * @param *e_option - a reference to e_option defined in main. Used as a flag.
 * @param *c_option - a reference to c_option defined in main. Used as a flag.
 * @param *tableSize - a reference to tableSize defined in main. This 
 *                     variable defines the hashtable size.
 * @param *hashtype - this variable indicates if linear probing or double
 *                    hashing is used in the hashtable.
 * @param argc - this is the count of command line arguments.
 * @param argv - this is a string array of command line arguments.
 * @param text_filename - the character string containing the name
 *                        of the document file, which is set in this function.
 * @param snapshots - the maximum number of statistical snapshots to print.
 *                    This value is set in this function.
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
      /* If c is passed as a command line argument, read in the 
	 name of the document text file and store in the char 
	 string text_filename. */
      if (optarg!=NULL) {
	strcpy(text_filename, optarg);
      }
      *c_option=1; 
      break;
    case 'd':
      /* Set to double hashing. Linear probing is the default.  */
      *hashtype = DOUBLE_H;
      break;
    case 'e':
      /* If e is set to one, the entire contents of the hashtable
       * are printed. */
      *e_option =1;
      break;
    case 'p':
      /* Check that the c option is not in the argument string. If
       * it is, skip this option. This flag when set will print 
       * stats info. */
      if (*c_option==0) {
	*p_option=1;
      }
      break;
    case 's':
      /* This option if found in the command line arguments will
       * read in a number that will eventually be used to display 
       * up to the given number of stats snapshots. */
      if (*p_option) {
	int value;
	value = atoi(optarg);
	if (value>0) {
	  *snapshots=value;
	}
      }
      break;
    case 't':
      /* If this option is set the number after t is used to
	 create a prime that is bigger than the input number
	 and this prime will determine the hashtable size. */
      if (optarg!=NULL) {
	string_size_option=atoi(optarg);
	*tableSize=get_next_prime(string_size_option);
      }
      break;
    case 'h':
      /* Call for help options. */
      help(stderr);
      exit(EXIT_SUCCESS);
      break;
    default:
      /* If an unknown option is given, print the help file and
	 then exit. */
      help(stderr);
      exit(EXIT_FAILURE);
      break;
            
    }
  }

}

/** 
 *
 * This function is responsible for opening the document text file
 * and checking it against the dictionary file stored in the hashtable.
 * If any word read from the document file is not contained in the
 * dictionary stored in the hashtable then it will get printed to
 * stdout.
 * 
 * @param h - the hash table.
 * @param text_filename - the character string containing the name
 *                         of the document file to be read in by
 *                         this function.
 * @param fill_time - the time taken to fill in the hashtable.
 *
 */ 

void process_txtfile(htable h, char *text_filename, double fill_time ) {

  /* These two variables are used to determine the time it takes
   * for this program to check the document text file against the
   * hashtable. */
  int start, end;
  /* A double that stores the time it takes to search the document
   * text file. */
  double search_time;
  /* A count of words found in the document text file but not in the 
   * hashtable. */
  int unknown_words=0;
  /* This string is used to read in words in from the document text file
   * from stdin. */
  char word[256];
  /* This variable points to an object associated with the document text
   * file. */ 
  FILE *file_pointer;
  
  file_pointer = fopen(text_filename, "r");
  if (file_pointer == NULL)
    {
      printf("Cannot open file '%s' using mode r.\n", text_filename);
      htable_free(h);
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

/**
 * 
 *This is the function that is first invoked when the program runs
 * 
 * @param argc - this is the count of command line arguments
 * @param argv - this is a string array of command line arguments
 *
 *********************************/

int main(int argc, char *argv[]) {

  /* The hashtable variable used in this program. */
  htable h;
  /* This string is used to read in words in from a dictionary file
   * from stdin. */
  char word[256];
  /* The default method for hashing is set to linear probing. */
  hashing_t hashtype = LINEAR_P;    
  /* A string to store the name of the text file to check if it is
   * specified in the command line arguments. */
  char text_filename[256];
  /* The following three integers are flags that are set depending
   * the command line arguments used. The flags determine how this
   * program will process the dictionary and document files. */
  int p_option=0;
  int e_option=0;
  int c_option=0;
  /* These two variables are used to determine the time it takes
   * for this program to fill out a hashtable with words from a
   *  dictionary file. */
  clock_t start, end;
  /* A double that stores the time it takes to fill a hashtable. */
  double fill_time;
  /* This integer is the default stats snapshot value to be used.
   * The -s argument in the command line will alter this value. */
  int snapshots = 10;
  /* This variable determines the size of the hashtable. It will 
   * always be a prime number. The -t argument in the command line
   * will alter this value. */
  int tableSize = DEFAULT_TABLE_SIZE;

  /* The following function reads in the command line arguments
     and sets the option flags based on the arguments use. */
   
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

  /* If -e is specified in the command line arguments the 
   * htable_print_entire_table function will display entire contents 
   * of hash table on stderr using the format string. */
    
  if (e_option) {
    htable_print_entire_table(h, stderr);
  }

  /* This next sections is the logic that deals with the option flags 
   * mentioned in an earlier comment. Depending of the combination of
   * command line arguments used, various stats about the hash table
   * and the state of the program will be printed. If the -c argument
   * is used a document file will be read in and processed. When this 
   * option is given then the -p option is ignored. */

    
  if (c_option==0) {
    if (p_option==0) {
      htable_print(h, print_info);
    } else {
      htable_print_stats(h, stdout, snapshots);
    }
  } else {
    process_txtfile(h, text_filename,fill_time);
  }

  /* At this point of the programming all processing has occurred
   * so the memory associated with the hashtable is freed before the
   * program terminates. */
    
  htable_free(h);
 
 
  return (EXIT_SUCCESS);
}
