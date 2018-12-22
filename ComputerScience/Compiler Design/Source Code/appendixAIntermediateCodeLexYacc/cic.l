%{
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include"cic.h"
#include "y.tab.h"
//#include "ic.tab.h"
int line_no = 1;
%}

%start BM

/*Auxilary definitions for letter, digit, and constants or literals*/
letter [a-zA-Z]
digit [0-9]
decimalLiteral [1-9]([0-9])*
hexLiteral [0][xX]([0-9a-fA-F])+ 
octalLiteral [0]([0-7])*
exponent [eE]([+-])?([0-9])+

%%
"//"(~["\n","\r"])*("\n"|"\r"|"\r\n")	{ ; /*Single Line Comment*/}
"/**"(~["*"])*"*"("*"|(~["*","/"](~["*"])*"*"))*"/"	{ ; /*Formal Comment*/}
"/*"(~["*"])*"*"("*"|(~["*","/"](~["*"])*"*"))*"/"	{ ; /*Multi Line Comment*/}
[ \t\r\f]		{ ; /* Skip white spaces*/}
[\n] 		{BEGIN 0; lineNo++; /*Increment the line number*/}

"auto"		{BEGIN 0; return _auto;	/*Keywords start here*/}
"break"		{BEGIN 0;return _break;}
"case"		{BEGIN 0;return _case;}
"char"		{BEGIN 0;return _char;}
"const"		{BEGIN 0;return _const;}
"continue"	{BEGIN 0;return _continue;}
"default"	{BEGIN 0;return _default;}
"do"		{BEGIN 0;return _do;}
"double"	{BEGIN 0;return _double;}
"else"		{BEGIN 0;return _else;}
"enum"		{BEGIN 0;return _enum;}
"extern"	{BEGIN 0;return _extern;}
"float"		{BEGIN 0;return _float;}
"for"		{BEGIN 0;return _for;}
"goto"		{BEGIN 0;return _goto;}
"if"		{BEGIN 0;return _if;}
"inline"	{BEGIN 0;return _inline;}
"int"		{BEGIN 0;return _int;}
"long"		{BEGIN 0;return _long;}
"register"	{BEGIN 0;return _register;}
"restrict"	{BEGIN 0;return _restrict;}
"return"	{BEGIN 0;return _return;}
"short"		{BEGIN 0;return _short;}
"signed"	{BEGIN 0;return _signed;}
"sizeof"	{BEGIN 0;return _sizeof;}
"static"	{BEGIN 0;return _static;}
"struct"	{BEGIN 0;return _struct;}
"switch"	{BEGIN 0;return _switch;}
"typedef"	{BEGIN 0;return _typedef;}
"union"		{BEGIN 0;return _union;}
"unsigned"	{BEGIN 0;return _unsigned;}
"void"		{BEGIN 0;return _void;}
"volatile"	{BEGIN 0;return _volatile;}
"while"		{BEGIN 0;return _while;}
"bool"		{BEGIN 0;return _bool;}
"complex"	{BEGIN 0;return _complex;}
"_Imaginary" {BEGIN 0;return _Imaginary;}

