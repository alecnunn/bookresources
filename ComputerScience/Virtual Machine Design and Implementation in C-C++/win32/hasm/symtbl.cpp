/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ symtbl.cpp - the symbol table and associated functions            +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	SymTbl---+---GlobalVariable
			 |
			 +---Procedure-----+--StackFrame ( ret, arg, local )
						       |
						       +--Label

	Pass1 - uses directives to fill out GlobVar, Proc entries
	Pass2 - uses above to generate bytecode
*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros and private variables                                      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOB_INIT	10		/* _INIT = initial array size */
#define GLOB_INC	5		/* _INC = increment size when expand array */

#define PROC_INIT	10
#define PROC_INC	5

#define ARG_INIT	5	
#define ARG_INC		5	

#define LOC_INIT	5
#define LOC_INC		5

#define LBL_INIT	10
#define LBL_INC		10

/*#define SYM_DEBUG		1*/

#ifdef	SYM_DEBUG
#define	SYM_DEBUG0(arg);			printf(arg);
#define SYM_DEBUG1(arg1,arg2);		printf(arg1,arg2);
#define SYM_DEBUG2(arg1,arg2,arg3);	printf(arg1,arg2,arg3);
#else
#define	SYM_DEBUG0(arg);
#define SYM_DEBUG1(arg1,arg2);
#define SYM_DEBUG2(arg1,arg2,arg3);
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class SymbolTable
{
	StringTable *strTbl;

	/*called via public constructors/destructors*/
	void initProc(struct Procedure *ptr);
	void freeProc(struct Procedure *ptr);

	/*called via public addProc___HL */
	void setProcRet(struct Procedure *ptr, struct StackFrame *add);
	void addProcArg(struct Procedure *ptr, struct StackFrame *add);
	void addProcLoc(struct Procedure *ptr, struct StackFrame *add);
	void addProcLbl(struct Procedure *ptr, struct Label *add);

	/*called via public printSymTbl*/
	void printGlobVar(struct GlobalVariable *ptr);
	void printProc(struct Procedure *ptr);

	public:
	struct GlobalVariable *globVar;
	U4 nGlobVar;
	U4 iGlobVar;

	struct Procedure *proc;
	U4 nProc;
	U4 iProc;

	SymbolTable(StringTable *st);
	~SymbolTable();

	void addGlobVar(struct GlobalVariable *add);
	void addProc(struct Procedure *add);

	/*set elements of current procedure*/
	void setProcRetHL(struct StackFrame *add);
	void addProcArgHL(struct StackFrame *add); 
	void addProcLocHL(struct StackFrame *add); 
	void addProcLblHL(struct Label *add); 

	void printSymTbl();

	void test();
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

SymbolTable::SymbolTable(StringTable *st)
{
	U4 i;

	strTbl = st;

	globVar = (struct GlobalVariable*)
		      malloc(GLOB_INIT*sizeof(struct GlobalVariable));

	if(globVar==NULL)
	{
		ERROR0("SymbolTable::SymbolTable(): out of memory\n");
		FATAL_ERROR();
	}
	nGlobVar = GLOB_INIT;
	iGlobVar = 0;

	proc = (struct Procedure*)
		   malloc(PROC_INIT*sizeof(struct Procedure));

	if(proc==NULL)
	{
		ERROR0("SymbolTable::SymbolTable(): out of memory\n");
		FATAL_ERROR();
	}
	nProc = PROC_INIT;
	iProc = 0;
	for(i=0;i<PROC_INIT;i++){ initProc(&proc[i]); }
	
	SYM_DEBUG1("SymbolTable::SymbolTable(): %lu globals\n",GLOB_INIT);
	SYM_DEBUG1("SymbolTable::SymbolTable(): %lu procs\n",PROC_INIT);

	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

void SymbolTable::initProc(struct Procedure *ptr)
{
	(*ptr).text = 0;		
	(*ptr).address = 0;		
	(*ptr).line = 0;
	
	/*exactly one struct StackFrame ret*/
	(*ptr).nRet = 0;

	(*ptr).arg = (struct StackFrame*)
		         malloc(ARG_INIT*sizeof(struct StackFrame));
	if((*ptr).arg==NULL)
	{
		ERROR0("SymbolTable::initProc(): out of memory\n");
		FATAL_ERROR();
	}
	(*ptr).nArg = ARG_INIT;
	(*ptr).iArg = 0;

	(*ptr).local = (struct StackFrame*)
		           malloc(LOC_INIT*sizeof(struct StackFrame));
	if((*ptr).local==NULL)
	{
		ERROR0("SymbolTable::initProc(): out of memory\n");
		FATAL_ERROR();
	}
	(*ptr).nLocal = LOC_INIT;
	(*ptr).iLocal = 0;

	(*ptr).label = (struct Label*)
		           malloc(LBL_INIT*sizeof(struct Label));
	if((*ptr).label==NULL)
	{
		ERROR0("SymbolTable::initProc(): out of memory\n");
		FATAL_ERROR();
	}
	(*ptr).nLabel = LBL_INIT;
	(*ptr).iLabel = 0;
	return;

}/*end initProc*/

/*-----------------------------------------------------------------*/

SymbolTable::~SymbolTable()
{
	free(globVar);
	SYM_DEBUG1("SymbolTable::~SymbolTable(): free %lu globals\n",nGlobVar);
	freeProc(proc);
	return;

}/*end destructor*/

/*-----------------------------------------------------------------*/

void SymbolTable::freeProc(struct Procedure *ptr)
{
	U4 i;
	SYM_DEBUG1("SymbolTable::freeProc(): freeing %lu procs\n",nProc);
	for(i=0;i<nProc;i++)
	{
		free(ptr[i].arg);
		free(ptr[i].local);
		free(ptr[i].label);
	}
	free(ptr);
	SYM_DEBUG0("SymbolTable::freeProc(): done freeing procs\n");
	return;

}/*end freeProc*/

/*-----------------------------------------------------------------*/

void SymbolTable::addGlobVar(struct GlobalVariable *add)
{
	U4 i;
	struct GlobalVariable *new_gv;

	/* capacity n will have indices 0,...,n-1 */

	if(iGlobVar == nGlobVar)
	{
		U4 newsize = nGlobVar + GLOB_INC;
		new_gv = (struct GlobalVariable*)
			     malloc((newsize)*(sizeof(struct GlobalVariable)));
		if(new_gv==NULL)
		{
			ERROR0("SymbolTable::addGlobVar(): out of memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=0;i<nGlobVar;i++){ new_gv[i] = globVar[i]; }
			free(globVar);
			globVar = new_gv;
			nGlobVar = nGlobVar+GLOB_INC;
		}
	}
	
	globVar[iGlobVar]=*add;
	iGlobVar++;
	return;

}/*end addGlobVar*/

/*-----------------------------------------------------------------*/

void SymbolTable::addProc(struct Procedure *add)
{
	U4 i;
	Procedure *new_p;

	/* capacity n will have indices 0,...,n-1 */

	if(iProc == nProc)
	{
		U4 newsize = nProc + PROC_INC;
		new_p = (struct Procedure*)
			    malloc(newsize*sizeof(struct Procedure));
		if(new_p==NULL)
		{
			ERROR0("SymbolTable::addProc(): out of memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=nProc;i<newsize;i++){ initProc(&new_p[i]); }
			for(i=0;i<nProc;i++){ new_p[i] = proc[i]; }
			delete(proc);
			proc = new_p;
			nProc = nProc+PROC_INC;
		}
	}
	
	/*
	adding virgin proc, no internal elements yet
	(i.e. args, locals, labels )
	*/

	proc[iProc].text=(*add).text;
	proc[iProc].address=(*add).address;
	proc[iProc].line=(*add).line;

	iProc++;
	return;

}/*end addProc*/

/*-----------------------------------------------------------------*/

void SymbolTable::setProcRetHL(struct StackFrame *add)
{
	/*
	add arg while processing current procedure
	current proc is at (iProc-1) 
	*/
	setProcRet(&proc[iProc-1],add);
	return;

}/*end setProcRetHL*/

void SymbolTable::setProcRet(struct Procedure *ptr, 
							 struct StackFrame *add)
{
	(*ptr).nRet=1;
	(*ptr).ret=*add;
	return;

}/*end setProcRet*/

/*-----------------------------------------------------------------*/

void SymbolTable::addProcArgHL(struct StackFrame *add)
{
	/*
	add arg while processing current procedure
	current proc is at (iProc-1) 
	*/
	addProcArg(&proc[iProc-1],add);
	return;

}/*end addProcArgHL*/

void SymbolTable::addProcArg(struct Procedure *ptr, 
							 struct StackFrame *add)
{
	U4 i;
	struct StackFrame *new_sf;

	/* capacity n will have indices 0,...,n-1 */

	if((*ptr).iArg == (*ptr).nArg)
	{
		U4 newsize = (*ptr).nArg + ARG_INC;
		new_sf = (struct StackFrame*)
			     malloc((newsize)*(sizeof(struct StackFrame)));
		if(new_sf==NULL)
		{
			ERROR0("SymbolTable::addProcArg(): out of  memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=0;i<((*ptr).nArg);i++){ new_sf[i] = (*ptr).arg[i]; }
			free((*ptr).arg);
			(*ptr).arg = new_sf;
			(*ptr).nArg = (*ptr).nArg+ARG_INC;
		}
	}
	
	(*ptr).arg[(*ptr).iArg]=*add;
	(*ptr).iArg++;
	return;

}/*end addProcArg*/

/*-----------------------------------------------------------------*/

void SymbolTable::addProcLocHL(struct StackFrame *add)
{
	/*
	add local while processing current procedure
	current proc is at (iProc-1) 
	*/
	addProcLoc(&proc[iProc-1],add);
	return;

}/*end addProcLocHL*/

void SymbolTable::addProcLoc(struct Procedure *ptr, 
							 struct StackFrame *add)
{
	U4 i;
	struct StackFrame *new_sf;

	/* capacity n will have indices 0,...,n-1 */

	if((*ptr).iLocal == (*ptr).nLocal)
	{
		U4 newsize = (*ptr).nLocal + LOC_INC;
		new_sf = (struct StackFrame*)
			     malloc((newsize)*(sizeof(struct StackFrame)));
		if(new_sf==NULL)
		{
			ERROR0("SymbolTable::addProcArg(): out of memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=0;i<((*ptr).nLocal);i++){ new_sf[i] = (*ptr).local[i]; }
			free((*ptr).local);
			(*ptr).local = new_sf;
			(*ptr).nLocal = (*ptr).nLocal+LOC_INC;
		}
	}
	
	(*ptr).local[(*ptr).iLocal]=*add;
	(*ptr).iLocal++;
	return;

}/*end addProcLoc*/

/*-----------------------------------------------------------------*/

void SymbolTable::addProcLblHL(struct Label *add)
{
	/*
	add label while processing current procedure
	current proc is at (iProc-1) 
	*/
	addProcLbl(&proc[iProc-1],add);
	return;

}/*end addProcLblHL*/

void SymbolTable::addProcLbl(struct Procedure *ptr, 
							 struct Label *add)
{
	U4 i;
	struct Label *new_sf;

	/* capacity n will have indices 0,...,n-1 */

	if((*ptr).iLabel == (*ptr).nLabel)
	{
		U4 newsize = (*ptr).nLabel + LBL_INC;
		new_sf = (struct Label*)
			     malloc((newsize)*(sizeof(struct Label)));
		if(new_sf==NULL)
		{
			ERROR0("SymbolTable::addProcArg(): out of memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=0;i<((*ptr).nLabel);i++){ new_sf[i] = (*ptr).label[i]; }
			free((*ptr).label);
			(*ptr).label = new_sf;
			(*ptr).nLabel = (*ptr).nLabel+LBL_INC;
		}
	}
	
	(*ptr).label[(*ptr).iLabel]=*add;
	(*ptr).iLabel++;
	return;

}/*end addProcLbl*/

/*-----------------------------------------------------------------*/

void SymbolTable::printSymTbl()
{
	U4 i;
	printf("\nSYMBOL TABLE------------\n");
	for(i=0;i<iGlobVar;i++)
	{
		printf("%d) ",i); 
		printGlobVar(&globVar[i]); 
		printf("\n");
	}
	for(i=0;i<iProc;i++)
	{
		printf("%d) ",i); 
		printProc(&proc[i]); 
		printf("\n");
	}
	return;

}/*end printSymTbl*/

/*-----------------------------------------------------------------*/

void SymbolTable::printGlobVar(struct GlobalVariable *ptr)
{
	printf("GLOBAL VAR--------------\n");
	printf("identifier=%s\n",&((*strTbl).text[(*ptr).text]));
	printf("data type=%s\n",globSz[(*ptr).dType]);
	printf("length="); pU8((*ptr).len); printf("\n");
	printf("size="); pU8((*ptr).size); printf("\n");
	printf("offset="); pS8(-((S8)(*ptr).offset)); printf("\n");
	printf("line=%lu\n",(*ptr).line); 
	return;

}/*end printGlobVar*/

/*-----------------------------------------------------------------*/

void SymbolTable::printProc(struct Procedure *ptr)
{
	U4 i;
	printf("PROC--------------------\n");
	printf("identifier=%s\n",&((*strTbl).text[(*ptr).text]));
	printf("address="); pU8((*ptr).address); printf("\n");
	printf("line=%lu\n\n",(*ptr).line);

	if((*ptr).nRet)
	{
		printf("RET\n");
		printf("\tid=%s\n",&((*strTbl).text[((*ptr).ret).text]));
		printf("\tfpOffset=%ld\n",((*ptr).ret).fpOffset);
		printf("\tline=%lu\n",((*ptr).ret).line);
	}

	printf("ARGS\n");
	for(i=0;i<(*ptr).iArg;i++)
	{
		printf("\t%d)",i);
		printf("id=%s\n",&((*strTbl).text[((*ptr).arg[i]).text]));
		printf("\tfpOffset=%ld\n",((*ptr).arg[i]).fpOffset);
		printf("\tline=%lu\n",((*ptr).arg[i]).line);
	}
	printf("LOCALS\n");
	for(i=0;i<(*ptr).iLocal;i++)
	{
		printf("\t%d)",i);
		printf("id=%s\n",&((*strTbl).text[((*ptr).local[i]).text]));
		printf("\tfpOffset=%ld\n",((*ptr).local[i]).fpOffset);
		printf("\tline=%lu\n",((*ptr).local[i]).line);
	}
	printf("LABELS\n");
	for(i=0;i<(*ptr).iLabel;i++)
	{
		printf("\t%d)",i);
		printf("id=%s\n",&((*strTbl).text[((*ptr).label[i]).text]));
		printf("\taddress="); 
		pU8(((*ptr).label[i]).address); 
		printf("\n");
		printf("\tline=%lu\n",((*ptr).label[i]).line);
	}
	return;

}/*end printProc*/

/*-----------------------------------------------------------------*/

void SymbolTable::test()
{
	U8 str_index;
	char name[32];
	struct GlobalVariable globvar;

	struct Procedure proc;
	struct StackFrame sf;
	struct Label label;

	strcpy(name,"gVar_1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	globvar.text=str_index;
	globvar.dType=1;
	globvar.len=globvar.size=globvar.offset=globvar.line=0;
	addGlobVar(&globvar);

	strcpy(name,"gVar_2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	globvar.text=str_index;
	globvar.dType=1;
	globvar.len=globvar.size=globvar.offset=globvar.line=0;
	addGlobVar(&globvar);

	strcpy(name,"gVar_3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	globvar.text=str_index;
	globvar.dType=1;
	globvar.len=globvar.size=globvar.offset=globvar.line=0;
	addGlobVar(&globvar);

	strcpy(name,"gVar_4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	globvar.text=str_index;
	globvar.dType=1;
	globvar.len=globvar.size=globvar.offset=globvar.line=0;
	addGlobVar(&globvar);

	strcpy(name,"gVar_5");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	globvar.text=str_index;
	globvar.dType=1;
	globvar.len=globvar.size=globvar.offset=globvar.line=0;
	addGlobVar(&globvar);

	/*add 4 procedures*/

	strcpy(name,"proc1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	proc.text=str_index;
	proc.address = proc.line = 0;
	addProc(&proc);
	strcpy(name,"proc1_ret");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	setProcRetHL(&sf);
	strcpy(name,"proc1_arg1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc1_arg2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc1_arg3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc1_arg4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc1_loc1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc1_loc2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc1_loc3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc1_loc4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc1_lbl1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc1_lbl2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc1_lbl3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc1_lbl4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);

	strcpy(name,"proc2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	proc.text=str_index;
	proc.address = proc.line = 0;
	addProc(&proc);
	strcpy(name,"proc2_ret");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	setProcRetHL(&sf);
	strcpy(name,"proc2_arg1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc2_arg2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc2_arg3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc2_arg4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc2_loc1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc2_loc2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc2_loc3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc2_loc4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc2_lbl1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc2_lbl2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc2_lbl3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc2_lbl4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);

	strcpy(name,"proc3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	proc.text=str_index;
	proc.address = proc.line = 0;
	addProc(&proc);
	strcpy(name,"proc3_ret");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	setProcRetHL(&sf);
	strcpy(name,"proc3_arg1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc3_arg2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc3_arg3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc3_arg4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc3_loc1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc3_loc2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc3_loc3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc3_loc4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc3_lbl1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc3_lbl2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc3_lbl3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc3_lbl4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);

	strcpy(name,"proc4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	proc.text=str_index;
	proc.address = proc.line = 0;
	addProc(&proc);
	strcpy(name,"proc4_ret");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	setProcRetHL(&sf);
	strcpy(name,"proc4_arg1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc4_arg2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc4_arg3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc4_arg4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcArgHL(&sf);
	strcpy(name,"proc4_loc1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc4_loc2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc4_loc3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc4_loc4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	sf.text = str_index;
	sf.fpOffset = sf.line =0;
	addProcLocHL(&sf);
	strcpy(name,"proc4_lbl1");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc4_lbl2");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc4_lbl3");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);
	strcpy(name,"proc4_lbl4");
	str_index = (*strTbl).iStr;
	(*strTbl).addStrTbl(name);
	label.text = str_index;
	label.address = label.line =0;
	addProcLblHL(&label);


	printSymTbl();
	(*strTbl).printStrTbl();
	return;

}/*end test*/
