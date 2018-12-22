#include <stdio.h>

#include "SCC.H"


// Flex reads the 'yyin' file handle for input.  'yyin' will be defined in
// the generated scc-lexer.cpp file, so we'll need to extern the variable.
extern FILE *yyin;


int main( int argc, char **argv )
{
  // The usage for this program is pretty simple.  If no arguments are given
  // to the program, then it will expect input on stdin (this is the default
  // value of 'yyin').  If there was an argument specified, then this will
  // specify the source file name to read from.
  if ( argc == 2 ) {
    // Open the given file name for reading.
    yyin = fopen( argv[1], "r" );

    // If the file could not be opened, then inform the user and terminate the
    // program.
    if ( yyin == NULL ) {
      perror( argv[1] );
      return 1;
    }
  }

  // Call into Bison to tell it to parse the input file.
  yyparse();

  return 0;
}
