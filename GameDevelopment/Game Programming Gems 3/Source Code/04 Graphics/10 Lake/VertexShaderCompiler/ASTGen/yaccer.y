%{


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "CastNode.h"
#include "CSymbolTable.h"
#include "CCodeGenerator.h"

//this is the root node for the main AST (abstract syntax tree) sent to the codegenerator
CastNode *root = NULL;
//declaration of symbol table used during compilation.
CSymbolTable symTab;

int eWhichRegComponent = eRegCompNone;

int yyerror(char *s);

extern int yylex();

//needed this so it would compile
int isatty(int flag) { return 0; }
%}

%union {
	float fval;
	int ival;
	int token;
	char *sval;
	//can't make it an CastNode because its a C++ class
	void *node;
	}


%token <sval> T_STRING
%token <fval> T_FLOAT
%token <token> T_PLUS T_MINUS T_MULTIPLY T_DIVIDE
%token <token> T_DOT4 T_NORMALIZE T_SQRT T_NEGATE T_POWER T_MAXWITH0 T_CLAMPTO1 T_SIN T_COS T_FLOOR T_CEILING
%token <token> T_EQUALS T_LBRACE T_RBRACE T_LPAREN T_RPAREN T_SEMICOLON T_COMMA
%token <token> T_TYPE_FLOAT T_TYPE_CLAMP

%type <node> Program Functions Function Expr StmtList Stmt

%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE T_DOT4 T_NORMALIZE T_SQRT T_NEGATE T_POWER T_DOT3

%%
Program:	Functions {
					printf("DONE\n");
					$$ = new CastNode("Program");
					((CastNode *)$$)->setDescription("Beginning of function");
					((CastNode *)$$)->addChildNode(0,(CastNode *)$1);			
					root = (CastNode *)$$;
					//CastNode::inOrderTreeWalk(root);
					}

Functions: Functions Function {
								printf("functions function\n");
								$$ = new CastNode("Functions");
								((CastNode *)$$)->setDescription("List of Functions");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								((CastNode *)$$)->addChildNode(1,(CastNode *)$2);
								}
		|  Function				{
								printf("function\n");
								$$ = new CastNode("Functions");
								((CastNode *)$$)->setDescription("Single Function");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								}

