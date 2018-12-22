%{
// This file contains all the rules that GNU Flex needs to identify tokens in
// the script source code.  GNU Flex is a lexical analyzer (or lexer) that
// finds tokens in the input stream.  Flex is a tool that is commonly used in
// conjunction with GNU Bison and more information about this tool can be
// found at:   http://www.gnu.org/software/flex/

#include "SCC.H"
#include "PTNode.H"
#include "scc-parser.h"

// NOTE: The C-style comments below are intentional.  Flex does not like
// C++-style comments inside of its rules definitions below.  Also, you want
// to make sure that the comment does not start in the first column.  Flex
// does not have any way to know distinguish between the /* comment and the
// regular expression /*

%}


  /* Define the regular expressions for comments, whitespace, and numbers. */
comment     \/\/.*$
ws          [\r\n\t ]+
number      [0-9]+

 /* This option tells Flex that this script is never ran interactively.  This
  * is needed for Windows builds since a few UNIX functions are not
  * implemented by Microsoft. */
%option never-interactive


%%


{comment}   /* comments are ignored */
{ws}        /* whitespace is ignored */

{number}    {
              /* This rule matches any integer found in the input stream.  The
               * matched text will be in the yytext string, and we need to
               * convert it to an integer.  After this is done, create a new
               * constant parse tree node and return this node to the parser.
               * This is done through the Bison-defined variable 'yylval'. */

              int value = strtol( yytext, NULL, 0 );
              yylval = new ConstantNode( value );

              return NUMBER;
            }

 /* This regular expression returns any character that was not matched by a
  * rule above to the parser.  This allows the parser to understand the single
  * letter tokens such as + and - */
.           return yytext[0];


%%


// Flex requires that this function is defined.  It will call this function
// whenever it reaches the end of the input stream.  If this function returns
// 1, then Flex will terminate reading.  This function is commonly used to
// switch to another file when the current one is finished (for example, the
// user entered two scripts on the command line).
int yywrap()
{
  return 1;
}
