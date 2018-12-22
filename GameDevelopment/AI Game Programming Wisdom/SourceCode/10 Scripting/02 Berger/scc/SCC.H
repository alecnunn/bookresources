#ifndef __SCC_H__
#define __SCC_H__

// Silly useless VC++ warnings...
#pragma warning( disable : 4786 )

#include <assert.h>
#include <string>
#include <map>

#include "PTNode.H"

// The symbol table contains mappings from a string that the script contains
// to the compiler's identifier node that represents this symbol.  The symbol
// table ensures that all references to a variable points to the same internal
// data structure.  In a more sophisticated compiler, the symbol table is more
// sophisticated to allow for scopes, objects, functions, etc.
using namespace std;

typedef map< string, IdentifierNodePtr > SymbolTable;
extern SymbolTable symtbl;


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
