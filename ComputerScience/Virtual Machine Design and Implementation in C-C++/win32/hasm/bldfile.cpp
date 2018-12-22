/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ bldfile.cpp -	construct header and output file                    +
+               append bytecode from pass2 on to end                +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define PASS3_DEBUG		1*/

#ifdef	PASS3_DEBUG
#define	PASS3_DEBUG0(arg);						printf(arg);
#define PASS3_DEBUG1(arg1,arg2);				printf(arg1,arg2);
#define PASS3_DEBUG2(arg1,arg2,arg3);			printf(arg1,arg2,arg3);
#else
#define	PASS3_DEBUG0(arg);
#define PASS3_DEBUG1(arg1,arg2);
#define PASS3_DEBUG2(arg1,arg2,arg3);
#endif

/*byte sizes of records in debug DB*/

#define SIZE_GLOBREC	37
#define SIZE_PROCREC	25
#define SIZE_RETREC		16
#define SIZE_ARGREC		16
#define SIZE_LOCREC		16
#define SIZE_LBLREC		20


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class BuildFile
{
	CommandLine *cmdLine;
	SymbolTable *symTbl;
	StringTable *strTbl;

	FILE *ofptr;			/*pointer to output file*/
	FILE *afptr;			/*pointer to temporary bytecode file*/

	char *ofile;			/*pointer to output file name*/
	char *afile;			/*pointer to temporary file name*/

	char ofBuffer[BUFFER_SIZE];		/*buffer for output file*/
	int iOFChar;					/*index into ofBuffer*/

	U1 output[8];		/*holds bytes to write to output file*/

	/*manage output buffer*/
	void putByteOutBuff(U1 byte);
	void flushOutBuff();

	/*these functions convert a number to big endian and write to output*/
	void commit2Bytes(U2 val);
	void commit4Bytes(U4 val);
	void commit8Bytes(U8 val);

	public:
	
	BuildFile(CommandLine *cptr, StringTable *st, SymbolTable *syt);
	~BuildFile();
	void buildFileFormat();

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BuildFile::BuildFile(CommandLine *cptr, StringTable *st, SymbolTable *syt)
{ 
	cmdLine = cptr;
	strTbl = st;
	symTbl = syt;

	ofile = (*cptr).outputFile;
	afile = (*cptr).tempFile;

	if(ofile==NULL)
	{
		ERROR0("BuildFile::BuildFile(): output file name has not been specified\n");
		FATAL_ERROR();
	}
	if(afile==NULL)
	{
		ERROR0("BuildFile::BuildFile(): temporary file's name has not been specified\n");
		FATAL_ERROR();
	}
	
	ofptr = fopen(ofile,"wb");
	afptr = fopen(afile,"rb");
	
	if(ofptr==NULL)
	{
		ERROR1("BuildFile::BuildFile(): could not open output file->%s\n",ofile);
		FATAL_ERROR();
	}
	if(afptr==NULL)
	{
		ERROR1("BuildFile::BuildFile(): could not open temporary file->%s\n",afile);
		FATAL_ERROR();
	}
	iOFChar = 0;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

BuildFile::~BuildFile()
{
	flushOutBuff();

	if(fclose(ofptr)){ ERROR1("BuildFile::BuildFile(): problem closing output file ->%s\n",ofile); }
	if(fclose(afptr)){ ERROR1("BuildFile::BuildFile(): problem closing temp file ->%s\n",afile); }
	
	return;

}/*end finalizePass3*/

/*-----------------------------------------------------------------*/

void BuildFile::putByteOutBuff(U1 byte)
{
	int nbytes;

	nbytes = 0;

	ofBuffer[iOFChar] = byte;
	iOFChar++;

	if(iOFChar==BUFFER_SIZE)
	{
		nbytes = fwrite(ofBuffer,sizeof(U1),BUFFER_SIZE,ofptr);
		if(nbytes!=BUFFER_SIZE)
		{
			ERROR1("BuildFile::putByteOutBuff(): error during fwrite to %s\n",ofile);
			FATAL_ERROR();
		}
		iOFChar = 0;
	}
	return;

}/*end putByteOutBuff*/

/*-----------------------------------------------------------------*/

void BuildFile::flushOutBuff()
{
	int nbytes;

	nbytes = 0;

	if(iOFChar>0)
	{
		PASS3_DEBUG1("BuildFile::flushOutBuff(): trying to flush %lu bytes\n",iOFChar);
		nbytes = fwrite(ofBuffer,sizeof(U1),iOFChar,ofptr);
		if(nbytes!=iOFChar)
		{
			ERROR1("BuildFile::flushOutBuff(): only flushed %lu bytes\n",nbytes);
			ERROR1("BuildFile::flushOutBuff(): error during fwrite to %s\n",ofile);
			FATAL_ERROR();
		}
		iOFChar = 0;	
	}
	return;

}/*end flushOutBuff*/

/*-----------------------------------------------------------------*/

void BuildFile::commit2Bytes(U2 val)
{
	wordToBytecode(val,output);
	putByteOutBuff(output[0]);
	putByteOutBuff(output[1]);
	return;

}/*end commit2Bytes*/

/*-----------------------------------------------------------------*/

void BuildFile::commit4Bytes(U4 val)
{
	dwordToBytecode(val,output);
	putByteOutBuff(output[0]);
	putByteOutBuff(output[1]);
	putByteOutBuff(output[2]);
	putByteOutBuff(output[3]);
	return;

}/*end commit4Bytes*/
/*-----------------------------------------------------------------*/

void BuildFile::commit8Bytes(U8 val)
{
	qwordToBytecode(val,output);
	putByteOutBuff(output[0]);
	putByteOutBuff(output[1]);
	putByteOutBuff(output[2]);
	putByteOutBuff(output[3]);
	putByteOutBuff(output[4]);
	putByteOutBuff(output[5]);
	putByteOutBuff(output[6]);
	putByteOutBuff(output[7]);
	return;

}/*end commit8Bytes*/

/*-----------------------------------------------------------------*/

void BuildFile::buildFileFormat()
{
	U8 size_symtbl;
	U8 size_strtbl;
	U8 size_bc;
	U8 i;
	U8 j;
	int ch;

	size_symtbl = 0;
	size_strtbl = 0;
	size_bc     = 0;

	/* 1) determine size of symbol table*/
	
	size_symtbl = size_symtbl + 8; /*table of contents */
	size_symtbl = size_symtbl + SIZE_GLOBREC*((*symTbl).iGlobVar); 
	size_symtbl = size_symtbl + SIZE_PROCREC*((*symTbl).iProc); 

	for(i=0;i<(*symTbl).iProc;i++)
	{
		size_symtbl = size_symtbl + SIZE_RETREC*(((*symTbl).proc[i]).nRet);
		size_symtbl = size_symtbl + SIZE_ARGREC*(((*symTbl).proc[i]).iArg);
		size_symtbl = size_symtbl + SIZE_LOCREC*(((*symTbl).proc[i]).iLocal);
		size_symtbl = size_symtbl + SIZE_LBLREC*(((*symTbl).proc[i]).iLabel);
	}

	/* 2) determine size of string table*/

	size_strtbl = (*strTbl).iStr;

	/* 3) determine size of bytecode */

	size_bc = getFileSize(afile);

	/* 4) commit header to output file */

	putByteOutBuff(0xDE); /*magic word to output file */
	putByteOutBuff(0xED);

	if((*cmdLine).omitDebugData==TRUE)
	{
		commit8Bytes((U8)0);
		commit8Bytes((U8)0);
		commit8Bytes(size_bc);

		ch = fgetc(afptr);
		while(ch!=EOF)
		{
			putByteOutBuff((U1)ch);
			ch = fgetc(afptr);
		}
		return;
	}

	commit8Bytes(size_symtbl);
	commit8Bytes(size_strtbl);
	commit8Bytes(size_bc);

	/* 5) commit tables of contents (# globals and # procs) */

	commit4Bytes((*symTbl).iGlobVar);
	commit4Bytes((*symTbl).iProc);

	/* 6) commit global vars to output file */

	for(i=0;i<(*symTbl).iGlobVar;i++)
	{
		commit8Bytes(((*symTbl).globVar[i]).text);
		putByteOutBuff(((*symTbl).globVar[i]).dType);
		commit8Bytes(((*symTbl).globVar[i]).len);
		commit8Bytes(((*symTbl).globVar[i]).size);
		commit8Bytes(-((S8)((*symTbl).globVar[i]).offset));
		commit4Bytes(((*symTbl).globVar[i]).line);
	}

	/* 7) commit procedures to output file */

	for(i=0;i<(*symTbl).iProc;i++)
	{
		commit8Bytes(((*symTbl).proc[i]).text);
		commit8Bytes(((*symTbl).proc[i]).address);
		commit4Bytes(((*symTbl).proc[i]).line);
		putByteOutBuff(((*symTbl).proc[i]).nRet);
		putByteOutBuff(((*symTbl).proc[i]).iArg);
		putByteOutBuff(((*symTbl).proc[i]).iLocal);
		commit2Bytes(((*symTbl).proc[i]).iLabel);

		for(j=0;j<((*symTbl).proc[i]).nRet;j++)
		{
			commit8Bytes((((*symTbl).proc[i]).ret).text);
			commit4Bytes((((*symTbl).proc[i]).ret).fpOffset);
			commit4Bytes((((*symTbl).proc[i]).ret).line);
		}
		for(j=0;j<((*symTbl).proc[i]).iArg;j++)
		{
			commit8Bytes((((*symTbl).proc[i]).arg[j]).text);
			commit4Bytes((((*symTbl).proc[i]).arg[j]).fpOffset);
			commit4Bytes((((*symTbl).proc[i]).arg[j]).line);
		}
		for(j=0;j<((*symTbl).proc[i]).iLocal;j++)
		{
			commit8Bytes((((*symTbl).proc[i]).local[j]).text);
			commit4Bytes((((*symTbl).proc[i]).local[j]).fpOffset);
			commit4Bytes((((*symTbl).proc[i]).local[j]).line);
		}
		for(j=0;j<((*symTbl).proc[i]).iLabel;j++)
		{
			commit8Bytes((((*symTbl).proc[i]).label[j]).text);
			commit8Bytes((((*symTbl).proc[i]).label[j]).address);
			commit4Bytes((((*symTbl).proc[i]).label[j]).line);
		}
	}

	/* 8) commit string table to output file */

	for(i=0;i<(*strTbl).iStr;i++){ putByteOutBuff((*strTbl).text[i]); }

	/* 9) append bytecode in temp to output file */

	ch = fgetc(afptr);
	while(ch!=EOF)
	{
		putByteOutBuff((U1)ch);
		ch = fgetc(afptr);
	}

	return;

}/*end buildFileFormat*/