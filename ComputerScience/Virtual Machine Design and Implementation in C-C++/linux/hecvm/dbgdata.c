/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ dbgdata.c reads executable, populates debug data structures       +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define SIZE_HEADER		26		/* byte size of executable header */
#define SIZE_GLOBREC	37		/* byte size of global record */
#define SIZE_PROCREC	25		/* byte size of procedure record */
#define SIZE_RETREC		16		/* byte size of return value record */
#define SIZE_ARGREC		16		/* byte size of argument record */
#define SIZE_LOCREC		16		/* byte size of local variable record */
#define SIZE_LBLREC		20		/* byte size of label record */

#define MAX_RECSIZE		SIZE_GLOBREC

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ declarations                                                      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct HeaderRec
{
	U2 magic;		/*should be magic number 0xDEED */
	U8 szSymTbl;	/*size of symbol table*/
	U8 szStrTbl;	/*size of string table*/
	U8 szByteCode;  /*size of bytecode*/
};

struct Contents
{
	U4 nGlobVarRec;		/*number of global variable records in symbol table*/
	U4 nProcRec;		/*number of procedure records in symbol table*/
};

#define	SZ_BYTE		1	/*used to indicate GlobVar type*/
#define SZ_WORD		2
#define SZ_DWORD	4
#define SZ_QWORD	8
char *globSz[]={"","SZ_BYTE","SZ_WORD","","SZ_DWORD",
                "","","","SZ_QWORD"};

struct GlobVarRec
{
	U8 text;		/*index into StrTbl of where identifier begins*/
	U1 dType;		/* SZ_BYTE, SZ_WORD, SZ_DWORD, SZ_QWORD */
	U8 len;			/* # elements if array */
	U8 size;		/* total byte size */
	S8 offset;      /* offset below $TOP, address(g) = $TOP - offset*/
	U4 line;		/* source code line containing declaration */
};
struct StackFrameRec
{
	U8 text;		/* index into StrTbl of where identifier begins */
	S4 fpOffset;	/* +n or -n from $FP */
	U4 line;		/* source code line containing declaration */
};
struct LabelRec
{
	U8 text;		/* index into StrTbl of where identifier begins */
	U8 address;		/* address of label*/
	U4 line;		/* source code line containing declaration */
};
struct ProcRec
{
	U8 text;		/* index into StrTbl of where identifier begins */
	U8 address;		/* address of procedure */
	U4 line;		/* source code line containing declaration */
	struct StackFrameRec ret;
	U1 nRet;		/* 0 = void return, 1 = returns a value*/
	struct StackFrameRec *arg;
	U1 nArg;
	struct StackFrameRec *local;
	U1 nLocal;
	struct LabelRec *label;
	U2 nLabel;
};
struct DebugData
{
	struct Contents contents;
	struct GlobVarRec *gvRec;
	struct ProcRec *pRec;
	U1 *strTbl;
};

U1 debug;		/* TRUE if VM is in debug mode */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private variables                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct DebugData debugData;
U1 debugbytes[MAX_RECSIZE];	/*holds data read in for single record*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printDebugData(struct DebugData *ptr);
void populateDebugData(struct HeaderRec *hptr, struct DebugData *ptr, FILE *fptr);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private prototypes                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printProcRec(struct ProcRec *pptr, struct DebugData *ptr);
void printStackFrameRec(struct StackFrameRec *sptr, struct DebugData *ptr);
void printLabelRec(struct LabelRec *lptr, struct DebugData *ptr);

struct HeaderRec readHeaderRec(FILE *fptr);
struct Contents readContents(FILE* fptr);
struct GlobVarRec readGlobVarRec(FILE *fptr);
struct ProcRec readProcRec(FILE *fptr);
struct StackFrameRec readStackFrameRec(FILE *fptr);
struct LabelRec readLabelRec(FILE *fptr);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printDebugData(struct DebugData *ptr)
{
	U8 i;

	for(i=0;i<((*ptr).contents).nGlobVarRec;i++)
	{
		printf("GLOBVAR "); pU8(i); printf("\n");
		printf("\tid=%s\n",&((*ptr).strTbl[(*ptr).gvRec[i].text]));
		printf("\ttype=%s\n",globSz[(*ptr).gvRec[i].dType]);
		printf("\tlen="); pU8((*ptr).gvRec[i].len); printf("\n");
		printf("\tsize="); pU8((*ptr).gvRec[i].size); printf("\n");
		printf("\toffset="); pS8((*ptr).gvRec[i].offset); printf("\n");
		printf("\tline="); pU4((*ptr).gvRec[i].line); printf("\n\n");
	}

	for(i=0;i<((*ptr).contents).nProcRec;i++)
	{
		printf("PROC "); pU8(i); printf("\n");
		printProcRec(&((*ptr).pRec[i]),ptr);
	}

	return;

}/*end printDebugData*/

/*-----------------------------------------------------------------*/