","		{BEGIN 0;return _comma; /*Separators*/}
";"		{BEGIN 0;return _semicolon;}
"{"		{BEGIN 0;return _leftb; }
"}"		{BEGIN 0;return _rightb;}
"["		{BEGIN 0;return _leftsp;}
"]"		{BEGIN BM;return _rightsp;}
"("		{BEGIN 0;return _leftp;}
")"		{BEGIN BM;return _rightp;}
"="		{BEGIN 0;return _assign;/*Operators*/}
">"		{BEGIN 0;return _gt;}	
"<"		{BEGIN 0;return _lt;}
"!"		{BEGIN 0;return _bang;}
"~"		{BEGIN 0;return _tilde;}
"?" 		{BEGIN 0;return _hook;}
":"		{BEGIN 0;return _colon;}
"=="		{BEGIN 0;return _eq;}
"<="		{BEGIN 0;return _le;}
">="		{BEGIN 0;return _ge;}
"!="		{BEGIN 0;return _ne;}
"++"		{BEGIN 0;return _incr;}
"--"		{BEGIN 0;return _decr;}
"+"		{BEGIN 0;return _plus;}
<BM>"-"		{BEGIN 0;return _minus;}
"-"		{return _uminus;}
"*"		{BEGIN 0;return _mul;}
"/"		{BEGIN 0;return _div;}
"%"		{BEGIN 0;return _modulo;}
"&"		{BEGIN 0;return _bitAnd;}
"|"		{BEGIN 0;return _bitOr;}	
"&&"		{BEGIN 0;return _and;}
"||"		{BEGIN 0;return _or;}	
"^"		{BEGIN 0;return _xor;}
"<<"		{BEGIN 0;return _lShift;}
">>" 		{BEGIN 0;return _rSignedShift;}
">>>"		{BEGIN 0;return _rUnsignedShift;}
"+="		{BEGIN 0;return _plusAssign;}
"-="		{BEGIN 0;return _minusAssign;}
"*="		{BEGIN 0;return _mulAssign;}
"/="		{BEGIN 0;return _divAssign;}
"&="		{BEGIN 0;return _andAssign;}
"|="		{BEGIN 0;return _orAssign;}
"^="		{BEGIN 0;return _xorAssign;}
"%="		{BEGIN 0;return _moduloAssign;}
"<<="		{BEGIN 0;return _lShiftAssign;}
">>="		{BEGIN 0;return _rSignedShiftAssign;}
">>>="		{BEGIN 0;return _rUnsignedShiftAssign;}
"."		{BEGIN 0;return _dot;}
"->"		{BEGIN 0;return _dereferencing;}
{letter}({letter}|{digit})* {		/*Identifier*/
				BEGIN BM;
				strcpy(yylval.attr.name,yytext);
				yylval.attr.type=ID;
				return _id;
			}
({decimalLiteral}(["l","L"])?)|({hexLiteral}(["l","L"])?)|({octalLiteral}(["l","L"])?) 	{ /*Integer Numbers*/
			BEGIN BM;
		 	strcpy(yylval.attr.name,yytext); 
		 	yylval.attr.type=CONST; 
		 	yylval.attr.subtype=INT; 
		 	yylval.attr.value.iVal=atoi(yytext);
		 	return _num;
		}
({digit}+["."]{digit}*{exponent}?["f","F","d","D"]?)|(["."]{digit}+{exponent}?["f","F","d","D"]?)|({digit}+{exponent}["f","F","d","D"]?)|({digit}+{exponent}?["f","F","d","D"]) 	{	/*Real Numbers*/

			BEGIN BM;
			strcpy(yylval.attr.name,yytext);
		 	yylval.attr.type=CONST; 
		 	yylval.attr.subtype=DOUBLE; 
		 	yylval.attr.value.dVal=atof(yytext);
			return _dnum;
		}
[']((~["'","\\","\n","\r"])|("\\"(["n","t","b","r","f","\\","'","\""]|["0"-"7"](["0"-"7"])?|["0"-"3"]["0"-"7"]["0"-"7"])))[']  	{
			/*Character Constant*/  teste
			BEGIN BM;
			strcpy(yylval.attr.name,yytext);
		 	yylval.attr.type=CONST; 
		 	yylval.attr.subtype=CHAR; 
		 	yylval.attr.value.cVal=yytext[0]; 
			return _charcons;
		}
["\""]((~["\"","\\","\n","\r"])|("\\"(["n","t","b","r","f","\\","'","\""]|["0"-"7"](["0"-"7"])?|["0"-"3"]["0"-"7"]["0"-"7"])))*["\""]  	{
			/*String Constant*/
			BEGIN BM;
			strcpy(yylval.attr.name,yytext);
		 	yylval.attr.type=CONST; 
		 	yylval.attr.subtype=STRING;
			yylval.attr.value.sVal = malloc(sizeof(yytext));
		 	strcpy(yylval.attr.value.sVal,yytext); 
			return _strcons;
		}
%%

int yywrap()
{
  return 1;
}

