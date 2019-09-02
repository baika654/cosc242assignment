#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[]) {


  const char *optstring = "c:deps:t:h";
  char option;
  while ((option = getopt(argc, argv, optstring)) != EOF) {
    switch (option) {
    case 'c':
      /* do something */
    case 'd':
      /* the argument after the -b is available
	 in the global variable ’optarg’ */
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

  return (EXIT_SUCCESS);
}

