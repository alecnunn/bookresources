/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ pass1.c - process directives to build symbol table				+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define PASS1_DEBUG		1*/

#ifdef	PASS1_DEBUG
#define	PASS1_DEBUG0(arg);						printf(arg);
#define PASS1_DEBUG1(arg1,arg2);				printf(arg1,arg2);
#define PASS1_DEBUG2(arg1,arg2,arg3);			printf(arg1,arg2,arg3);
#else
#define	PASS1_DEBUG0(arg);
#define PASS1_DEBUG1(arg1,arg2);
#define PASS1_DEBUG2(arg1,arg2,arg3);
#endif

#define OUTSIDE_PROC_PASS1	-1

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class Pass1
{
	StringTable *strTbl;	/*pointers to symbol repository structures*/
	SymbolTable *symTbl;
	HashTable *hashTbl;

	U4 currentProcPass1;	/*index into symTbl of current procedure*/
	U8 bytePosPass1;		/*current index of bytecode being generated*/
	U8 globalTotal;			/*current total size of globals*/

	void  processDirective(struct Token *tptr, LineTokenizer *ptr);
	void  processGDirective(struct Token *tptr, LineTokenizer *ptr);
	void  processPDirective(struct Token *tptr, LineTokenizer *ptr);

	void  processInstruction(struct Token *tptr);

	public:
	Pass1(StringTable *st, SymbolTable *syt, HashTable *ht);
	void parseLine(struct Line *line);
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

Pass1::Pass1(StringTable *st, SymbolTable *syt, HashTable *ht)
{ 
	strTbl = st;
	symTbl = syt;
	hashTbl = ht;

	currentProcPass1 = OUTSIDE_PROC_PASS1;
	bytePosPass1 = 0;
	globalTotal = 0;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

void Pass1::parseLine(struct Line *line)
{
	struct Token token;
	PASS1_DEBUG2("Pass1::parseLine(): line->(%d)\t text->(%s)\n",(*line).line,(*line).src);

	/*
	line can be
		i) a directive ( starts with a period, i.e.  .IP )
		ii) an instruction ( opcode operand(s) )
		iii) a comment ( starts with # )
	*/
	
	LineTokenizer toker(line);
	token = toker.getNextLineToken();

	if(token.type==TOK_NO_MORE){ return; }

	switch(token.text[0])
	{
		case '.' :{ processDirective(&token,&toker); }break;
		case '#' :{ /*comment, ignore line*/ }break;
		default:{ processInstruction(&token); }
	}
	return;

}/*end parseLine*/

/*-----------------------------------------------------------------*/

/*
	Directives:
	.GB identifier  [ integer ]		define global variable
	( same for .GW, .GD, .GQ )
	.PB	identifier					procedure begin
		.PR identifier +n				procedure return value
		.PA identifier +n				procedure argument
		.PV identifier -n				procedure local variable
		.PL identifier					procedure label
	.PE								procedure end
*/

void  Pass1::processDirective(struct Token *tptr, LineTokenizer *ptr)
{
	if((*tptr).type==TOK_IDENTIFIER)
	{
		switch((*tptr).text[1])
		{
			case 'G':{ processGDirective(tptr,ptr); }break;
			case 'P':{ processPDirective(tptr,ptr); }break;
			default:
			{
				ERROR2("Pass1::processDirective(): %s not directive, line %lu\n",(*tptr).text,(*tptr).line);
			}
		}
	}
	else
	{
		ERROR2("Pass1::processDirective(): %s not directive, line %lu\n",(*tptr).text,(*tptr).line);
	}
	return;

}/*end processDirective*/

/*-----------------------------------------------------------------*/

/*
.GB identifier  [ integer ]	define global variable
	( same for .GW, .GD, .GQ )
*/

void  Pass1::processGDirective(struct Token *tptr, LineTokenizer *ptr)
{
	struct Token t;
	struct HashTbl *hptr; /*returned from hash table*/
	U1 bret;

	/*global variable metadata*/
	char id[ID_SIZE];
	U1 dtype;
	U8 length;
	U8 size;
	U8 offset;
	U4 line;

	switch((*tptr).text[2]) /*.GB identifier  [ integer ]*/
	{
		/*third char, text[2], gives datatype*/

		case 'B':{ dtype = SZ_BYTE; }break;
		case 'W':{ dtype = SZ_WORD; }break;
		case 'D':{ dtype = SZ_DWORD; }break;
		case 'Q':{ dtype = SZ_QWORD; }break;
		default:
		{
			ERROR2("Pass1::processGDirective(): line %d, invalid global data type (%s)\n",(*tptr).line,(*tptr).text);
			return;
		}
	}

	/*set some defaults*/

	length = 1;
	size = dtype*length; 
	offset = 0;
	line = (*tptr).line;

	bret = (*ptr).match(&t,TOK_IDENTIFIER);
	if(bret!=TRUE)
	{ 
		ERROR1("Pass1::processGDirective(): line %d, global missing identifier\n",(*tptr).line);
		return; 
	}

	strcpy(id,t.text);

	PASS1_DEBUG2("Pass1::processGDirective(): .GX %s=%s\n",TokStr[t.type],t.text);
		
	/*
	the [integer] modifiers is optional
	so we cannot use match() because there are 2 correct ways
	
	i)	 .GX identifier integer EOL
	ii)  .GX identifier EOL
	*/

	t = (*ptr).getNextLineToken();

	/* .GX identifier integer EOL */

	if(t.type==TOK_INT_CONST)
	{
		PASS1_DEBUG2("Pass1::processGDirective(): nElements %s=%lu\n",TokStr[t.type],t.val);
			
		length = (U8)t.val;
		size = dtype*length;

		globalTotal = globalTotal + size;
		offset = globalTotal;

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret==FALSE)
		{ 
			ERROR1("Pass1::processGDirective(): line %d, bad global declaration\n",(*tptr).line);
			return; 
		}
	}

	/*.GX identifier EOL*/

	else if(t.type==TOK_NO_MORE)
	{
		size = dtype;
		globalTotal = globalTotal + size;
		offset = globalTotal;
	}

	/*.GX identifer XXXX */

	else 
	{ 
		ERROR1("Pass1::processGDirective(): line %d, bad global declaration\n",(*tptr).line);
		return; 
	}
	

	/*if evade all errors, add .GB entry*/

	PASS1_DEBUG0("Pass1::processGDirective(): adding .GX to symbol table\n");
	hptr =  (*hashTbl).queryHashTbl(id);
	if(hptr!=NULL)
	{
		ERROR2("Pass1::processGDirective(): line %d, %s re-defined\n",line,id);
		return;
	}
	else
	{
		struct GlobalVariable gv;
		gv.text = (*strTbl).iStr;
		gv.dType = dtype;
		gv.len = length;
		gv.size = size;
		gv.offset = offset;
		gv.line= line;
		(*strTbl).addStrTbl(id);
		(*symTbl).addGlobVar(&gv);
		(*hashTbl).addHashTblEntry(id,gv.text,GLOBAL_VAR,((*symTbl).iGlobVar-1),0,line);
	}
	return;

}/*end processGDirective*/

/*-----------------------------------------------------------------*/

/*
	.PB	identifier 				procedure begin
		.PR identifier +n			procedure return value
		.PA identifier +n			procedure argument
		.PV identifier -n			procedure local variable
		.PL identifier				procedure label
	.PE							procedure end
*/

void  Pass1::processPDirective(struct Token *tptr, LineTokenizer *ptr)
{
	struct Token t;
	U1 bret;

	if(strcmp((*tptr).text,".PB")==0)  /*.PB	identifier EOL*/
	{
		char id[ID_SIZE];
		U8 address;
		U4 line;
		struct HashTbl *hptr;

		/*set some defaults*/
		address = 0;
		line  = 0;

		bret = (*ptr).match(&t,TOK_IDENTIFIER);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return; 
		}
		
		strcpy(id,t.text);
		line = t.line;
		address = bytePosPass1;

		PASS1_DEBUG2("Pass1::processPDirective(): .PB identifier %s=%s\n",TokStr[t.type],t.text);

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret==FALSE)
		{ 
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return; 
		}	
		
		PASS1_DEBUG0("Pass1::processPDirective(): adding .PB to symbol table\n");

		hptr =  (*hashTbl).queryHashTbl(id);

		if(hptr!=NULL)
		{
			ERROR2("Pass1::processPDirective(): line %d, %s re-definition\n",line,id);
			return;
		}
		else if(currentProcPass1!=OUTSIDE_PROC_PASS1)
		{
			ERROR2("Pass1::processPDirective(): line %d, %s, cannot nest procedures\n",line,id);
			return;
		}
		else
		{
			struct Procedure p;
			p.text = (*strTbl).iStr;
			p.address = address;
			p.line= line;
			(*strTbl).addStrTbl(id);
			(*symTbl).addProc(&p);
			(*hashTbl).addHashTblEntry(id,p.text,PROC,((*symTbl).iProc-1),0,line);
			currentProcPass1 = (*symTbl).iProc-1;
		}

	}/*end .PB*/
	else if(strcmp((*tptr).text,".PR")==0)	/*.PR identifier +n	*/
	{
		char name[ID_SIZE];
		S4 offset;
		U4 line;
		struct HashTbl *hptr;

		line = (*tptr).line;

		bret = (*ptr).match(&t,TOK_IDENTIFIER);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}

		strcpy(name,t.text);

		bret = (*ptr).match(&t,TOK_INT_CONST);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;
		}

		offset = (S4)t.val;

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret!=TRUE)
		{ 
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return; 
		}
			
		PASS1_DEBUG0("processPDirective(): adding .PR to symbol table\n");

		hptr =  (*hashTbl).queryHashTbl(name);
		if(hptr!=NULL)
		{
			ERROR2("processPDirective(): line %d, %s re-defined\n",line,name);
			return;
		}
		else
		{
			struct StackFrame sf;
			sf.text = (*strTbl).iStr;
			sf.fpOffset = offset;
			sf.line = line;
			(*strTbl).addStrTbl(name);
			(*symTbl).setProcRetHL(&sf);
			(*hashTbl).addHashTblEntry(name,sf.text,PROC_RET,((*symTbl).iProc-1),0,line);
		}

	}/*end .PR*/
	else if(strcmp((*tptr).text,".PA")==0)	/*.PA identifier +n*/
	{
		char name[ID_SIZE];
		S4 offset;
		U4 line;
		struct HashTbl *hptr;

		bret = (*ptr).match(&t,TOK_IDENTIFIER);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}

		strcpy(name,t.text);
		line = t.line;

		bret = (*ptr).match(&t,TOK_INT_CONST);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}
		
		offset = (S4)t.val;

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret!=TRUE)
		{ 
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return; 
		}
				
		PASS1_DEBUG0("processPDirective(): adding .PA to symbol table\n");

		hptr =  (*hashTbl).queryHashTbl(name);
		if(hptr!=NULL)
		{
			ERROR2("processPDirective(): line %d, %s re-defined\n",line,name);
			return;
		}
						
		struct StackFrame sf;
		sf.text = (*strTbl).iStr;
		sf.fpOffset = offset;
		sf.line = line;
		(*strTbl).addStrTbl(name);
		(*symTbl).addProcArgHL(&sf);
		(*hashTbl).addHashTblEntry(name,
			                       sf.text,
								   PROC_ARG,
								   ((*symTbl).iProc-1),
								   (((*symTbl).proc[(*symTbl).iProc-1]).iArg-1),
								   line);
		
	}/*end .PA*/
	else if(strcmp((*tptr).text,".PV")==0)	/*.PV identifier -n*/
	{
		char name[ID_SIZE];
		S4 offset;
		U4 line;
		struct HashTbl *hptr;

		line = (*tptr).line;

		bret = (*ptr).match(&t,TOK_IDENTIFIER);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}
			
		strcpy(name,t.text);

		bret = (*ptr).match(&t,TOK_INT_CONST);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}

		offset = (S4)t.val;

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}
		
		PASS1_DEBUG0("processPDirective(): adding .PV to symbol table\n");

		hptr =  (*hashTbl).queryHashTbl(name);
		if(hptr!=NULL)
		{
			ERROR2("processPDirective(): line %d, %s re-defined\n",line,name);
			return;
		}
		
		struct StackFrame sf;
		sf.text = (*strTbl).iStr;
		sf.fpOffset = (S4)offset;
		sf.line = line;
		(*strTbl).addStrTbl(name);
		(*symTbl).addProcLocHL(&sf);
		(*hashTbl).addHashTblEntry(name,
			            sf.text,
						PROC_LOC,
						((*symTbl).iProc-1),
						(((*symTbl).proc[(*symTbl).iProc-1]).iLocal-1),
						line);

	}/*end .PV*/
	else if(strcmp((*tptr).text,".PL")==0)	/*.PL identifier*/
	{
		char name[ID_SIZE];
		U8 address;
		U4 line;
		struct HashTbl *hptr;
		
		line = (*tptr).line;
		bret = (*ptr).match(&t,TOK_IDENTIFIER);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}
		
		strcpy(name,t.text);
		address = bytePosPass1;

		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret!=TRUE)
		{
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return;

		}
	
		PASS1_DEBUG0("processPDirective(): adding .PL to symbol table\n");

		hptr =  (*hashTbl).queryHashTbl(name);
		if(hptr!=NULL)
		{
			ERROR2("processPDirective(): line %d, %s re-defined\n",line,name);
			return;
		}
				
		struct Label lb;
		lb.text = (*strTbl).iStr;
		lb.address = address;
		lb.line = line;
		(*strTbl).addStrTbl(name);
		(*symTbl).addProcLblHL(&lb);
		(*hashTbl).addHashTblEntry(name,
			                       lb.text,
								   PROC_LBL,
								   ((*symTbl).iProc-1),
								   (((*symTbl).proc[(*symTbl).iProc-1]).iLabel-1),
								   line);

	}/*end .PL*/
	else if(strcmp((*tptr).text,".PE")==0)/*.PE*/
	{
		bret = (*ptr).match(&t,TOK_NO_MORE);
		if(bret==TRUE)
		{ 
			/*
			cannot define code outside of a procedure
			*/
			currentProcPass1 = OUTSIDE_PROC_PASS1;
		}
		else
		{ 
			ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
			return; 
		}

	}/*end .PE*/
	else
	{
		ERROR1("Pass1::processPDirective(): line %lu, bad directive\n",(*tptr).line);
	}
	return;

}/*end processPDirective*/