void printProcRec(struct ProcRec *pptr, struct DebugData *ptr)
{
	U4 i;

	printf("\tid=%s\n",&((*ptr).strTbl[(*pptr).text]));
	printf("\taddress="); pU8((*pptr).address); printf("\n");
	printf("\tline="); pU4((*pptr).line); printf("\n");
	if((*pptr).nRet)
	{
		printf("\tRET\n");
		printStackFrameRec(&((*pptr).ret), ptr);
	}
	for(i=0;i<(*pptr).nArg;i++)
	{
		printf("\tARG->"); pU4(i); printf("\n");
		printStackFrameRec(&((*pptr).arg[i]), ptr);
	}
	for(i=0;i<(*pptr).nLocal;i++)
	{
		printf("\tLOCAL->"); pU4(i); printf("\n");
		printStackFrameRec(&((*pptr).local[i]), ptr);
	}
	for(i=0;i<(*pptr).nLabel;i++)
	{
		printf("\tLABEL->"); pU4(i); printf("\n");
		printLabelRec(&((*pptr).label[i]), ptr);
	}
	return;

}/*end printProcRec*/

/*-----------------------------------------------------------------*/

void printStackFrameRec(struct StackFrameRec *sptr, struct DebugData *ptr)
{
	printf("\t\tid=%s\n",&((*ptr).strTbl[(*sptr).text]));
	printf("\t\tfpOffset="); pS4((*sptr).fpOffset); printf("\n");
	printf("\t\tline="); pU4((*sptr).line); printf("\n");
	return;

}/*end printStackFrameRec*/

/*-----------------------------------------------------------------*/

void printLabelRec(struct LabelRec *lptr, struct DebugData *ptr)
{
	printf("\t\tid=%s\n",&((*ptr).strTbl[(*lptr).text]));
	printf("\t\taddress="); pU8((*lptr).address); printf("\n");
	printf("\t\tline="); pU4((*lptr).line); printf("\n");
	return;

}/*end printLabelRec*/

/*-----------------------------------------------------------------*/

void populateDebugData(struct HeaderRec *hptr, struct DebugData *ptr, FILE *fptr)
{
	U8 i;
	U4 np;
	U4 ng;

	/*1) read header, skip population if zero sizes */

	(*hptr) = readHeaderRec(fptr);

	if(((*hptr).szSymTbl==0)&&((*hptr).szStrTbl==0))
	{ 
		(*ptr).contents.nGlobVarRec = 0;
		(*ptr).contents.nProcRec = 0;
		return; 
	}

	/*2) read table of contents, info on number of globs and procs */

	(*ptr).contents = readContents(fptr);
	ng = ((*ptr).contents).nGlobVarRec;
	np = ((*ptr).contents).nProcRec;

	/*3) based on contents info, allocate globs and procs */

	(*ptr).gvRec = (struct GlobVarRec*)malloc(sizeof(struct GlobVarRec)*ng);

	if((*ptr).gvRec==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for global variable records");
	}

	(*ptr).pRec = (struct ProcRec*)malloc(sizeof(struct ProcRec)*np);

	if((*ptr).pRec==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for procedure records");
	}

	/*4) read in globvar recs following contents */

	for(i=0;i<ng;i++)
	{
		(*ptr).gvRec[i]= readGlobVarRec(fptr);
	}

	/*5) read in procs which follow globvars */

	for(i=0;i<np;i++)
	{
		(*ptr).pRec[i]= readProcRec(fptr);
	}

	/*
	6) allocate and populate string table (follows symbol table)
	   note: native limit via malloc(size_t) 
	*/

	(*ptr).strTbl = (U1*)malloc((size_t)(*hptr).szStrTbl);

	if((*ptr).strTbl==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for string table");
	}

	for(i=0;i<(*hptr).szStrTbl;i++)
	{
		(*ptr).strTbl[i] = (U1)fgetc(fptr);
	}
	return;

}/*end populateDebugData*/

/*-----------------------------------------------------------------*/

struct HeaderRec readHeaderRec(FILE *fptr)
{
	struct HeaderRec hr;
	int i;
	i = fread(debugbytes,sizeof(U1),SIZE_HEADER,fptr);
	if(i<SIZE_HEADER)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading header record");
	}
	
	if((debugbytes[0]==0xDE)&&(debugbytes[1]==0xED))
	{ 
		hr.magic = 0xDEED; 
	}
	else
	{ 
		hr.magic = 0xBAD; 
	}
	
	hr.szSymTbl = bytecodeToQWord(&(debugbytes[2]));
	hr.szStrTbl = bytecodeToQWord(&(debugbytes[10]));
	hr.szByteCode = bytecodeToQWord(&(debugbytes[18]));

	return(hr);

}/*end readHeaderRec*/

/*-----------------------------------------------------------------*/

