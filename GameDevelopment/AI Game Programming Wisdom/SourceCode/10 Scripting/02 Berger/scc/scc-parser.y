%{
// This file contains all of the rules for GNU Bison.  This grammar defines a
// very simple language that can only understand the very basic match
// instructions.  Nevertheless, this still shows the basics of code generation.

#include "SCC.H"
#include "PTNode.H"
#include "CodeGen.H"

%}


// Define the order of precidence for the operators.  The first line has the
// lowest precidence and the last line has the highest.  Operators on the same
// line have the same precidence, and these operators associate to the left.
// For example, the following expressions are equivalent:
//
//    2 + 3 + 5 + 7
//    ( (2 + 3) + 5) + 7
//
// This defines the operator precidence as you would expect.

// Note that these first two lines gets rid of the "dangling else" with
// C-style if statements.  Since Bison only has a single token of look ahead,
// it does not know if an if-statement will contain an else or not.  These two
// rules help Bison get rid of this conflict.
%nonassoc LESS_THAN_ELSE
%nonassoc K_ELSE

%left '='
%left '+' '-'
%left '*' '/'



// Define the set of tokens that the lexer will give the parser.  The compiler
// only understands numbers, basic identifiers, and a small handful of
// keywords.
%token NUMBER
%token IDENTIFIER

%token K_ELSE
%token K_FOR
%token K_IF


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
           cg.StartGen( $1 );
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


// This rule defines what a statement is.  This rule simply passes the "what
// is a statement" definition to sub-rules.
stmt : block_stmt                  { $$ = $1; }
     | expr_stmt                   { $$ = $1; }
     | for_stmt                    { $$ = $1; }
     | if_stmt                     { $$ = $1; }
     ;


// A block statement is any statement list that is surrounded by braces.  This
// is commonly used in if-statements and for-loops to group a body of
// statements.
block_stmt : '{' stmtList '}'      { $$ = $2; }
           ;


// An expression statement is a any expression followed by a semicolon.  This
// will be the most common type of statement in a script.  Since an expression
// will leave its value on the stack, the code generator will generate pops to
// cleanup the stack when it encounters statement nodes.
expr_stmt : expr ';'               { $$ = new StatementNode( $1 ); }
          ;


// This rule defines a C-style for-loop.
for_stmt : K_FOR '(' opt_expr ';' opt_expr ';' opt_expr ';' ')' stmt
             {
               // In order to keep the for-loop's data structure a little more
               // convient, it does not expect any of its arguments to be
               // NULL.  Therefore, we need to create dummy nodes where
               // appropiate.  Note that when the conditional is empty, that
               // means we should really push a constant 1 on the stack (ie,
               // true).
               PTNodePtr pre = $3;
               PTNodePtr expr = $5;
               PTNodePtr post = $7;
               PTNodePtr body = $10;

               if ( pre == NULL )
                 pre = new StatementNode( NULL );

               if ( expr == NULL )
                 pre = new ConstantNode( 1 );

               if ( post == NULL )
                 post = new StatementNode( NULL );

               if ( body == NULL )
                 body = new StatementNode( NULL );

               $$ = new ForNode( pre, expr, post, body );
             }


// This rule defines an optional expression.  This is used by the for-loop so
// its various parts can be empty.
opt_expr : expr                    { $$ = $1;   }
         | /* empty */             { $$ = NULL; }
         ;


// This rule defines a C-style if-statement.  Note that there are two sub
// rules that actually deal with the if-statement's details (whether or not
// the if statement has an else).
if_stmt : if_no_else_stmt          { $$ = $1; }
        | if_with_else_stmt        { $$ = $1; }
        ;


// This rule defines an C-style if-statement that does not contain an else.
// Since Bison only has a single token of look-ahead, this rule conflicts with
// if_with_else_stmt; therefore, we force Bison to drop the precidence of this
// rule.  This effectively binds the else with the closest if (just like C).
if_no_else_stmt : K_IF '(' expr ')'
                    stmt     %prec LESS_THAN_ELSE
                                   { $$ = new IfNode( $3, $5 ); }
                ;


if_with_else_stmt : K_IF '(' expr ')'
                      stmt
                    K_ELSE
                      stmt
                                   { $$ = new IfNode( $3, $5, $7 ); }
                  ;


// This rule defines what an expression.  An expression can be a single number
// (the last rule), or it can be any list basic math instructions.  Each of
// these rules handles creating the proper parse tree node with the operands.
expr : expr '+' expr               { $$ = new AddNode( $1, $3 );        }
     | expr '-' expr               { $$ = new SubtractNode( $1, $3 );   }
     | expr '*' expr               { $$ = new MultiplyNode( $1, $3 );   }
     | expr '/' expr               { $$ = new DivideNode( $1, $3 );     }
     | '(' expr ')'                { $$ = $2;                           }
     | IDENTIFIER '=' expr         { $$ = new AssignmentNode( $1, $3 ); }
     | NUMBER                      { $$ = $1;                           }
     | IDENTIFIER                  { $$ = $1;                           }
     ;


%%


// This function is required to be defined by Bison.  It is called whenever
// any kind of error is raised while parsing the input stream.
int yyerror( char *err )
{
  puts( err );

  return 0;
}
