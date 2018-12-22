%{
#include<string.h>
#include<stdlib.h>
#include"ic.h"
#include "ic.tab.h"
%}
%start BM
%%
[ \t]		;
[\n] 		{BEGIN 0; lineNo++;}
"auto"		{BEGIN 0; return _auto;}
"break"		{BEGIN 0;return _break;}
"case"		{BEGIN 0;return _case;}
"char"		{BEGIN 0;return _char;}
"const"		{BEGIN 0;return _const;}
"continue"	{BEGIN 0;return _continue;}
"default"	{BEGIN 0;return _default;}
"do"		{BEGIN 0;return _do;}
"double"	{BEGIN 0;return _double;}
"else"		{BEGIN 0;return _else;}
"extern"	{BEGIN 0;return _extern;}
"float"		{BEGIN 0;return _float;}
"for"		{BEGIN 0;return _for;}
"goto"		{BEGIN 0;return _goto;}
"if"		{BEGIN 0;return _if;}
"int"		{BEGIN 0;return _int;}
"long"		{BEGIN 0;return _long;}
"main"		{BEGIN 0;return _main;}
"register"	{BEGIN 0;return _register;}
"signed"	{BEGIN 0;return _signed;}
"short"		{BEGIN 0;return _short;}
"static"	{BEGIN 0;return _static;}
"struct"	{BEGIN 0;return _struct;}
"switch"	{BEGIN 0;return _switch;}
"typedef"	{BEGIN 0;return _typedef;}
"union"		{BEGIN 0;return _union;}
"unsigned"	{BEGIN 0;return _unsigned;}
"void"		{BEGIN 0;return _void;}
"volatile"	{BEGIN 0;return _volatile;}
"while"		{BEGIN 0;return _while;}
","		{BEGIN 0;return _comma;}
"="		{BEGIN 0;return _assign;}
";"		{BEGIN 0;return _semicolon;}
":"		{BEGIN 0;return _colon;}
"{"		{BEGIN 0;return _leftb;}
"}"		{BEGIN 0;return _rightb;}
"["		{BEGIN 0;return _leftsp;}
"]"		{BEGIN BM;return _rightsp;}
"("		{BEGIN 0;return _leftp;}
")"		{BEGIN BM;return _rightp;}
"+"		{BEGIN 0;return _plus;}
<BM>"-"		{BEGIN 0;return _minus;}
"-"		{return _uminus;}
"*"		{BEGIN 0;return _mul;}
"/"		{BEGIN 0;return _div;}
"%"		{BEGIN 0;return _modulo;}
">"		{BEGIN 0;return _gt;}
">="		{BEGIN 0;return _ge;}
"<"		{BEGIN 0;return _lt;}
"<="		{BEGIN 0;return _le;}
"=="		{BEGIN 0;return _eq;}
"!="		{BEGIN 0;return _ne;}
"&&"		{BEGIN 0;return _and;}
"||"		{BEGIN 0;return _or;}

[a-zA-Z][a-zA-Z0-9]*	{
				BEGIN BM;
				strcpy(yylval.attr.name,yytext);
				yylval.attr.type=ID;
				return _id;
			}
[0-9]+		{
			BEGIN BM;
		 	strcpy(yylval.attr.name,yytext); 
		 	yylval.attr.type=CONST; 
		 	yylval.attr.datatype=INT; 
		 	yylval.attr.value.iVal=atoi(yytext);
		 	return _num;
		}
(([0-9]*[.][0-9]+)|([0-9]+[.][0-9]*))([eE][+-]?[0-9]+)?	{
			BEGIN BM;
		 	//strcpy(yylval.attr.name,yytext); 
		 	yylval.attr.type=CONST; 
		 	yylval.attr.datatype=DOUBLE; 
		 	yylval.attr.value.dVal=atof(yytext);
			return _dnum;
		}
['].[']		{
			BEGIN BM; 
		 	//yylval.attr.name[0]=yytext[1]; 
		 	//yylval.attr.name[1]='\0';
		 	yylval.attr.type=CONST; 
		 	yylval.attr.datatype=CHAR; 
		 	yylval.attr.value.cVal=yytext[0]; 
			return _charcons;
		}
<<EOF>>		{
			return _eofile;
		}
%%

int yywrap()
{
  return 1;
}