Function: Expr T_LPAREN T_RPAREN T_LBRACE StmtList T_RBRACE {
															//TODO: this maybe should not be expr but a T_STRING instead?
															printf("FUNCTION is complete\n");
															$$ = new CastNode("Function");
															((CastNode *)$$)->setDescription("Function");
															((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
															((CastNode *)$$)->addChildNode(4,(CastNode *)$5);
															}

StmtList: StmtList Stmt 
						{
						printf("stmt_list: stmt_list stmt\n");
						$$ = new CastNode("StmtList");
						((CastNode *)$$)->setDescription("List of Statements");
						((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
						((CastNode *)$$)->addChildNode(1,(CastNode *)$2);
						}
		|  Stmt 
				{
				printf("stmt_list: stmt\n");
				$$ = new CastNode("StmtList");
				((CastNode *)$$)->setDescription("List of Statements");
				((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
				}

Stmt: Expr T_EQUALS Expr T_SEMICOLON	{
										printf("expr EQUALS expr\n");
										$$ = new CastNode("Stmt");
										((CastNode *)$$)->setDescription("equals");
										//CastNode::inOrderTreeWalk((CastNode *)$1);
										((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
										
										CastNode *pNewNode = new CastNode("Equals");
										
										//((CastNode *)$1)->setDescription("Equals description");
										((CastNode *)$$)->addChildNode(1, (CastNode *)pNewNode);
									
										printf("******************************\n");
										CastNode::inOrderTreeWalk((CastNode *)$3);
										printf("******************************\n");
										((CastNode *)$$)->addChildNode(2,(CastNode *)$3);		
										}
	| T_TYPE_FLOAT T_STRING T_SEMICOLON {
										printf("declaration\n");
										$$ = new CastNode("Stmt");
										((CastNode *)$$)->setDescription("float declaration ");
										//TODO: figure out how i want to do this
										//determine if symbol exists, if not, add to symbol table, if it does
										symTab.AddSymbol($2,"float",T_STRING,eRegTypeNone,-1);
										//if necessary, add a child node with the proper data
										}
	| T_TYPE_CLAMP T_STRING T_SEMICOLON {
										printf("declaration\n");
										$$ = new CastNode("Stmt");
										((CastNode *)$$)->setDescription("clamp declaration MORE STUFF WILL GO HERE");
										symTab.AddSymbol($2,"clamp",T_STRING,eRegTypeNone,-1);
										//TODO: figure out how i want to do this
										
										
										}
//////////////////////////////////////////////////////////
//This is the Expr stuff here, meat
/////////////////////////////////////////////////////////

Expr: Expr T_PLUS Expr {
								printf("expr complete\n");
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_PLUS");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);
						}
Expr: Expr T_MINUS Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_MINUS");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}
Expr: Expr T_MULTIPLY Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_MULTIPLY");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}
Expr: Expr T_DIVIDE Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_DIVIDE");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}
Expr: Expr T_DOT4 Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_DOT4");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}
Expr: Expr T_DOT3 Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_DOT3");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}

Expr: Expr T_POWER Expr {
								$$ = new CastNode("Expr");
								((CastNode *)$$)->addChildNode(0,(CastNode *)$1);
								CastNode *pNewNode = new CastNode("T_POWER");
								((CastNode *)$$)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)$$)->addChildNode(2,(CastNode *)$3);

						}

Expr: T_NORMALIZE T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_NORMALIZE");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_SQRT T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_SQRT");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_MAXWITH0 T_LPAREN Expr T_RPAREN 
						{
							printf("recognized maxwith0\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_MAXWITH0");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}

Expr: T_NEGATE T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_NEGATE");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_CLAMPTO1 T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_CLAMPTO1");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_SIN T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_SIN");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_COS T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_COS");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_FLOOR T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_FLOOR");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}
Expr: T_CEILING T_LPAREN Expr T_RPAREN 
						{
							printf("recognized normalize\n");
							$$ = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_CEILING");
							((CastNode *)$$)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)$$)->addChildNode(1,(CastNode *)$3);
						}


Expr: T_STRING				{
								printf("expr: STRING: %s is string  \n", $1);
								$$ = new CastNode("Expr");
								((CastNode *)$$)->setDescription("T_STRING");
								((CastNode *)$$)->sval = strdup(yylval.sval);
								((CastNode *)$$)->setRegComponent(eWhichRegComponent);
							}
	| T_FLOAT
							{
								$$ = new CastNode("Expr");
								((CastNode *)$$)->setDescription("T_FLOAT");
								((CastNode *)$$)->fval = yylval.fval;
							}
	| T_LPAREN Expr T_RPAREN {
							$$ = $2;
							}
	|  T_LPAREN Expr T_COMMA Expr T_COMMA Expr T_COMMA Expr T_RPAREN 
							{
								printf("FLOAT VECTOR RECOGNIZED\n");
								$$ = new CastNode("Expr");
								//create the vector node subgraph
								CastNode *pVectorNode = NULL;
								pVectorNode = new CastNode("Vector");
								pVectorNode->addChildNode(0,(CastNode *)$2);
								pVectorNode->addChildNode(1,(CastNode *)$4);
								pVectorNode->addChildNode(2,(CastNode *)$6);
								pVectorNode->addChildNode(3,(CastNode *)$8);
								//attach it to the rest of the graph
								((CastNode *)$$)->addChildNode(0,pVectorNode);
								
							}
%%
extern FILE *yyin;

void InsertSymbols();
void CodeGen(char *outputFileName);

/*
	main()

	Main entry point for entire program.  
	First, insert symbols (keywords) into the symbol table.
	Second, call yyparse() which is the main parsing function.
	Third, call the code Generator to generate code using the AST generated by the 
	scanner and parser in the function yyparse().
	
	*/
void main(int argc, char **argv)
{
	InsertSymbols();
	printf("%s is file to open\n",argv[1]);
	if(argc > 1)
	{
		FILE *file;

		file = fopen(argv[1], "r");
		if(!file)
		{
			printf("Error: Could not open file %s\n",argv[1]);
			exit(-1);
		}
		else
		{ 
			yyin = file;
		}
		yyparse();
	}
	printf("Program Parse Complete\nPress Any Key to Start Code Generation...");
	getchar();
	CodeGen(argv[2]);
	printf("Code Generation Complete\nPress Any Key to End");
	getchar();
}

/* 
	yyerror()
	
	yyerror is the built-in error function.  See Levine's Lex and Yacc book for more information on
	using this function to report errors.  It is expected you overload this with a more sophisticated
	error handling routine.
*/

int yyerror(char *s)
{
	fprintf(stderr,"%s IS MY ERROR\n",s);
	return 1;
}

/*
	InsertSymbols()

	This function adds symbols (mostly keywords) to the symbol table to be used
	during parsing.
*/

void InsertSymbols()
{

#define NEXTAVAILABLEREG 36.y

	symTab.AddSymbol("float","keyword",T_TYPE_FLOAT,eRegTypeNone,-1);
	symTab.AddSymbol("clamp","keyword",T_TYPE_CLAMP,eRegTypeNone,-1);

	symTab.AddSymbol("+","keyword",T_PLUS,eRegTypeNone,-1);
	symTab.AddSymbol("-","keyword",T_MINUS,eRegTypeNone,-1);
	symTab.AddSymbol("*","keyword",T_MULTIPLY,eRegTypeNone,-1);
	symTab.AddSymbol("/","keyword",T_DIVIDE,eRegTypeNone,-1);
	symTab.AddSymbol("^","keyword",T_POWER,eRegTypeNone,-1);

	symTab.AddSymbol("'dot4'","keyword",T_DOT4,eRegTypeNone,-1);
	symTab.AddSymbol("'dot3'","keyword",T_DOT3,eRegTypeNone,-1);
	symTab.AddSymbol("'normalize'","keyword",T_NORMALIZE,eRegTypeNone,-1);
	symTab.AddSymbol("'sqrt'","keyword",T_SQRT,eRegTypeNone,-1);
	symTab.AddSymbol("'negate'","keyword",T_NEGATE,eRegTypeNone,-1);
	symTab.AddSymbol("'maxWith0'","keyword",T_MAXWITH0,eRegTypeNone,-1);
	symTab.AddSymbol("'clampTo1'","keyword",T_CLAMPTO1,eRegTypeNone,-1);
	symTab.AddSymbol("'cos'","keyword",T_COS,eRegTypeNone,-1);
	symTab.AddSymbol("'sin'","keyword",T_SIN,eRegTypeNone,-1);
	symTab.AddSymbol("'floor'","keyword",T_FLOOR,eRegTypeNone,-1);
	symTab.AddSymbol("'ceiling'","keyword",T_CEILING,eRegTypeNone,-1);


	symTab.AddSymbol("=","keyword",T_EQUALS,eRegTypeNone,-1);
	symTab.AddSymbol("{","keyword",T_LBRACE,eRegTypeNone,-1);
	symTab.AddSymbol("}","keyword",T_RBRACE,eRegTypeNone,-1);
	symTab.AddSymbol("(","keyword",T_LPAREN,eRegTypeNone,-1);
	symTab.AddSymbol(")","keyword",T_RPAREN,eRegTypeNone,-1);
	symTab.AddSymbol(";","keyword",T_SEMICOLON,eRegTypeNone,-1);
	symTab.AddSymbol(",","keyword",T_COMMA,eRegTypeNone,-1);

	//shader keywords	
	//Matrix Info
	symTab.AddSymbol("MatrixWorldView","keyword",T_STRING,eRegTypeConst,4);
	symTab.AddSymbol("MatrixView","keyword",T_STRING,eRegTypeConst,12);
	symTab.AddSymbol("MatrixProj","keyword",T_STRING, eRegTypeConst,28);
	symTab.AddSymbol("ViewerPos","keyword",T_STRING, eRegTypeConst,29);


	//Materials and Light data
	symTab.AddSymbol("LightDir0","keyword",T_STRING, eRegTypeConst,26);
	symTab.AddSymbol("LightAmb0","keyword",T_STRING, eRegTypeConst,24);
	symTab.AddSymbol("LightDif0","keyword",T_STRING, eRegTypeConst,25);
	symTab.AddSymbol("LightSpec0","keyword",T_STRING, eRegTypeConst,31);
	symTab.AddSymbol("LightPos0","keyword",T_STRING, eRegTypeConst,20);

	symTab.AddSymbol("LightDir1","keyword",T_STRING, eRegTypeConst,37);
	symTab.AddSymbol("LightAmb1","keyword",T_STRING, eRegTypeConst,38);
	symTab.AddSymbol("LightDif1","keyword",T_STRING, eRegTypeConst,39);
	symTab.AddSymbol("LightSpec1","keyword",T_STRING, eRegTypeConst,40);
	symTab.AddSymbol("LightPos1","keyword",T_STRING, eRegTypeConst,41);

	symTab.AddSymbol("LightDir2","keyword",T_STRING, eRegTypeConst,42);
	symTab.AddSymbol("LightAmb2","keyword",T_STRING, eRegTypeConst,43);
	symTab.AddSymbol("LightDif2","keyword",T_STRING, eRegTypeConst,44);
	symTab.AddSymbol("LightSpec2","keyword",T_STRING, eRegTypeConst,45);
	symTab.AddSymbol("LightPos2","keyword",T_STRING, eRegTypeConst,46);

	symTab.AddSymbol("LightDir3","keyword",T_STRING, eRegTypeConst,47);
	symTab.AddSymbol("LightAmb3","keyword",T_STRING, eRegTypeConst,48);
	symTab.AddSymbol("LightDif3","keyword",T_STRING, eRegTypeConst,49);
	symTab.AddSymbol("LightSpec3","keyword",T_STRING, eRegTypeConst,50);
	symTab.AddSymbol("LightPos3","keyword",T_STRING, eRegTypeConst,51);
	
	//when there is only one material light combo, as in a simple demo app

	symTab.AddSymbol("PostCombinedDiffuse","keyword",T_STRING, eRegTypeConst,21);	
	
	symTab.AddSymbol("MatAmb","keyword",T_STRING, eRegTypeConst,22);
	//used in place of PostCombinedDiffuse now
	symTab.AddSymbol("MatDif","keyword",T_STRING, eRegTypeConst,21);
	symTab.AddSymbol("MatSpec","keyword",T_STRING, eRegTypeConst,27);
	symTab.AddSymbol("MatShininess","keyword",T_STRING, eRegTypeConst,30);
	
	//Vertex data
	symTab.AddSymbol("Pos","keyword",T_STRING,eRegTypeVertex, 0);
	symTab.AddSymbol("Normal","keyword",T_STRING,eRegTypeVertex, 3);
	symTab.AddSymbol("TexCoord","keyword",T_STRING,eRegTypeVertex, 6);

	//Output Regs
	symTab.AddSymbol("oPos","keyword",T_STRING,eRegTypeOutputPosition, 0 );
	symTab.AddSymbol("oColor","keyword",T_STRING,eRegTypeOutputColor, 0);
	symTab.AddSymbol("oFog","keyword",T_STRING,eRegTypeOutputFog, 0);
	symTab.AddSymbol("oTexture","keyword",T_STRING,eRegTypeOutputTexture, 0);
	
}

/*
	CodeGen()

	This function is the entry point to the other half of the compiler.
	Here, we create a code generator, give it a handle to a file to write to, 
	then pass it a program in the form of an AST
*/

void CodeGen(char *outputFileName)
{
	//call the codeGen class with an AST pointer
	FILE *fp = NULL;

	fp = fopen(outputFileName,"w");
	if(fp == NULL)
	{
		printf("output.vsh open for writing failed\n");
	}

	CCodeGenerator *pCodeGenerator = new CCodeGenerator();
	pCodeGenerator->SetFileHandle(fp);
	pCodeGenerator->HandleProgram(root);
	
	if(fp != NULL)	fclose(fp);
	delete pCodeGenerator;

}