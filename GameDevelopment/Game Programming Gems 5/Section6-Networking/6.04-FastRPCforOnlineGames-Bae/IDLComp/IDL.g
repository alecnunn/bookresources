// MyRPC IDL grammar definition
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

options {
	language="Cpp";
}

/* A Simple IDL Recognizer authored by Bae,Hyun-jik,imays@hitel.net. */

class IDLParser extends Parser;
options {
	exportVocab=IDL;
}

//********* This is the first plce to parse for IDL file
specification returns [ RefCount<CSpecification> spec ]
	{
		spec=RefCount<CSpecification>(new CSpecification);
		RefCount<CConfigurations> configs;
		RefCount<CFunction> func;
	}
	:	configs=configurations { spec->SetConfigs(configs); }
		(func=functionDefinition { spec->AddFunction(func); } )*
		EOF!
	;

// RPC configurations to be applied to every RPC function
configurations returns [ RefCount<CConfigurations> configs ]
	{
		configs=RefCount<CConfigurations>(new CConfigurations);
		RefCount<CConfiguration> config;
	}
	:
	"config"
	LCURLY
	(config=configuration { configs->AddConfig(config); } )*
	RCURLY
	;
	
// configurations defined in IDL files
configuration returns [ RefCount<CConfiguration> config ]
	{
		config=RefCount<CConfiguration>(new CConfiguration);
	}
	:
	configName:IDENT { config->SetName(configName->getText()); }
	ASSIGN
	configValue:INT { config->SetValue(configValue->getText()); }
	SEMI
	;
	
// an RPC function definition
functionDefinition returns [ RefCount<CFunction> func ]
	{
		func=RefCount<CFunction>(new CFunction);
		RefCount<CParameter> param;
	}
	:
	functionName:IDENT { func->SetName(functionName->getText()); }
	LPAREN
	(
		param=parameterDefinition { func->AddParameter(param); } 
		( COMMA! param=parameterDefinition { func->AddParameter(param); } )* 
	)?
	RPAREN
	SEMI
	;

// an RPC parameter
parameterDefinition returns [ RefCount<CParameter> param ]
	{
		param=RefCount<CParameter>(new CParameter);
	}
	:
	type:IDENT { param->SetType(type->getText()); }
	name:IDENT { param->SetName(name->getText()); }
	;

/********* This is the end of my simple RPC grammar. 
The other grammar definitions below are original IDL grammar. I let them remain because they
may assist you to add more functionality to your own IDL compiler. */

identifier
	:   IDENT
  	;

/* IDL LEXICAL RULES  */
class IDLLexer extends Lexer;
options {
	exportVocab=IDL;
	k=4;
}

SEMI
options {
  paraphrase = ";";
}
	:	';'
	;

QUESTION
options {
  paraphrase = "?";
}
	:	'?'
	;

LPAREN
options {
  paraphrase = "(";
}
	:	'('
	;

RPAREN
options {
  paraphrase = ")";
}
	:	')'
	;

LBRACK
options {
  paraphrase = "[";
}
	:	'['
	;

RBRACK
options {
  paraphrase = "]";
}
	:	']'
	;

LCURLY
options {
  paraphrase = "{";
}
	:	'{'
	;

RCURLY
options {
  paraphrase = "}";
}
	:	'}'
	;

OR
options {
  paraphrase = "|";
}
	:	'|'
	;

XOR
options {
  paraphrase = "^";
}
	:	'^'
	;

AND
options {
  paraphrase = "&";
}
	:	'&'
	;

COLON
options {
  paraphrase = ":";
}
	:	':'
	;

COMMA
options {
  paraphrase = ",";
}
	:	','
	;

DOT
options {
  paraphrase = ".";
}
	:	'.'
	;

ASSIGN
options {
  paraphrase = "=";
}
	:	'='
	;

NOT
options {
  paraphrase = "!";
}
	:	'!'
	;

LT_
options {
  paraphrase = "<";
}
	:	'<'
	;

LSHIFT
options {
  paraphrase = "<<";
}
	: "<<"
	;

GT
options {
  paraphrase = ">";
}
	:	'>'
	;

