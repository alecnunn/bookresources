
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yaccer_tab.h"
#include "CSymbolTable.h"
#include "CastNode.h"

int iID;
extern CSymbolTable symTab;
extern CastNode *root;
extern int eWhichRegComponent;

%}

%%

float					|
clamp					{
							printf("lex found keyword float\n");
							iID = symTab.GetIDFromName(yytext);
							if(iID == -1)
							{
								printf("Error: ID %s not a keyword\n",yytext);
								exit(-1);
							}
							yylval.sval = strdup(yylval.sval);
							return(iID);
						}
MatrixWorldViewProj		|
MatrixWorldView			|
MatrixView				|
ViewerPos				|
LightDir0				|
LightAmb0				|
LightDif0				|
LightSpec0				|
LightPos0				|
LightDir1				|
LightAmb1				|
LightDif1				|
LightSpec1				|
LightPos1				|
LightDir2				|
LightAmb2				|
LightDif2				|
LightSpec2				|
LightPos2				|
LightDir3				|
LightAmb3				|
LightDif3				|
LightSpec3				|
LightPos3				|
PostCombinedDiffuse		|
MatAmb					|
MatDif					|
MatSpec					|
MatShininess			|
MatProj					|
Pos						|
Normal					|
TexCoord				|
oPos					|
oColor					|
oFog					|
oTexture				{
							printf("lex found shader keyword: %s\n",yytext);
							yylval.sval = strdup(yytext);
							return(T_STRING);
						}
([a-zA-Z]+[0-9]*)   { 
					printf("lex found string %s\n", yytext); 
					yylval.sval = strdup(yytext); 
					printf("%s is sval\n",yylval.sval);
					eWhichRegComponent = eRegCompAll;
					return(T_STRING);
				  }
([a-zA-Z]+[0-9]*)(".x") {
					printf("lex found string %s\n", yytext); 
					yylval.sval = strdup(yytext); 
					printf("%s is sval\n",yylval.sval);
					eWhichRegComponent = eRegCompX;
					return(T_STRING);
				  }
([a-zA-Z]+[0-9]*)(".y") {
					printf("lex found string %s\n", yytext); 
					yylval.sval = strdup(yytext); 
					printf("%s is sval\n",yylval.sval);
					eWhichRegComponent = eRegCompY;
					return(T_STRING);
				  }
([a-zA-Z]+[0-9]*)(".z") {
					printf("lex found string %s\n", yytext); 
					yylval.sval = strdup(yytext); 
					printf("%s is sval\n",yylval.sval);
					eWhichRegComponent = eRegCompZ;
					return(T_STRING);
				  }
([a-zA-Z]+[0-9]*)(".w") {
					printf("lex found string %s\n", yytext); 
					yylval.sval = strdup(yytext); 
					printf("%s is sval\n",yylval.sval);
					eWhichRegComponent = eRegCompW;
					return(T_STRING);
				  }

([0-9]+\.[0-9]*)|([0-9]+)	 {
								printf("lex found float %s\n", yytext); 
								yylval.fval = (float)atof(yytext); 
								printf("%f is float\n",yylval.fval);
								return(T_FLOAT); 
							 }



"+"				|
"-"				|
"*"				|
"/"				|
"\^"			|
"'dot4'"		|
"'normalize'"	|
"'sqrt'"		|
"'negate'"		|
"'maxWith0'"	|
"'dot3'"		|
"'clamptTo1'"	|
"'sin'"			|
"'cos'"			|
"'floor'"		|
"'ceiling'"		{   /*Operations */
						iID = symTab.GetIDFromName(yytext);
						printf("%s is recognized\n",yytext);
							if(iID == -1)
							{
								printf("Error: ID %s not a keyword\n",yytext);
								exit(-1);
							}
							return(iID);
							}


"="						{	/*Relational ops*/
							iID = symTab.GetIDFromName(yytext);
							if(iID == -1)
							{
								printf("Error: ID %s not a keyword\n",yytext);
								exit(-1);
							}
							return(iID);
						}
"{"|"}"|"("|")"|";"|","	{		/*Delimiters*/
							iID = symTab.GetIDFromName(yytext);
							if(iID == -1)
							{
								printf("Error: ID %s not a keyword\n",yytext);
								exit(-1);
							}
							return(iID);
						}

\.[0-9]+		  { 
					printf("found clamp %s\n",yytext);  
					//yylval.fval = (float)atof(yytext); 
					//return(CLAMP); 
				   }
--.*				{
					// printf("found comment\n");
					}


.|\n		 {  
				printf("singleton \n");
				//yylval.sval = yytext;
				///return(yytext[0]);
				//return(MINUS);
			 }

%%

int yywrap() {return 1;}

/*
void main(int argc, char **argv)
{
	yylex();
}
*/