/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ linetok.cpp   - facilities to break a line of text into a         +
+                 series of tokens.                                 +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define TOK_IDENTIFIER	0
#define TOK_INT_REG		1
#define TOK_FLT_REG		2
#define TOK_DBL_REG		3
#define TOK_CHAR_CONST	4
#define TOK_INT_CONST	5
#define TOK_FLT_CONST	6
#define TOK_COMMA		7
#define TOK_NO_MORE		8
#define TOK_BAD			9

#define ID_SIZE	256		/* max. chars in identifier */

#define EOL		'\0'

/*#define LINE_TOK_DEBUG		1*/

#ifdef LINE_TOK_DEBUG
#define	LINE_TOK_DEBUG0(arg);				printf(arg);
#define LINE_TOK_DEBUG1(arg1,arg2);			printf(arg1,arg2);
#define LINE_TOK_DEBUG2(arg1,arg2,arg3);	printf(arg1,arg2,arg3);
#else
#define	LINE_TOK_DEBUG0(arg);				
#define LINE_TOK_DEBUG1(arg1,arg2);			
#define LINE_TOK_DEBUG2(arg1,arg2,arg3);			
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations                                                      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *TokStr[] = {"TOK_IDENTIFIER","TOK_INT_REG","TOK_FLT_REG",
                 "TOK_DBL_REG","TOK_CHAR_CONST","TOK_INT_CONST",
				 "TOK_FLT_CONST","TOK_COMMA","TOK_NO_MORE","TOK_BAD"};

struct Token
{
	char text[ID_SIZE];
	char *fName;	/*file reading assembly code from*/
	U4 line;		/* line number in source file */
	U1 type;		/* type of token */
	S8 val;			/* register bytecode val, char val, integer const */
	F8 fval;		/* floating-pt val */
};

class LineTokenizer
{
	char tokenBuffer[LINE_SIZE];	/*whole line of assembler text*/
	int ntChars;					/*number chars + null char */
	int itChars;					/*index into buffer*/
	struct Line *lineptr;			/*ptr to Line fed to constructor*/

	/*called by proces----- functions*/
	char getNextLineChar();
	void goBackOneChar();
	char skipLineWhiteSpace();

	/*called by getNextLineToken*/
	void processRegister(struct Token *tptr);
	void processCharConst(struct Token *tptr);
	void processIdentifier(struct Token *tptr, char ch);
	void processNumConst(struct Token *tptr, char ch);

	public:
	LineTokenizer(struct Line *ln);

	/*called by match*/
	struct Token getNextLineToken(); 
	