RSHIFT
options {
  paraphrase = ">>";
}
	: ">>"
	;

DIV
options {
  paraphrase = "/";
}
	:	'/'
	;

PLUS
options {
  paraphrase = "+";
}
	:	'+'
	;

MINUS
options {
  paraphrase = "-";
}
	:	'-'
	;

TILDE
options {
  paraphrase = "~";
}
	:	'~'
	;

STAR
options {
  paraphrase = "*";
}
	:	'*'
	;

MOD
options {
  paraphrase = "%";
}
	:	'%'
	;

SCOPEOP
options {
  paraphrase = "::";
}
	:  	"::"
	;

WS_
options {
  paraphrase = "white space";
}
	:	(' '
	|	'\t'
	|	'\n'  { newline(); }
	|	'\r')
		{ $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP); }
	;

PREPROC_DIRECTIVE
options {
  paraphrase = "a preprocessor directive";
}

	:
	'#'
	(~'\n')* '\n'
	{ $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP); }
	;

SL_COMMENT
options {
  paraphrase = "a comment";
}

	:
	"//"
	(~'\n')* '\n'
	{ $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP); newline(); }
	;

ML_COMMENT
options {
  paraphrase = "a comment";
}
	:
	"/*"
	(
			STRING_LITERAL
		|	CHAR_LITERAL
		|	'\n' { newline(); }
		|	'*' ~'/'
		|	~'*'
	)*
	"*/"
	{ $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP);  }
	;

CHAR_LITERAL
options {
  paraphrase = "a character literal";
}
	:
	'\''
	( ESC | ~'\'' )
	'\''
	;

STRING_LITERAL
options {
  paraphrase = "a string literal";
}
	:
	'"'
	(ESC|~'"')*
	'"'
	;

protected
ESC
options {
  paraphrase = "an escape sequence";
}
	:	'\\'
		(	'n'
		|	't'
		|	'v'
		|	'b'
		|	'r'
		|	'f'
		|	'a'
		|	'\\'
		|	'?'
		|	'\''
		|	'"'
		|	('0' | '1' | '2' | '3')
			(
				/* Since a digit can occur in a string literal,
				 * which can follow an ESC reference, ANTLR
				 * does not know if you want to match the digit
				 * here (greedy) or in string literal.
				 * The same applies for the next two decisions
				 * with the warnWhenFollowAmbig option.
				 */
				options {
					warnWhenFollowAmbig = false;
				}
			:	OCTDIGIT
				(
					options {
						warnWhenFollowAmbig = false;
					}
				:	OCTDIGIT
				)?
			)?
		|   'x' HEXDIGIT
			(
				options {
					warnWhenFollowAmbig = false;
				}
			:	HEXDIGIT
			)?
		)
	;

protected
VOCAB
options {
  paraphrase = "an escaped character value";
}
	:	'\3'..'\377'
	;

protected
DIGIT
options {
  paraphrase = "a digit";
}
	:	'0'..'9'
	;

protected
OCTDIGIT
options {
  paraphrase = "an octal digit";
}
	:	'0'..'7'
	;

protected
HEXDIGIT
options {
  paraphrase = "a hexadecimal digit";
}
	:	('0'..'9' | 'a'..'f' | 'A'..'F')
	;

/* octal literals are detected by checkOctal */

HEX
options {
  paraphrase = "a hexadecimal value value";
}

	:    ("0x" | "0X") (HEXDIGIT)+
	;

INT
options {
  paraphrase = "an integer value";
}
	:    (DIGIT)+                  // base-10
             (  '.' (DIGIT)*                      	{$setType(FLOAT);}
	         (('e' | 'E') ('+' | '-')? (DIGIT)+)?
	     |   ('e' | 'E') ('+' | '-')? (DIGIT)+   	{$setType(FLOAT);}
             )?
	;

FLOAT
options {
  paraphrase = "an floating point value";
}

	:    '.' (DIGIT)+ (('e' | 'E') ('+' | '-')? (DIGIT)+)?
     	;

IDENT
options {
  testLiterals = true;
  paraphrase = "an identifer";
}

	:	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*
	;
