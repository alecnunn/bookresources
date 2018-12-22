/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ pass2.c -	build listing file and                                  +
+           generate bytecode into temporary file					+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define PASS2_DEBUG		1*/

#ifdef	PASS2_DEBUG
#define	PASS2_DEBUG0(arg);						printf(arg);
#define PASS2_DEBUG1(arg1,arg2);				printf(arg1,arg2);
#define PASS2_DEBUG2(arg1,arg2,arg3);			printf(arg1,arg2,arg3);
#else
#define	PASS2_DEBUG0(arg);
#define PASS2_DEBUG1(arg1,arg2);
#define PASS2_DEBUG2(arg1,arg2,arg3);
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class Pass2
{
	U4 bytePosPass2;		/*current index of bytecode being generated*/

	FILE *lfptr;			/*pointer to list file*/
	FILE *tfptr;			/*pointer to temporary bytecode file*/

	char *lfile;			/*pointer to list file name*/
	char *tfile;			/*pointer to temporary bytecode file name*/

	char lfBuffer[BUFFER_SIZE];   /*output buffer for list file*/
	char tfBuffer[BUFFER_SIZE];	  /*output buffer for temp file*/

	int iLFChar;	/*index into lfBuffer*/
	int iTFChar;	/*index into tfBuffer*/

	CommandLine *cmdLine;
	StringTable *strTbl;
	SymbolTable *symTbl;
	HashTable *hashTbl;
	LineTokenizer *toker;

	char listing[LINE_SIZE];	/*line of text file to place in listing file*/	
	char lineNumber[32];		/*line number to place in listing file*/
	U1 encoded[11];				/*holds bytes to write to temp file*/

	void putByteTempBuff(U1 byte);
	void flushTempBuff();

	void putStrLstBuff(char *str);
	void putByteLstBuff(U1 byte);
	void flushLstBuff();

	void  processDirective(struct Token *tptr, struct Line *line);
	void  processGDirective(struct Token *tptr, struct Line *line);
	void  processPDirective(struct Token *tptr, struct Line *line);

	void  processInstruction(struct Token *tptr);

	/*print symbol table, called by generateSymbolSummary()*/
	void printGlobVarToLst(struct GlobalVariable *ptr); 
	void printProcToLst(struct Procedure *ptr);
	void printTreeToLst(struct HashTbl* link, int level);

	/*
	Instruction handling
		I = instruction opcode
		B = byte constant
		R, F, D = registers ( int, float, double )
		C = constant, varies in size
		A = identifier -> resolved to 'A'ddress
	*/

	void I(U1 opcode, struct Token *tptr);
	
	void IB(U1 opcode, struct Token *tptr);
	void IRC(U1 opcode, U1 bytes, struct Token *tptr);
	void IRA(struct Token *tptr);
	void I2RA(struct Token *tptr);

	void IR(U1 opcode, struct Token *tptr);
	void I2R(U1 opcode,struct Token *tptr);
	void I3R(U1 opcode, struct Token *tptr);
	void I4R(U1 opcode, struct Token *tptr);

	void IRF(U1 opcode, struct Token *tptr);
	void IRD(U1 opcode, struct Token *tptr);

	void IFC(U1 opcode, struct Token *tptr);
	void IF(U1 opcode, struct Token *tptr);
	void I2F(U1 opcode, struct Token *tptr);
	void I3F(U1 opcode, struct Token *tptr);
	void IFR(U1 opcode, struct Token *tptr);
	void IFD(U1 opcode, struct Token *tptr);

	void IDC(U1 opcode, struct Token *tptr);
	void ID(U1 opcode, struct Token *tptr);
	void I2D(U1 opcode, struct Token *tptr);
	void I3D(U1 opcode, struct Token *tptr);
	void IDR(U1 opcode, struct Token *tptr);
	void IDF(U1 opcode, struct Token *tptr);

	void commitToFiles(U1 len);

	public:
	Pass2(CommandLine *cptr, StringTable *st, SymbolTable *syt, HashTable *ht);
	~Pass2();

	void parseLine(struct Line *line);
	void generateSymbolSummary();
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

Pass2::Pass2(CommandLine *cptr, StringTable *st, SymbolTable *syt, HashTable *ht)
{ 
	cmdLine = cptr;
	strTbl = st;
	symTbl = syt;
	hashTbl = ht;

	lfile = (*cptr).listFile;
	tfile = (*cptr).tempFile;

	if((*cmdLine).listing==TRUE)
	{
		if(lfile==NULL)
		{
			ERROR0("Pass2::Pass2(): listing file name has not been specified\n");
			FATAL_ERROR();
		}
		lfptr = fopen(lfile,"wb");
		if(lfptr==NULL)
		{
			ERROR0("Pass2::Pass2(): could not open listing file\n");
			FATAL_ERROR();
		}
	}

	if(tfile==NULL)
	{
		ERROR0("Pass2::Pass2(): temporary file's name is NULL\n");
		FATAL_ERROR();
	}
	tfptr = fopen(tfile,"wb");
	if(tfptr==NULL)
	{
		ERROR0("Pass2::Pass2(): could not open temporary file\n");
		FATAL_ERROR();
	}
	bytePosPass2 = 0;
	iLFChar = 0;
	iTFChar = 0;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

Pass2::~Pass2()
{
	if((*cmdLine).listing==TRUE)
	{
		flushLstBuff();
		if(fclose(lfptr)){ ERROR1("Pass2::~Pass2(): problem closing listing file\n",lfile); }
	}

	flushTempBuff();
	if(fclose(tfptr)){ ERROR1("Pass2::~Pass2(): problem closing temp file\n",tfile); }
	return;

}/*end destructor*/

/*-----------------------------------------------------------------*/

void Pass2::putByteTempBuff(U1 byte)
{
	int nbytes;
	nbytes = 0;

	tfBuffer[iTFChar] = byte;
	iTFChar++;

	if(iTFChar==BUFFER_SIZE)
	{
		nbytes = fwrite(tfBuffer,sizeof(U1),BUFFER_SIZE,tfptr);
		if(nbytes!=BUFFER_SIZE)
		{
			ERROR1("Pass2::putByteTempBuff(): error during fwrite to %s\n",tfile);
			FATAL_ERROR();
		}
		iTFChar = 0;
	}
	return;

}/*end putByteTempBuff*/

/*-----------------------------------------------------------------*/

void Pass2::flushTempBuff()
{
	int nbytes;
	nbytes = 0;

	if(iTFChar>0)
	{
		PASS2_DEBUG1("Pass2::flushTempBuff(): trying to flush %lu bytes\n",iTFChar);
		nbytes = fwrite(tfBuffer,sizeof(U1),iTFChar,tfptr);
		if(nbytes!=iTFChar)
		{
			ERROR1("Pass2::flushTempBuff(): only flushed %lu bytes\n",nbytes);
			ERROR1("Pass2::flushTempBuff(): error during flush to %s\n",tfile);
			FATAL_ERROR();
		}
		iTFChar = 0;	
	}
	return;

}/*end flushTempBuff*/

/*-----------------------------------------------------------------*/

void Pass2::putStrLstBuff(char *str)
{
	U4 i;
	U4 j;
	U4 size;
	size = strlen(str);
	j=0;
	while((str[j]==' ')||(str[j]=='\t')){ j++; }
	for(i=j;i<size;i++){ putByteLstBuff(str[i]); }
	return;

}/*end putStrLstBuff*/

/*-----------------------------------------------------------------*/

void Pass2::putByteLstBuff(U1 byte)
{
	int nbytes;

	nbytes = 0;

	lfBuffer[iLFChar] = byte;
	iLFChar++;

	if(iLFChar==BUFFER_SIZE)
	{
		nbytes = fwrite(lfBuffer,sizeof(U1),BUFFER_SIZE,lfptr);
		if(nbytes!=BUFFER_SIZE)
		{
			ERROR1("Pass2::putByteLstBuff(): only flushed %lu bytes\n",nbytes);
			ERROR1("Pass2::putByteLstBuff(): error during fwrite to %s\n",lfile);
			FATAL_ERROR();
		}
		iLFChar = 0;
	}
	return;

}/*end putByteLstBuff*/

/*-----------------------------------------------------------------*/

void Pass2::flushLstBuff()
{
	int nbytes;

	nbytes = 0;

	if(iLFChar>0)
	{
		PASS2_DEBUG1("Pass2::flushLstBuff(): trying to flush %lu bytes\n",iLFChar);
		nbytes = fwrite(lfBuffer,sizeof(U1),iLFChar,lfptr);
		if(nbytes!=iLFChar)
		{
			ERROR1("Pass2::flushLstBuff(): only flushed %lu bytes\n",nbytes);
			ERROR1("Pass2::flushLstBuff(): error during fwrite to %s\n",lfile);
			FATAL_ERROR();
		}
		iLFChar = 0;	
	}
	return;

}/*end flushLstBuff*/

/*-----------------------------------------------------------------*/

void Pass2::parseLine(struct Line *line)
{
	struct Token token;
	PASS2_DEBUG2("Pass2::parseLinePass2(): line->(%d)\t text->(%s)\n",(*line).line,(*line).src);

	/*
	line can be
		i) a directive ( starts with a period, i.e.  .IP )
		ii) an instruction ( opcode operand(s) )
		iii) a comment ( starts with # )
	*/
	
	LineTokenizer t(line);
	toker = &t;

	token = (*toker).getNextLineToken();

	if(token.type==TOK_NO_MORE){ return; }

	switch(token.text[0])
	{
		case '.':
		{ 
			if((*cmdLine).listing==TRUE){ processDirective(&token,line); } 
		}break;
		case '#':
		{ 
			/*ignore line*/ 
		}break;
		default:{ processInstruction(&token); }
	}
	return;

}/*end parseLine*/

/*-----------------------------------------------------------------*/

/*
	Directives:
	.IP identifier							import procedure
	.IG identifier							import global variable
	.GB identifier  [ integer ] [ EXPORT ]	define global variable
	( same for .GW, .GD, .GQ )
	.PB	identifier [ EXPORT ]				procedure begin
		.PR identifier +n					procedure return value
		.PA identifier +n					procedure argument
		.PV identifier -n					procedure local variable
		.PL identifier						procedure label
	.PE
*/

void  Pass2::processDirective(struct Token *tptr, struct Line *line)
{
	if((*tptr).type==TOK_IDENTIFIER)
	{
		switch((*tptr).text[1])
		{
			case 'G':{ processGDirective(tptr,line); }break;
			case 'P':{ processPDirective(tptr,line); }break;
			default:
			{
				ERROR1("Pass2::processDirective(): %s not a directive\n",(*tptr).text);
			}
		}
	}
	else
	{
		ERROR1("Pass2::processDirective(): %s not a directive\n",(*tptr).text);
	}
	return;

}/*end processDirective*/

/*-----------------------------------------------------------------*/

/*
.GB identifier  [ integer ] [ EXPORT ]	define global variable
	( same for .GW, .GD, .GQ )
*/

void  Pass2::processGDirective(struct Token *tptr, struct Line *line)
{
	sprintf(lineNumber,"%lu",(*tptr).line);
	putStrLstBuff(lineNumber);
	putByteLstBuff(')');

	putStrLstBuff((*line).src);
	putByteLstBuff('\n');

	return;

}/*end processGDirective*/

/*-----------------------------------------------------------------*/

/*
	.PB	identifier [ EXPORT ]				procedure begin
		.PR identifier +n					procedure return value
		.PA identifier +n					procedure argument
		.PV identifier -n					procedure local variable
		.PL identifier						procedure label
	.PE
*/

void  Pass2::processPDirective(struct Token *tptr, struct Line *line)
{
	if(strcmp((*tptr).text,".PB")==0)/*.PB	identifier [ EXPORT ]*/
	{
		putStrLstBuff("#++++++++++++++++++++++++++++++++++++++++\n");

		sprintf(lineNumber,"%lu",(*tptr).line);
		putStrLstBuff(lineNumber);
		putByteLstBuff(')');

		putStrLstBuff((*line).src);
		putByteLstBuff('\n');

	}/*end .PB*/
	else 
	{
		sprintf(lineNumber,"%lu",(*tptr).line);
		putStrLstBuff(lineNumber);
		putByteLstBuff(')');
		putByteLstBuff('\t');

		putStrLstBuff((*line).src);
		putByteLstBuff('\n');	
	}
	return;

}/*end processPDirective*/

/*-----------------------------------------------------------------*/

/*
Do not generate bytecode in first pass, but we need to know
	-offset of label directives
	-size of functions
Thus, we need to do minimal instruction processing to keep
track of bytesize via ->bytePos<- variable

use switch to help cut down on calls to strcmp()

can probably recycle basic skeleton and extend on this code for Pass2

Note: have 68 instructions total

Basic goal in each case is to populate 1/2 arrays and write them to file
	char listing[LINE_SIZE]; -> to listing file	
	U1 encoded[11];		-> to bytecode temp file
*/

void  Pass2::processInstruction(struct Token *tptr)
{
	if((*tptr).type==TOK_IDENTIFIER)
	{
		switch((*tptr).text[0])
		{
			case 'A':
			{
				if(strcmp((*tptr).text,"ADD")==0){ I3R(ADD,tptr); }
				else if(strcmp((*tptr).text,"AND")==0){ I3R(AND,tptr); }
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'B':
			{
				if(strcmp((*tptr).text,"BS")==0){ I2R(BS,tptr); }
				else if(strcmp((*tptr).text,"BT")==0){ I3R(BT,tptr); }
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'C':
			{
				if(strcmp((*tptr).text,"CAST_IF")==0){ IRF(CAST_IF,tptr);}
				else if(strcmp((*tptr).text,"CAST_ID")==0){ IRD(CAST_ID,tptr);}
				else if(strcmp((*tptr).text,"CAST_FI")==0){ IFR(CAST_FI,tptr);}
				else if(strcmp((*tptr).text,"CAST_FD")==0){ IFD(CAST_FD,tptr);}
				else if(strcmp((*tptr).text,"CAST_DI")==0){ IDR(CAST_DI,tptr);}
				else if(strcmp((*tptr).text,"CAST_DF")==0){ IDF(CAST_DF,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'D':
			{
				if(strcmp((*tptr).text,"DIV")==0){ I4R(DIV,tptr);}
				else if(strcmp((*tptr).text,"DI")==0){ I(DI,tptr);}
				else if(strcmp((*tptr).text,"DADD")==0){ I3D(DADD,tptr);}
				else if(strcmp((*tptr).text,"DSUB")==0){ I3D(DSUB,tptr);}
				else if(strcmp((*tptr).text,"DMULT")==0){ I3D(DMULT,tptr);}
				else if(strcmp((*tptr).text,"DDIV")==0){ I3D(DDIV,tptr);}
				else if(strcmp((*tptr).text,"DSLT")==0){ I3D(DSLT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'E':
			{
				if(strcmp((*tptr).text,"EI")==0){ I(EI,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'F':
			{
				if(strcmp((*tptr).text,"FADD")==0){ I3F(FADD,tptr);}
				else if(strcmp((*tptr).text,"FSUB")==0){ I3F(FSUB,tptr);}
				else if(strcmp((*tptr).text,"FMULT")==0){ I3F(FMULT,tptr);}
				else if(strcmp((*tptr).text,"FDIV")==0){ I3F(FDIV,tptr);}
				else if(strcmp((*tptr).text,"FSLT")==0){ I3F(FSLT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'H':
			{
				if(strcmp((*tptr).text,"HALT")==0){ I(HALT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'I':
			{
				if(strcmp((*tptr).text,"INT")==0){ IB(INT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'J':
			{
				if(strcmp((*tptr).text,"JMP")==0){ IR(JMP,tptr);}
				else if(strcmp((*tptr).text,"JE")==0){ I3R(JE,tptr);}
				else if(strcmp((*tptr).text,"JNE")==0){ I3R(JNE,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'L':
			{
				if(strcmp((*tptr).text,"LBI")==0){ IRC(LBI,1,tptr);}
				else if(strcmp((*tptr).text,"LWI")==0){ IRC(LWI,2,tptr);}
				else if(strcmp((*tptr).text,"LDI")==0){ IRC(LDI,4,tptr);}
				else if(strcmp((*tptr).text,"LQI")==0){ IRC(LQI,8,tptr);}
				else if(strcmp((*tptr).text,"LF1I")==0){ IFC(LF1I,tptr);}
				else if(strcmp((*tptr).text,"LF2I")==0){ IDC(LF2I,tptr);}
				else if(strcmp((*tptr).text,"LB")==0){ I2R(LB,tptr);}
				else if(strcmp((*tptr).text,"LW")==0){ I2R(LW,tptr);}
				else if(strcmp((*tptr).text,"LD")==0){ I2R(LD,tptr);}
				else if(strcmp((*tptr).text,"LQ")==0){ I2R(LQ,tptr);}
				else if(strcmp((*tptr).text,"LF1")==0){ IFR(LF1,tptr);}
				else if(strcmp((*tptr).text,"LF2")==0){ IDR(LF2,tptr);}
				else if(strcmp((*tptr).text,"LAD")==0){ IRA(tptr);}
				else if(strcmp((*tptr).text,"LAI")==0){ I2RA(tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'M':
			{
				if(strcmp((*tptr).text,"MOV")==0){ I2R(MOV,tptr); }
				else if(strcmp((*tptr).text,"MOVF")==0){ I2F(MOVF,tptr); }
				else if(strcmp((*tptr).text,"MOVD")==0){ I2D(MOVD,tptr); }
				else if(strcmp((*tptr).text,"MULT")==0){ I3R(MULT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'N':
			{
				if(strcmp((*tptr).text,"NOT")==0){ I2R(NOT,tptr);}
				else if(strcmp((*tptr).text,"NOP")==0){ I(NOP,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'O':
			{
				if(strcmp((*tptr).text,"OR")==0){ I3R(OR,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'P':
			{
				if(strcmp((*tptr).text,"PUSHB")==0){ IR(PUSHB,tptr);}
				else if(strcmp((*tptr).text,"PUSHW")==0){ IR(PUSHW,tptr);}
				else if(strcmp((*tptr).text,"PUSHD")==0){ IR(PUSHD,tptr);}
				else if(strcmp((*tptr).text,"PUSHQ")==0){ IR(PUSHQ,tptr);}
				else if(strcmp((*tptr).text,"PUSHF1")==0){ IF(PUSHF1,tptr);}
				else if(strcmp((*tptr).text,"PUSHF2")==0){ ID(PUSHF2,tptr);}
				else if(strcmp((*tptr).text,"POPB")==0){ IR(POPB,tptr);}
				else if(strcmp((*tptr).text,"POPW")==0){ IR(POPW,tptr);}
				else if(strcmp((*tptr).text,"POPD")==0){ IR(POPD,tptr);}
				else if(strcmp((*tptr).text,"POPQ")==0){ IR(POPQ,tptr);}
				else if(strcmp((*tptr).text,"POPF1")==0){ IF(POPF1,tptr);}
				else if(strcmp((*tptr).text,"POPF2")==0){ ID(POPF2,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'S':
			{
				if(strcmp((*tptr).text,"SB")==0){ I2R(SB,tptr);}
				else if(strcmp((*tptr).text,"SW")==0){ I2R(SW,tptr);}
				else if(strcmp((*tptr).text,"SD")==0){ I2R(SD,tptr);}
				else if(strcmp((*tptr).text,"SQ")==0){ I2R(SQ,tptr);}
				else if(strcmp((*tptr).text,"SF1")==0){ IFR(SF1,tptr);}
				else if(strcmp((*tptr).text,"SF2")==0){ IDR(SF2,tptr);}
				else if(strcmp((*tptr).text,"SRA")==0){ I3R(SRA,tptr);}
				else if(strcmp((*tptr).text,"SRL")==0){ I3R(SRL,tptr);}
				else if(strcmp((*tptr).text,"SL")==0){ I3R(SL,tptr);}
				else if(strcmp((*tptr).text,"SUB")==0){ I3R(SUB,tptr);}
				else if(strcmp((*tptr).text,"SLT")==0){ I3R(SLT,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			case 'X':
			{
				if(strcmp((*tptr).text,"XOR")==0){ I3R(XOR,tptr);}
				else
				{
					ERROR2("processInstructionPass1(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
					return;
				}

			}break;
			default:
			{
				ERROR3("Pass2::processInstruction(): %s on line %lu type %s not a valid opcode\n",(*tptr).text,(*tptr).line,TokStr[(*tptr).type]);
				return;
			}
		}/*end switch*/
	}
	else
	{
		ERROR3("Pass2::processInstruction(): %s on line %lu type %s not a valid opcode\n",(*tptr).text,(*tptr).line,TokStr[(*tptr).type]);
	}
	return;

}/*end processInstruction*/

/*------------------------------------------------------------------*/

void Pass2::I(U1 opcode, struct Token *tptr) /* Instruction */
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 1;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE)
	{ 
		return; 
	}

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I*/

/*------------------------------------------------------------------*/

void Pass2::IB(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 2; 

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_CONST);
	if(bret==TRUE)
	{
		char number[8];
		sprintf(number,"%d",(U1)t.val);
		strcat(listing,number);
		encoded[1]=(U1)t.val;
	}
	else
	{ 
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE)
	{ 
		return; 
	}

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IB*/

/*-----------------------------------------------------------------*/

void Pass2::IR(U1 opcode, struct Token *tptr) /* Instruction IntegerRegister */
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 2;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else
	{ 
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE)
	{ 
		return; 
	}

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IR*/

/*------------------------------------------------------------------*/

/* Instruction IntegerRegister Constant */

void Pass2::IRC(U1 opcode, U1 bytes, struct Token *tptr) 
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 2+bytes; /*bytes = 1,2,4,8 ( byte->qword )*/

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else
	{ 
		return; 
	}

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else
	{ 
		return; 
	}

	t = (*toker).getNextLineToken();
	if(t.type==TOK_INT_CONST)
	{
		strcat(listing,t.text);
		switch(bytes)
		{
			case 1:
			{ 
				encoded[2] = (U1)t.val; 
			}break;
			case 2:
			{ 
				wordToBytecode((U2)t.val, &encoded[2]);
			}break;
			case 4:
			{ 
				dwordToBytecode((U4)t.val, &encoded[2]);
			}break;
			case 8:
			{ 
				qwordToBytecode(t.val, &encoded[2]);
			}break;
		}
	}
	else if(t.type==TOK_CHAR_CONST)
	{
		strcat(listing,t.text);
		encoded[2]=0;encoded[3]=0;encoded[4]=0;encoded[5]=0;
		encoded[6]=0;encoded[7]=0;encoded[8]=0;encoded[9]=0;
		
		/*encode in big-endian format*/

		switch(bytes)
		{
			case 1:{ encoded[2] = (U1)t.val; }break;
			case 2:{ encoded[3] = (U1)t.val; }break;
			case 4:{ encoded[5] = (U1)t.val; }break;
			case 8:{ encoded[9] = (U1)t.val; }break;
		}
	}
	else
	{ 
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE)
	{ 
		return; 
	}

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end IRC*/

/*------------------------------------------------------------------*/

/* Instruction IntegerRegisters identifier */

void Pass2::IRA(struct Token *tptr) 
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	struct HashTbl *hptr;

	nBYTES = 2+8; /*address = 8 bytes*/

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=LAD;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else
	{ 
		return; 
	}

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE)
	{ 
		strcat(listing,t.text); 
	}
	else
	{ 
		return; 
	}

	t = (*toker).getNextLineToken();
	if(t.type==TOK_IDENTIFIER)
	{
		strcat(listing,t.text);

		/*symbol must exist*/

		hptr =  (*hashTbl).queryHashTbl(t.text);
		if(hptr==NULL)
		{
			ERROR2("IRA(): line %lu, undefined identifier %s\n",t.line,t.text);
			return;
		}
		else
		{
			/*must be symbol type =  PROC, PROC_LBL */

			U8 val;
			if((*hptr).type==PROC)
			{
				/*resolve offset/address */
				val = ((*symTbl).proc[(*hptr).index]).address; 
				qwordToBytecode(val, &encoded[2]); 
			}
			else if((*hptr).type==PROC_LBL)
			{
				/*resolve offset/address */
				val = (((*symTbl).proc[(*hptr).index]).label[(*hptr).subIndex]).address;
				qwordToBytecode(val, &encoded[2]); 
			}
			else
			{
				ERROR2("IRA(): line %lu, invalid operand for LAD %s\n",t.line,t.text);
				return;
			}
		}
	}
	else
	{ 
		ERROR2("IRA(): line %lu, invalid constant %s\n",t.line,t.text);
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IRA*/

/*------------------------------------------------------------------*/

/* Instruction 2IntegerRegisters identifier */

void Pass2::I2RA(struct Token *tptr) 
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	struct HashTbl *hptr;
	nBYTES = 3+8; /*address = 8 bytes*/

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=LAI;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	t = (*toker).getNextLineToken();
	if(t.type==TOK_IDENTIFIER)
	{
		strcat(listing,t.text);

		/*symbol must exist*/

		hptr =  (*hashTbl).queryHashTbl(t.text);
		if(hptr==NULL)
		{
			ERROR2("I2RA(): line %lu, undefined identifier %s\n",t.line,t.text);
			return;
		}
		else
		{
			/*must be symbol type = GLOBAL_VAR, PROC_RET, PROC_ARG, PROC_LOC */

			S8 val;
			if((*hptr).type==GLOBAL_VAR)
			{
				/*resolve offset/address */
				val = -((S8)((*symTbl).globVar[(*hptr).index]).offset);
				qwordToBytecode(val, &encoded[3]); 
			}
			else if((*hptr).type==PROC_RET)
			{
				/*resolve offset/address */
				val = (((*symTbl).proc[(*hptr).index]).ret).fpOffset;
				qwordToBytecode(val, &encoded[3]); 
			}
			else if((*hptr).type==PROC_ARG)
			{
				/*resolve offset/address */
				val = (((*symTbl).proc[(*hptr).index]).arg[(*hptr).subIndex]).fpOffset;
				qwordToBytecode(val, &encoded[3]); 
			}
			else if((*hptr).type==PROC_LOC)
			{
				/*resolve offset/address */
				val = (((*symTbl).proc[(*hptr).index]).local[(*hptr).subIndex]).fpOffset;
				qwordToBytecode(val, &encoded[3]); 
			}
			else
			{
				ERROR2("I2RA(): line %lu, invalid operand for LAD %s\n",t.line,t.text);
				return;
			}
		}
	}
	else
	{ 
		ERROR2("I2RA(): line %lu, invalid constant %s\n",t.line,t.text);
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I2RA*/

/*------------------------------------------------------------------*/

void Pass2::I2R(U1 opcode, struct Token *tptr) /* Instruction 2IntegerRegisters */
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I2R*/

/*------------------------------------------------------------------*/

/* Instruction 3Integer Registers */

void Pass2::I3R(U1 opcode, struct Token *tptr) 
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 4;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }
	
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[3]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I3R*/

/*------------------------------------------------------------------*/

void Pass2::I4R(U1 opcode, struct Token *tptr) /* Instruction 4Integer Registers */
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 5;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }
	
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[3]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[4]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end I4R*/

/*-----------------------------------------------------------------*/

void Pass2::IFC(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 6; /* float = 4 bytes */

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	t = (*toker).getNextLineToken();
	if(t.type==TOK_FLT_CONST)
	{
		strcat(listing,t.text);
		floatToBytecode((F4)t.fval, &encoded[2]); 
	}
	else
	{ 
		ERROR2("IFC(): line %lu, expecting float constant %s\n",t.line,t.text);
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end IFC*/

/*-----------------------------------------------------------------*/

void Pass2::IFR(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end IFR*/

/*-----------------------------------------------------------------*/

void Pass2::IF(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 2;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IF*/

/*-----------------------------------------------------------------*/

void Pass2::I2F(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I2F*/

/*-----------------------------------------------------------------*/

void Pass2::I3F(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 4;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }
	
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[3]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end I3F*/

/*-----------------------------------------------------------------*/

void Pass2::IRF(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
	
	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IRF*/

/*-----------------------------------------------------------------*/

void Pass2::IDF(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end IDF*/

/*-----------------------------------------------------------------*/

void Pass2::IDC(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 10; /* double = 8 bytes */

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	t = (*toker).getNextLineToken();
	if(t.type==TOK_FLT_CONST)
	{
		strcat(listing,t.text);
		doubleToBytecode(t.fval, &encoded[2]); 
	}
	else
	{
		ERROR2("IDC(): line %lu, expecting double constant %s\n",t.line,t.text);
		return; 
	}

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IDC*/

/*-----------------------------------------------------------------*/

void Pass2::IDR(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end IDR*/

/*-----------------------------------------------------------------*/

void Pass2::ID(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 2;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end ID*/

/*-----------------------------------------------------------------*/

void Pass2::I2D(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end I2D*/


/*-----------------------------------------------------------------*/

void Pass2::I3D(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 4;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }
	
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }

	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[3]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;
	
}/*end I3D*/

/*-----------------------------------------------------------------*/

void Pass2::IRD(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_INT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IRD*/

/*-----------------------------------------------------------------*/

void Pass2::IFD(U1 opcode, struct Token *tptr)
{
	struct Token t;
	U1 bret;
	U1 nBYTES;
	nBYTES = 3;

	sprintf(lineNumber,"%lu",(*tptr).line);

	strcpy(listing,(*tptr).text);
	strcat(listing," ");
	encoded[0]=opcode;

	bret = (*toker).match(&t,TOK_FLT_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[1]=(U1)t.val;
	}
	else{ return; }
					
	bret = (*toker).match(&t,TOK_COMMA);
	if(bret==TRUE){ strcat(listing,t.text); }
	else{ return; }
					
	bret = (*toker).match(&t,TOK_DBL_REG);
	if(bret==TRUE)
	{
		strcat(listing,t.text);
		encoded[2]=(U1)t.val;
	}
	else{ return; }

	bret = (*toker).match(&t,TOK_NO_MORE);
	if(bret!=TRUE){ return; }

	commitToFiles(nBYTES);

	bytePosPass2 = bytePosPass2 + nBYTES;
	return;

}/*end IFD*/

/*-----------------------------------------------------------------*/

void Pass2::commitToFiles(U1 len)
{
	U4 i;

	/*populate list file*/

	if((*cmdLine).listing==TRUE)
	{
		/*line number*/

		putStrLstBuff(lineNumber);
		putByteLstBuff(')');

		/*address*/
		putByteLstBuff('[');
		sprintf(lineNumber,"%lu",bytePosPass2);
		putStrLstBuff(lineNumber);
		putByteLstBuff(']');

		/*instruction*/

		putByteLstBuff('\t'); 
		putByteLstBuff('\t'); 
		putStrLstBuff(listing);						
		i = strlen(listing);
		while(i<=30){ putByteLstBuff(' '); i++; }

		/*binary version*/

		putByteLstBuff('\t');
		for(i=0;i<len;i++)
		{
			putByteLstBuff('[');
			sprintf(lineNumber,"%lu",encoded[i]);
			putStrLstBuff(lineNumber);
			putByteLstBuff(']');
		}
		putByteLstBuff('\n');	
	}

	/*populate temp file*/

	for(i=0;i<len;i++){ putByteTempBuff(encoded[i]); }

	return;

}/*end commitToFiles*/

/*-----------------------------------------------------------------*/

void Pass2::generateSymbolSummary()
{
	U4 i;
	U8 nstr;

	/* 1) print out symbol table to listing file*/

	putByteLstBuff('\n');
	sprintf(listing,"%s","#++++SYMBOL TABLE++++++");
	putStrLstBuff(listing);
	putByteLstBuff('\n');

	for(i=0;i<(*symTbl).iGlobVar;i++)
	{
		sprintf(listing,"->GLOBAL VARIABLE %d)",i); 
		putStrLstBuff(listing);
		printGlobVarToLst(&((*symTbl).globVar[i]));
		putByteLstBuff('\n');
	}
	for(i=0;i<(*symTbl).iProc;i++)
	{
		sprintf(listing,"->PROCEDURE %d)",i);
		putStrLstBuff(listing);
		printProcToLst(&((*symTbl).proc[i]));
		putByteLstBuff('\n');
	}

	/* 2) print out hash table to listing file*/

	putByteLstBuff('\n');
	sprintf(listing,"%s","#++++HASH TABLE++++++");
	putStrLstBuff(listing);
	putByteLstBuff('\n');

	for(i=0;i<PRIME;i++)
	{ 
		if((*hashTbl).hashTbl[i].empty == FALSE)
		{
			sprintf(listing,"Hash Slot %d)\n",i);
			putStrLstBuff(listing);

			printTreeToLst(&((*hashTbl).hashTbl[i]), 0);
			putByteLstBuff('\n');
		}
	}

	/* 3) print out string table to listing file*/

	putByteLstBuff('\n');
	sprintf(listing,"%s","#++++STRING TABLE++++++");
	putStrLstBuff(listing);
	putByteLstBuff('\n');

	nstr = 1;

	for(i=0;i<(*strTbl).iStr;i++)
	{
		if(i==0)
		{
			putByteLstBuff('0');
			putByteLstBuff(')');
		}
		if((*strTbl).text[i]!='\0')
		{ 
			putByteLstBuff((*strTbl).text[i]); 
		}
		else if(i<((*strTbl).iStr-1))
		{ 
			putByteLstBuff('\n');
			sprintf(lineNumber,"%d)",nstr); 
			putStrLstBuff(lineNumber);
			nstr++; 
		}
		else if(i==((*strTbl).iStr-1)){ putByteLstBuff('\n');}
	}

	return;

}/*end generateSymbolSummary*/

/*-----------------------------------------------------------------*/

void Pass2::printGlobVarToLst(struct GlobalVariable *ptr)
{	
	sprintf(listing,"identifier=%s\n",&((*strTbl).text[(*ptr).text]));
	putStrLstBuff(listing);

	sprintf(listing,"data type=%s\n",globSz[(*ptr).dType]);
	putStrLstBuff(listing);

	sprintf(listing,"array length=%lu\n",(*ptr).len);
	putStrLstBuff(listing);

	sprintf(listing,"total size=%lu\n",(*ptr).size);
	putStrLstBuff(listing);

	sprintf(listing,"line=%lu\n",(*ptr).line);
	putStrLstBuff(listing);

	sprintf(listing,"offset_$TOP=%lu\n",(*ptr).offset);
	putStrLstBuff(listing);

	return;

}/*end printGlobVarToLst*/ 

/*-----------------------------------------------------------------*/

void Pass2::printProcToLst(struct Procedure *ptr)
{
	U2 i;

	sprintf(listing,"identifier=%s\n",&((*strTbl).text[(*ptr).text]));
	putStrLstBuff(listing);


	sprintf(listing,"address=%lu\n",(*ptr).address);
	putStrLstBuff(listing);

	sprintf(listing,"source file line=%lu\n",(*ptr).line);
	putStrLstBuff(listing);

	if((*ptr).nRet)
	{
		
		sprintf(listing,"RET\n");
		putStrLstBuff(listing);
		
		putByteLstBuff('\t');
		sprintf(listing,"identifier=%s\n",&((*strTbl).text[((*ptr).ret).text]));
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"fpOffset=%d\n",((*ptr).ret).fpOffset);
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"line=%lu\n",((*ptr).ret).line);
		putStrLstBuff(listing);

	}

	sprintf(listing,"ARGS\n");
	putStrLstBuff(listing);

	for(i=0;i<(*ptr).iArg;i++)
	{
		sprintf(listing,"%d)",i);
		putStrLstBuff(listing);

		sprintf(listing," identifier=%s\n",&((*strTbl).text[((*ptr).arg[i]).text]));
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"fpOffset=%d\n",((*ptr).arg[i]).fpOffset);
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"line=%lu\n",((*ptr).arg[i]).line);
		putStrLstBuff(listing);

	}
	sprintf(listing,"LOCALS\n");
	putStrLstBuff(listing);
	for(i=0;i<(*ptr).iLocal;i++)
	{
		sprintf(listing,"%d)",i);
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing," identifier=%s\n",&((*strTbl).text[((*ptr).local[i]).text]));
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"fpOffset=%d\n",((*ptr).local[i]).fpOffset);
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"line=%lu\n",((*ptr).local[i]).line);
		putStrLstBuff(listing);

	}
	sprintf(listing,"LABELS\n");
	putStrLstBuff(listing);
	for(i=0;i<(*ptr).iLabel;i++)
	{
		sprintf(listing,"%d)",i);
		putStrLstBuff(listing);

		sprintf(listing," identifier=%s\n",&((*strTbl).text[((*ptr).label[i]).text]));
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"address=%lu\n",((*ptr).label[i]).address);
		putStrLstBuff(listing);

		putByteLstBuff('\t');
		sprintf(listing,"line=%lu\n",((*ptr).label[i]).line);
		putStrLstBuff(listing);

	}
	return;

}/*end printProcToLst*/

/*-----------------------------------------------------------------*/

void Pass2::printTreeToLst(struct HashTbl* link, int level)
{
	int i;
	int size;

    if(link==NULL)
    {
        return;
    }
    
	printTreeToLst((*link).left,level+1);
    for(i=0;i<level;i++){ putByteLstBuff('-'); }

    sprintf(listing,"id =%s",&((*strTbl).text[(*link).text]));
	size = strlen(listing);
	for(i=0;i<size;i++){ putByteLstBuff(listing[i]);}
	while(i<20){ i++; putByteLstBuff(' '); }

	sprintf(listing," type=%s",SymTypeStr[(*link).type]);
	size = strlen(listing);
	for(i=0;i<size;i++){ putByteLstBuff(listing[i]);}
	while(i<20){ i++; putByteLstBuff(' '); }

	sprintf(listing,"(i,si)=(%d,%d)\n",(*link).index,(*link).subIndex);
	putStrLstBuff(listing);

    printTreeToLst((*link).right,level+1);
	return;

}/*end printTreeToLst*/