/*-----------------------------------------------------------------*/

/*
Do not generate bytecode in first pass, but we need to know
	-offset of label directives
	-offset of procedures
Thus, we need to do minimal instruction processing to keep
track of bytesize via ->bytePos<- variable

use switch to help cut down on calls to strcmp()

can probably recycle basic skeleton and extend on this code for Pass2
*/

void  Pass1::processInstruction(struct Token *tptr)
{
	if((*tptr).type!=TOK_IDENTIFIER)
	{
		ERROR3("Pass1::processInstruction(): %s on line %lu type %s not a valid opcode\n",(*tptr).text,(*tptr).line,TokStr[(*tptr).type]);
		return;
	}
	
	switch((*tptr).text[0])
	{
		case 'A':
		{
			if(strcmp((*tptr).text,"ADD")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"AND")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'B':
		{
			if(strcmp((*tptr).text,"BS")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"BT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}			
		}break;
		case 'C':
		{
			if(strcmp((*tptr).text,"CAST_IF")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"CAST_ID")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"CAST_FI")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"CAST_FD")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"CAST_DI")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"CAST_DF")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'D':
		{
			if(strcmp((*tptr).text,"DIV")==0){ bytePosPass1 = bytePosPass1 + 5;}
			else if(strcmp((*tptr).text,"DI")==0){ bytePosPass1 = bytePosPass1 + 1;}
			else if(strcmp((*tptr).text,"DADD")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"DSUB")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"DMULT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"DDIV")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"DSLT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'E':
		{
			if(strcmp((*tptr).text,"EI")==0){ bytePosPass1 = bytePosPass1 + 1;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'F':
		{
			if(strcmp((*tptr).text,"FADD")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"FSUB")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"FMULT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"FDIV")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"FSLT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'H':
		{
			if(strcmp((*tptr).text,"HALT")==0){ bytePosPass1 = bytePosPass1 + 1;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'I':
		{
			if(strcmp((*tptr).text,"INT")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'J':
		{
			if(strcmp((*tptr).text,"JMP")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"JE")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"JNE")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'L':
		{
			if(strcmp((*tptr).text,"LBI")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LWI")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"LDI")==0){ bytePosPass1 = bytePosPass1 + 6;}
			else if(strcmp((*tptr).text,"LQI")==0){ bytePosPass1 = bytePosPass1 + 10;}
			else if(strcmp((*tptr).text,"LF1I")==0){ bytePosPass1 = bytePosPass1 + 6;}
			else if(strcmp((*tptr).text,"LF2I")==0){ bytePosPass1 = bytePosPass1 + 10;}
			else if(strcmp((*tptr).text,"LB")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LW")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LD")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LQ")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LF1")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LF2")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"LAD")==0){ bytePosPass1 = bytePosPass1 + 10;}
			else if(strcmp((*tptr).text,"LAI")==0){ bytePosPass1 = bytePosPass1 + 11;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'M':
		{
			if(strcmp((*tptr).text,"MOV")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"MOVF")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"MOVD")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"MULT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;				
			}
		}break;
		case 'N':
		{
			if(strcmp((*tptr).text,"NOT")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"NOP")==0){ bytePosPass1 = bytePosPass1 + 1;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'O':
		{
			if(strcmp((*tptr).text,"OR")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'P':
		{
			if(strcmp((*tptr).text,"PUSHB")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"PUSHW")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"PUSHD")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"PUSHQ")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"PUSHF1")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"PUSHF2")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPB")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPW")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPD")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPQ")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPF1")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else if(strcmp((*tptr).text,"POPF2")==0){ bytePosPass1 = bytePosPass1 + 2;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'S':
		{
			if(strcmp((*tptr).text,"SB")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SW")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SD")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SQ")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SF1")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SF2")==0){ bytePosPass1 = bytePosPass1 + 3;}
			else if(strcmp((*tptr).text,"SRA")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"SRL")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"SL")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"SUB")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else if(strcmp((*tptr).text,"SLT")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		case 'X':
		{
			if(strcmp((*tptr).text,"XOR")==0){ bytePosPass1 = bytePosPass1 + 4;}
			else
			{
				ERROR2("Pass1::processInstruction(): line %d, invalid opcode (%s)\n",(*tptr).line,(*tptr).text);
				return;
			}
		}break;
		default:
		{
			ERROR3("Pass1::processInstruction(): %s on line %lu type %s not a valid opcode\n",(*tptr).text,(*tptr).line,TokStr[(*tptr).type]);
			return;
		}
	}/*end switch*/

	return;

}/*end processInstruction*/