#ifndef __SCC_H__
#define __SCC_H__

// Every rule in Bison have an associated type or "lvalue".  Bison expects a
// program to override the 'YYSTYPE' macro with the type name.  Here, we
// define all rules in Bison are a PTNode smart pointer.
#define YYSTYPE PTNodePtr

// Extern a few functions that are needed by Flex & Bison to communicate to
// each other.
extern int yyerror( char *err );
extern int yylex();
extern int yyparse();


#endif // __SCC_H__
