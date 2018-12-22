%{
// This file contains all of the rules for GNU Bison.  This grammar defines a
// very simple language that can only understand the very basic match
// instructions.  Nevertheless, this still shows the basics of code generation.

#include "SCC.H"
#include "PTNode.H"
#include "CodeGen.H"

%}


// These two lines defines the order of precidence for the operators.  The
// first line has the lowest precidence and the last line has the highest.
// Operators on the same line have the same precidence, and these operators
// associate to the left.  For example, the following expressions are
// equivalent:
//
//    2 + 3 + 5 + 7
//    ( (2 + 3) + 5) + 7
//
// This defines the operator precidence as you would expect.
%left '+' '-'
%left '*' '/'


// Define the set of tokens that the lexer will give the parser.  Currently,
// the only token that the parser understands is a number.  A more robust
// compiler would understand identifiers, keywords, and other tokens.
%token NUMBER


%%


// This rule is where Bison starts (since this is the first rule), and it
// defines what a valid source file.  For this sample, a source file is simply
// a list of statements.
file : stmtList
         {
           // This block of code will be executed once the statement list is
           // "reduced" or completely matched.  Since this is the first rule
           // that Bison started with, parsing of the source script has been
           // completed.  We dump the parse tree that was built up, and then
           // run the code generator over this parse tree.

           // Note that the $1, $2, etc match the "return results" of the of
           // the rule's arguments (e.g., the right side of the colon).  $1 in
           // this rule's case is the result of 'stmtList'.  Rules can set
           // their return values by setting $$.

           $1->Dump();

           CodeGen cg;
           cg.Gen( $1 );
         }
     ;


// This rule defines a list of one or more statements.  Notice how this rule
// refers to itself recusively.  The second part of this rule (just the single
// 'stmt') will always be matched first.  This little bit of code handles
// creating a block parse tree node that will contain the entire list of
// statements.  If there are any additional statements, they will be matched
// by the first part of this rule (notice how its code adds the new statement
// to the end of block node).
stmtList : stmtList stmt           { $$ = $1;  $$->Add( $2 );  }
         | stmt                    { $$ = new BlockNode( $1 ); }
         ;


// This rule defines what a statement is.  For this sample, a statement is
// simply an expression that is terminated by a semi-colon.  In a more complex
// language, other statements would be if statements, while loops, for loops,
// etc.
stmt : expr ';'                    { $$ = new StatementNode( $1 ); }
     ;


// This rule defines what an expression.  An expression can be a single number
// (the last rule), or it can be any list basic math instructions.  Each of
// these rules handles creating the proper parse tree node with the operands.
expr : expr '+' expr               { $$ = new AddNode( $1, $3 );      }
     | expr '-' expr               { $$ = new SubtractNode( $1, $3 ); }
     | expr '*' expr               { $$ = new MultiplyNode( $1, $3 ); }
     | expr '/' expr               { $$ = new DivideNode( $1, $3 );   }
     | '(' expr ')'                { $$ = $2;                         }
     | NUMBER                      { $$ = $1;                         }
     ;


%%


// This function is required to be defined by Bison.  It is called whenever
// any kind of error is raised while parsing the input stream.
int yyerror( char *err )
{
  puts( err );

  return 0;
}