	U1 match(struct Token *tptr, int ttype); /* big one */
	void printToken(struct Token *tptr);
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LineTokenizer::LineTokenizer(struct Line *ln)
{
	LINE_TOK_DEBUG2("LineTokenizer::LinkeTokenizer(): fed \"%s\" at line (%lu)\n",(*ln).src,(*ln).line);
	lineptr = ln;
	strcpy(tokenBuffer,(*ln).src);
	ntChars = strlen(tokenBuffer)+1; /*include null char*/
	itChars = -1; 
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

char LineTokenizer::getNextLineChar()
{
	itChars++; /* can be in range 0,1, ..., (ntChars-1)*/

	if(itChars>=ntChars-1)
	{
		return(EOL);
	}

	return(tokenBuffer[itChars]);

}/*end getNextLineChar*/

/*-----------------------------------------------------------------*/

void LineTokenizer::goBackOneChar()
{
	if(itChars >= 0){ itChars--; }
	else
	{ 
		LINE_TOK_DEBUG1("LineTokenizer::goBackOneChar(): decrement below 0, %d\n",itChars); 
	} 
	return;

}/*end goBackOneChar*/

/*-----------------------------------------------------------------*/

char LineTokenizer::skipLineWhiteSpace()
{
	char ch;
	ch=getNextLineChar();
	while((ch==' ')||(ch=='\t'))
	{
		ch=getNextLineChar();
	}
	return(ch);

}/*end skipLineWhiteSpace*/

/*-----------------------------------------------------------------*/

/*
	Gameplan:  
	
	i) skip white space ( space or tab )
	ii) look at first char ( determines tok type )
			$ = register ( $R1, $F1, $D1 )
			' = char constant ( 'a' )
			a-z, A-Z, @, _, ?, .  = identifier
			0-9, +, - = numeric constant
			, = comma
			'\0' = end of string
	iii) keep reading until
			-hit end of line
			-reach char not belonging to tok type
	iv) populate token attributes and return
*/

struct Token LineTokenizer::getNextLineToken()
{
	struct Token token;
	char current;
	
	/* still need to set text,type,val/fval*/

	token.line = (*lineptr).line; 
	token.fName = (*lineptr).fName;
	token.val  = 0; 
	token.fval = 0.0; 

	current = skipLineWhiteSpace();

	if(current=='$'){ processRegister(&token); }

	else if(current=='\''){ processCharConst(&token); }

	else if(((current>='a')&&(current<='z'))||
		    ((current>='A')&&(current<='Z'))||
			(current=='@')||
			(current=='_')||
			(current=='?')||
			(current=='.')){ processIdentifier(&token,current); }

	else if(((current>='0')&&(current<='9'))||
		    (current=='-')||
			(current=='+')){ processNumConst(&token,current); }

	else if(current==',')
	{
		token.text[0]=current;
		token.text[1]='\0';
		token.type = TOK_COMMA; 
	}
	else if(current==EOL)
	{
		LINE_TOK_DEBUG2("LineTokenizer::getNextLineToken(): hit EOL line (%lu) index(%d)\n",token.line,itChars);
		strcpy(token.text,"EOL");
		token.type = TOK_NO_MORE; 
	}
	else
	{
		token.text[0]=current;
		token.text[1]='\0';
		token.type = TOK_BAD; 
	}

	return(token);

}/*end getNextLineToken*/

/*-----------------------------------------------------------------*/

void LineTokenizer::processRegister(struct Token *tptr)
{
	char current;
	current = getNextLineChar();

	switch(current)
	{
		/*INT_REGISTER-----------------------------------------*/
		case 'R':
		case 'r':
		{
			current = getNextLineChar();

			switch(current)
			{
				case '1': /* $R1- */
				{
					char peek = getNextLineChar();

					switch(peek)
					{
						case '0':
						{
							strcpy((*tptr).text,"$R10");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R10;
						}break;
						case '1':
						{
							strcpy((*tptr).text,"$R11");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R11;
						}break;
						case '2':
						{
							strcpy((*tptr).text,"$R12");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R12;
						}break;
						case '3':
						{
							strcpy((*tptr).text,"$R13");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R13;
						}break;
						case '4':
						{
							strcpy((*tptr).text,"$R14");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R14;
						}break;
						case '5':
						{
							strcpy((*tptr).text,"$R15");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R15;
						}break;
						case '6':
						{
							strcpy((*tptr).text,"$R16");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R16;
						}break;
						case '7':
						{
							strcpy((*tptr).text,"$R17");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R17;
						}break;
						case '8':
						{
							strcpy((*tptr).text,"$R18");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R18;
						}break;
						case '9':
						{
							strcpy((*tptr).text,"$R19");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R19;
						}break;
						default:
						{
							strcpy((*tptr).text,"$R1");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R1;
							goBackOneChar();
						}
					}/*end of $R1 sub-switch*/
				}break;
				case '2': /* $R2- */
				{
					char peek = getNextLineChar();

					switch(peek)
					{
						case '0':
						{
							strcpy((*tptr).text,"$R20");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R20;
						}break;
						case '1':
						{
							strcpy((*tptr).text,"$R21");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R21;
						}break;
						case '2':
						{
							strcpy((*tptr).text,"$R22");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R22;
						}break;
						case '3':
						{
							strcpy((*tptr).text,"$R23");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R23;
						}break;
						case '4':
						{
							strcpy((*tptr).text,"$R24");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R24;
						}break;
						default:
						{
							strcpy((*tptr).text,"$R2");
							(*tptr).type = TOK_INT_REG;
							(*tptr).val = $R2;
							goBackOneChar();
						}
					}/*end of $R2 sub-switch*/
				}break;
				case '3':
				{
					strcpy((*tptr).text,"$R3");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R3;
				}break;
				case '4':
				{
					strcpy((*tptr).text,"$R4");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R4;
				}break;
				case '5':
				{
					strcpy((*tptr).text,"$R5");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R5;
				}break;
				case '6':
				{
					strcpy((*tptr).text,"$R6");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R6;
				}break;
				case '7':
				{
					strcpy((*tptr).text,"$R7");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R7;
				}break;
				case '8':
				{
					strcpy((*tptr).text,"$R8");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R8;
				}break;
				case '9':
				{
					strcpy((*tptr).text,"$R9");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $R9;
				}break;
				default: /*does not start with 0-9*/
				{
					(*tptr).text[0]='$';
					(*tptr).text[1]='R';
					(*tptr).text[2]=current;
					(*tptr).text[3]='\0';
					(*tptr).type = TOK_BAD; 
				}
			}/*end $R switch*/
		}break;
		/*FLT_REGISTER-----------------------------------------*/
		case 'F':
		case 'f':
		{
			current = getNextLineChar();

			switch(current)
			{
				case '1':
				{
					current = getNextLineChar();

					if(current=='0')
					{
						strcpy((*tptr).text,"$F10");
						(*tptr).type = TOK_FLT_REG;
						(*tptr).val = $F10;
					}
					else
					{
						strcpy((*tptr).text,"$F1");
						(*tptr).type = TOK_FLT_REG;
						(*tptr).val = $F1;
						goBackOneChar();
					}
				}break;
				case '2':
				{
					strcpy((*tptr).text,"$F2");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F2;
				}break;
				case '3':
				{
					strcpy((*tptr).text,"$F3");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F3;
				}break;
				case '4':
				{
					strcpy((*tptr).text,"$F4");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F4;
				}break;
				case '5':
				{
					strcpy((*tptr).text,"$F5");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F5;
				}break;
				case '6':
				{
					strcpy((*tptr).text,"$F6");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F6;
				}break;
				case '7':
				{
					strcpy((*tptr).text,"$F7");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F7;
				}break;
				case '8':
				{
					strcpy((*tptr).text,"$F8");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F8;
				}break;
				case '9':
				{
					strcpy((*tptr).text,"$F9");
					(*tptr).type = TOK_FLT_REG;
					(*tptr).val = $F9;
				}break;
				case 'P':
				case 'p':
				{
					strcpy((*tptr).text,"$FP");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $FP;
				}break;
				default: /* not 0-9*/
				{
					(*tptr).text[0]='$';
					(*tptr).text[1]='F';
					(*tptr).text[2]=current;
					(*tptr).text[3]='\0';
					(*tptr).type = TOK_BAD;
				}
			}/*end of $F switch*/
		}break;
		/*DBL_REGISTER-----------------------------------------*/
		case 'D':
		case 'd':
		{
			current = getNextLineChar();

			switch(current)
			{
				case '1':
				{
					current = getNextLineChar();

					if(current=='0')
					{
						strcpy((*tptr).text,"$D10");
						(*tptr).type = TOK_DBL_REG;
						(*tptr).val = $D10;
					}
					else
					{
						strcpy((*tptr).text,"$D1");
						(*tptr).type = TOK_DBL_REG;
						(*tptr).val = $D1;
						goBackOneChar();
					}
				}break;
				case '2':
				{
					strcpy((*tptr).text,"$D2");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D2;
				}break;
				case '3':
				{
					strcpy((*tptr).text,"$D3");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D3;
				}break;
				case '4':
				{
					strcpy((*tptr).text,"$D4");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D4;
				}break;
				case '5':
				{
					strcpy((*tptr).text,"$D5");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D5;
				}break;
				case '6':
				{
					strcpy((*tptr).text,"$D6");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D6;
				}break;
				case '7':
				{
					strcpy((*tptr).text,"$D7");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D7;
				}break;
				case '8':
				{
					strcpy((*tptr).text,"$D8");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D8;
				}break;
				case '9':
				{
					strcpy((*tptr).text,"$D9");
					(*tptr).type = TOK_DBL_REG;
					(*tptr).val = $D9;
				}break;
				default: /* not 0-9*/
				{
					(*tptr).text[0]='$';
					(*tptr).text[1]='D';
					(*tptr).text[2]=current;
					(*tptr).text[3]='\0';
					(*tptr).type = TOK_BAD;
				}
			}/*end of $D switch*/

		}break;
		/*end case D*/
		case 'I':
		case 'i':
		{
			current = getNextLineChar();
			if((current=='P')||(current=='p'))
			{
				strcpy((*tptr).text,"$IP");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $IP;
			}
			else
			{
				(*tptr).text[0]='$';
				(*tptr).text[1]='I';
				(*tptr).text[2]=current;
				(*tptr).text[3]='\0';
				(*tptr).type = TOK_BAD;
			}

		}break;
		case 'B':
		case 'b':
		{
			current = getNextLineChar();
			if((current=='E')||(current=='e'))
			{
				strcpy((*tptr).text,"$BE");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $BE;
			}
			else
			{
				(*tptr).text[0]='$';
				(*tptr).text[1]='B';
				(*tptr).text[2]=current;
				(*tptr).text[3]='\0';
				(*tptr).type = TOK_BAD;
			}

		}break;
		case 'S':
		case 's':
		{
			current = getNextLineChar();
			if((current=='P')||(current=='p'))
			{
				strcpy((*tptr).text,"$SP");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $SP;
			}
			else if((current=='S')||(current=='s'))
			{
				strcpy((*tptr).text,"$SS");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $SS;
			}
			else
			{
				(*tptr).text[0]='$';
				(*tptr).text[1]='S';
				(*tptr).text[2]=current;
				(*tptr).text[3]='\0';
				(*tptr).type = TOK_BAD;
			}

		}break;
		case 'H':
		case 'h':
		{
			current = getNextLineChar();
			if((current=='S')||(current=='s'))
			{
				strcpy((*tptr).text,"$HS");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $HS;
			}
			else if((current=='E')||(current=='e'))
			{
				strcpy((*tptr).text,"$HE");
				(*tptr).type = TOK_INT_REG;
				(*tptr).val = $HE;
			}
			else
			{
				(*tptr).text[0]='$';
				(*tptr).text[1]='H';
				(*tptr).text[2]=current;
				(*tptr).text[3]='\0';
				(*tptr).type = TOK_BAD;
			}

		}break;
		case 'T':
		case 't':
		{
			current = getNextLineChar();
			if((current=='o')||(current=='O'))
			{
				current = getNextLineChar();
				if((current=='P')||(current=='p'))
				{
					strcpy((*tptr).text,"$TOP");
					(*tptr).type = TOK_INT_REG;
					(*tptr).val = $TOP;
				}
				else
				{
					(*tptr).text[0]='$';
					(*tptr).text[1]='T';
					(*tptr).text[2]='O';
					(*tptr).text[3]=current;
					(*tptr).text[4]='\0';
					(*tptr).type = TOK_BAD;
				}
			}
			else
			{
				(*tptr).text[0]='$';
				(*tptr).text[1]='T';
				(*tptr).text[2]=current;
				(*tptr).text[3]='\0';
				(*tptr).type = TOK_BAD;
			}

		}break;
		default:  /*NOT $R_,$F_,$D_, or $IP, $SP, $FP, $BE, $HS, $HE, $SS, $TOP*/
		{
			(*tptr).text[0]='$';
			(*tptr).text[1]=current;
			(*tptr).text[2]='\0';
			(*tptr).type = TOK_BAD;
		}
	}/*end switch*/

	return;

}/*end processRegister*/

/*-----------------------------------------------------------------*/

void LineTokenizer::processCharConst(struct Token *tptr)
{
	char current;
	char ch;
	ch = current = getNextLineChar();
	if((current>=32)&&(current<=126))
	{
		(*tptr).text[0]=current;
		(*tptr).text[1]='\0';
		(*tptr).val=current;
		(*tptr).type = TOK_CHAR_CONST;
		current = getNextLineChar();
		if(current!='\'')
		{
			(*tptr).text[0]='\'';
			(*tptr).text[1]=ch;
			(*tptr).text[2]=current;
			(*tptr).text[3]='\0';
			(*tptr).type = TOK_BAD; 
		}
	}
	else
	{
			(*tptr).text[0]='\'';
			(*tptr).text[1]=current;
			(*tptr).text[2]='\0';
			(*tptr).type = TOK_BAD; 
	}
	return;

}/*end processCharConstant*/

/*-----------------------------------------------------------------*/

void LineTokenizer::processIdentifier(struct Token *tptr, char ch)
{
	int i;
	char current;

	i=0;
	(*tptr).text[i]=ch;
	current = getNextLineChar();
	while(((current>='a')&&(current<='z'))||
		  ((current>='A')&&(current<='Z'))||
		  ((current>='0')&&(current<='9'))||
		  (current=='@')||
		  (current=='_')||
		  (current=='?')||
		  (current=='.'))
	{
		i++;
		(*tptr).text[i]=current;
		current = getNextLineChar();
	}
	i++;
	(*tptr).text[i]='\0';
	(*tptr).type=TOK_IDENTIFIER;
	goBackOneChar();
	return;

}/*end processIdentifier*/

/*-----------------------------------------------------------------*/

void LineTokenizer::processNumConst(struct Token *tptr, char ch)
{
	int i;
	char current;
	
	i=0;
	current = ch;

	if((current=='+')||(current=='-'))
	{
		(*tptr).text[i]=current;
		i++;
		current = getNextLineChar();
	}

	/* float/integer starts with 0-9 */

	if((current>='0')&&(current<='9')) 
	{
		/* if 0 is first digit, must be followed by decimal place*/

		if(current=='0')
		{
			current = getNextLineChar();
			if(current =='.')
			{ 
				current = '0';
				goBackOneChar(); 
			}
			else
			{
				(*tptr).text[i]=current;
				(*tptr).text[i+1]='\0';
				(*tptr).type = TOK_INT_CONST;
				return;
			}
		}

		while((current>='0')&&(current<='9'))
		{
			(*tptr).text[i]=current;
			i++;
			current = getNextLineChar();
		}

		/* digits end in decimal point*/

		if(current=='.')
		{
			(*tptr).text[i]='.';
			i++;
			current = getNextLineChar();
				
			if((current>='0')&&(current<='9'))
			{
				while((current>='0')&&(current<='9'))
				{
					(*tptr).text[i]=current;
					i++;
					current = getNextLineChar();
				}
			}
			else /* no digits following decimal point, required */
			{
				(*tptr).text[i]='\0';
				(*tptr).type = TOK_BAD;
				goBackOneChar();
				return;
			}

			if((current=='e')||(current=='E'))
			{
				(*tptr).text[i]=current;
				i++;
				current = getNextLineChar();

				if((current=='+')||(current=='-'))
				{
					(*tptr).text[i]=current;
					i++;
					current = getNextLineChar();
				}

				if((current>='0')&&(current<='9'))
				{
					while((current>='0')&&(current<='9'))
					{
						(*tptr).text[i]=current;
						i++;
						current = getNextLineChar();
					}

					(*tptr).text[i]='\0';
					(*tptr).type=TOK_FLT_CONST;
					(*tptr).fval = atof((*tptr).text);
					goBackOneChar();
					return;
				}
				else /*no digits after +/- */
				{
					(*tptr).text[i]='\0';
					(*tptr).type = TOK_BAD;
					goBackOneChar();
					return;
				}

			}
			else /*no e/E following .digits */
			{
				(*tptr).text[i]='\0';
				(*tptr).type=TOK_FLT_CONST;
				(*tptr).fval = atof((*tptr).text);
				goBackOneChar();
				return;
			}
		}
		
		/*digits end in e/E potential float*/
		
		else if((current=='e')||(current=='E')) 
		{
			(*tptr).text[i]=current;
			i++;
			current = getNextLineChar();

			if((current=='+')||(current=='-'))
			{
				(*tptr).text[i]=current;
				i++;
				current = getNextLineChar();
			}

			if((current>='0')&&(current<='9'))
			{
				while((current>='0')&&(current<='9'))
				{
					(*tptr).text[i]=current;
					i++;
					current = getNextLineChar();
				}
					
				(*tptr).text[i]='\0';
				(*tptr).type=TOK_FLT_CONST;
				(*tptr).fval = atof((*tptr).text);
				goBackOneChar();
				return;
			}
			else /*no digits after +/- */
			{
				(*tptr).text[i]='\0';
				(*tptr).type = TOK_BAD;
				goBackOneChar();
				return;
			}
		}
		
		/*digits do not end in decimal point or e/E, have integer*/
		
		else 
		{
			(*tptr).text[i]='\0';
			(*tptr).type = TOK_INT_CONST;
			(*tptr).val = stringToU8((*tptr).text);
			goBackOneChar();
			return;
		}
	}
	
	/*does start with 0-9  after +/- */

	else
	{
		(*tptr).text[0]='\0';
		(*tptr).type = TOK_BAD;
		goBackOneChar();
	}
	return;

}/*end processNumConstant*/

/*-----------------------------------------------------------------*/

U1 LineTokenizer::match(struct Token *tptr, int ttype)
{
	(*tptr) = getNextLineToken();
	if((*tptr).type!=ttype)
	{
		ERROR5("LineTokenizer::match(): %s:(%lu) expecting %s, not %s in \"%s\"\n",(*tptr).fName,(*tptr).line,TokStr[ttype],TokStr[(*tptr).type],(*tptr).text);
		return(FALSE);
	}
	LINE_TOK_DEBUG2("LineTokenizer::match(): OK text=%s type=%s\n",(*tptr).text,TokStr[(*tptr).type]);

	return(TRUE);

}/*end match*/

/*-----------------------------------------------------------------*/

void LineTokenizer::printToken(struct Token *tptr)
{
	printf("TOK-> file=%s line=%d type=%s ",(*tptr).fName,(*tptr).line,TokStr[(*tptr).type]);
	switch((*tptr).type)
	{
		case TOK_IDENTIFIER:{ printf("text=%s\n",(*tptr).text); }break;
		case TOK_INT_REG:{ printf("text=%s val=%d\n",(*tptr).text,(*tptr).val); }break;
		case TOK_FLT_REG:{ printf("text=%s val=%d\n",(*tptr).text,(*tptr).val); }break;
		case TOK_DBL_REG:{ printf("text=%s val=%d\n",(*tptr).text,(*tptr).val); }break;
		case TOK_CHAR_CONST:{ printf("text=%s\n",(*tptr).text); }break;
		case TOK_INT_CONST:
		{ 
			printf("text=%s val=",(*tptr).text); 
			pS8((*tptr).val);
			printf("\n");
		}break;
		case TOK_FLT_CONST:{ printf("text=%s fval=%g\n",(*tptr).text,(*tptr).fval); }break;
		case TOK_COMMA:{ printf("text=%s\n",(*tptr).text);}break;
		case TOK_NO_MORE:{ printf("text=%s\n",(*tptr).text);}break;
		case TOK_BAD:{ printf("text=%s\n",(*tptr).text);}break;
	}
	return;

}/*end printToken*/