struct Contents readContents(FILE* fptr)
{
	struct Contents ct;
	int i;
	i = fread(debugbytes,sizeof(U1),8,fptr);
	if(i<8)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading contents record");
	}
	
	ct.nGlobVarRec = bytecodeToDWord(&(debugbytes[0]));
	ct.nProcRec = bytecodeToDWord(&(debugbytes[4]));

	return(ct);

}/*end readContents*/

/*-----------------------------------------------------------------*/

struct GlobVarRec readGlobVarRec(FILE *fptr)
{
	struct GlobVarRec gr;
	int i;
	i = fread(debugbytes,sizeof(U1),SIZE_GLOBREC,fptr);
	if(i<SIZE_GLOBREC)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading global record");
	}
	gr.text = bytecodeToQWord(&(debugbytes[0]));
	gr.dType = debugbytes[8];
	gr.len = bytecodeToQWord(&(debugbytes[9]));
	gr.size = bytecodeToQWord(&(debugbytes[17]));
	gr.offset = bytecodeToQWord(&(debugbytes[25]));
	gr.line = bytecodeToDWord(&(debugbytes[33]));

	return(gr);

}/*end readGlobVarRec*/

/*-----------------------------------------------------------------*/

struct ProcRec readProcRec(FILE *fptr)
{
	struct ProcRec pr;
	int i;
	i = fread(debugbytes,sizeof(U1),SIZE_PROCREC,fptr);
	if(i<SIZE_PROCREC)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading procedure record");
	}

	pr.text = bytecodeToQWord(&(debugbytes[0]));
	pr.address = bytecodeToQWord(&(debugbytes[8]));
	pr.line = bytecodeToDWord(&(debugbytes[16]));
	pr.nRet = debugbytes[20];
	pr.nArg = debugbytes[21];
	pr.nLocal = debugbytes[22];
	pr.nLabel = bytecodeToWord(&(debugbytes[23]));

	pr.arg = (struct StackFrameRec *)malloc(pr.nArg*sizeof(struct StackFrameRec));
	pr.local= (struct StackFrameRec *)malloc(pr.nLocal*sizeof(struct StackFrameRec));
	pr.label= (struct LabelRec *)malloc(pr.nLabel*sizeof(struct LabelRec));

	if(pr.arg==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for procedure-argument records");
	}
	if(pr.local==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for procedure-local records");
	}
	if(pr.label==NULL)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("could not allocate space for procedure-label records");
	}

	if(pr.nRet)
	{
		pr.ret = readStackFrameRec(fptr);
	}
	for(i=0;i<pr.nArg;i++)
	{
		pr.arg[i]= readStackFrameRec(fptr);
	}
	for(i=0;i<pr.nLocal;i++)
	{
		pr.local[i]= readStackFrameRec(fptr);
	}
	for(i=0;i<pr.nLabel;i++)
	{
		pr.label[i]= readLabelRec(fptr);
	}

	return(pr);

}/*end readProcRec*/

/*-----------------------------------------------------------------*/

struct StackFrameRec readStackFrameRec(FILE *fptr)
{
	struct StackFrameRec sfr;
	int i;
	i = fread(debugbytes,sizeof(U1),SIZE_RETREC,fptr);
	if(i<SIZE_RETREC)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading stackframe record");
	}

	sfr.text = bytecodeToQWord(&(debugbytes[0]));
	sfr.fpOffset = bytecodeToDWord(&(debugbytes[8]));
	sfr.line = bytecodeToDWord(&(debugbytes[12]));

	return(sfr);

}/*end readStackFramerec*/

/*-----------------------------------------------------------------*/

struct LabelRec readLabelRec(FILE *fptr)
{
	struct LabelRec lr;
	int i;
	i = fread(debugbytes,sizeof(U1),SIZE_LBLREC,fptr);
	if(i<SIZE_LBLREC)
	{
		FCLOSE(fptr);
		FATAL_ERROR1("error reading label record");
	}

	lr.text = bytecodeToQWord(&(debugbytes[0]));
	lr.address = bytecodeToQWord(&(debugbytes[8]));
	lr.line = bytecodeToDWord(&(debugbytes[16]));

	return(lr);

}/*end readLabelRec*/

/*-----------------------------------------------------------------*/

/*void testDriver(int argc, char *argv[])
{
	struct HeaderRec hr;
	FILE *fptr;

	if(argc==1)
	{
		printf("file name must be specified");
		return;
	}

	fptr = fopen(argv[1],"rb");
	if(fptr==NULL)
	{
		printf("cannot open %s\n",argv[1]);
		return;
	}

	populateDebugData(&hr, &debugData,fptr);

	printf("\n\nHEADER->\n");
	printf("\tmagic=%lX\n",hr.magic);
	printf("\tbytes in symbol table %lu\n",hr.szSymTbl);
	printf("\tbytes in string table %lu\n",hr.szStrTbl);
	printf("\tbytes in bytecode %lu\n",hr.szByteCode);

	printDebugData(&debugData);
	return;

}*//*end main*/
