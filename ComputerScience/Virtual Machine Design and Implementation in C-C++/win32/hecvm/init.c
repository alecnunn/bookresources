/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ init.c - loads bytecode off of disk and into physical memory    +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define	APPLICATION		0xDEED	/* identifies an executable file (magic number) */

/*#define DBG_INIT		1*/

#ifdef DBG_INIT
#define DBG_INIT0(str);				printf("initHEC(): "); printf(str);
#define DBG_INIT1(str,arg1);		printf("initHEC(): "); printf(str,arg1);
#define DBG_INIT2(str,arg1,arg2);	printf("initHEC(): "); printf(str,arg1,arg2);
#define PRINT_SYM_TBL();			printDebugData(&debugData);	
#else
#define DBG_INIT0(str);
#define DBG_INIT1(str,arg1);
#define DBG_INIT2(str,arg1,arg2); 
#define PRINT_SYM_TBL();	
#endif	

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ public prototypes                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

U1 initHEC(struct CmdLine cl);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ function definitions                                              +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	init execution environment ( same way BIOS would boot a PC )
	1) open executable and read header and debug data
	2) validate header data
	3) user header data to allocate RAM and init registers ( 4GB limit imposed here )
	4) load bytecode into RAM[]
*/

U1 initHEC(struct CmdLine cl)
{
	U8 fileSize;			/* size of bytecode file in bytes */
	FILE *fptr;				/* pointer to open file */

	struct HeaderRec hr;	/*header read from file*/

	U8 bytecodeStart;		/*byte offsets into executable file*/
	U8 bytecodeEnd;

	U8 exec_bytes;			/* bytes in bytecode section of RAM[] */
	U8 total_bytes;			/* total bytes allocated for RAM[] */
	U8 free_bytes;			/* available physical RAM */

	U8 index;				/*index into file*/
	U8 i;					/*index into RAM[]*/
	int read_byte;		

	/*1) open executable and read header and debug data */

	fileSize = getFileSize(cl.binaryFile);

	if(fileSize == 0)
	{
		ERROR0_LVL2("initHEC(): executable has zero size");
		return(FALSE);	
	}

	if((fptr=fopen(cl.binaryFile,"rb"))== NULL)
	{
		ERROR1_LVL2("initHEC(): cannot open %s",cl.binaryFile);
		return(FALSE);
	}

	DBG_INIT0("populate symbol table---------------------\n");
	populateDebugData(&hr,&debugData,fptr);
	PRINT_SYM_TBL();
	DBG_INIT0("end symbol table population---------------\n");

	DBG_INIT0("finished reading header and symbol table\n");
	DBG_INIT1("magic number=%X\n",hr.magic);
	DBG_INIT1("symbol table size=%lu\n",hr.szSymTbl);
	DBG_INIT1("string table size=%lu\n",hr.szStrTbl);
	DBG_INIT1("bytecode table size=%lu\n",hr.szByteCode);

	/*2) validate header data */

	if(hr.magic!=(U2)APPLICATION)
	{
		ERROR0_LVL2("initHEC(): file not HEC executable");
		FCLOSE(fptr);
		return(FALSE);
	}

	if(hr.szByteCode==0)
	{
		ERROR1_LVL2("initHEC(): no bytecode in %s",cl.binaryFile);
		FCLOSE(fptr);
		return(FALSE);
	}

	/*3) user header data to allocate RAM and init registers*/
	
	bytecodeStart = SIZE_HEADER + hr.szSymTbl + hr.szStrTbl;
	bytecodeEnd = (bytecodeStart+hr.szByteCode)-1;

	DBG_INIT1("bytecodeStart in file=%lu\n",bytecodeStart);
	DBG_INIT1("bytecodeEnd in file=%lu\n",bytecodeEnd);

	exec_bytes = ( bytecodeEnd - bytecodeStart ) + 1;

	total_bytes = exec_bytes+
		         (cl.heapRequest*1024)+
				 (cl.stackRequest*1024);

	free_bytes = getAvailableMemory();

	if(total_bytes > free_bytes)
	{
		ERROR0_LVL2("initHEC(): not enough memory to init runtime");
		FCLOSE(fptr);
		return(FALSE);
	}

	DBG_INIT1("available physical RAM= %lu\n", free_bytes );
    DBG_INIT1("file size= %lu\n", fileSize );
	DBG_INIT1("size of bytecode in file= %lu\n", exec_bytes );
	DBG_INIT1("heap bytes= %lu\n", cl.heapRequest*1024 );
	DBG_INIT1("stack bytes= %lu\n", cl.stackRequest*1024 );
	DBG_INIT1("total required bytes= %lu\n", total_bytes );

	/* we're legal, so can allocate RAM  */

	printf("allocating %lu bytes\n",total_bytes); 

	/* U4 size limit is imposed here!!!! */

	RAM = ( U1* )malloc((U4)total_bytes); 

	/*
		now set up registers
		
		memory layout:  [0 -> $BE] [$HS -> $HE] [$SS -> $TOP]
	*/

	DBG_INIT0("setting up registers\n");

	R[$IP]=0;
	R[$SP]=total_bytes-1;
	R[$FP]=0;
	R[$BE]=exec_bytes-1;

	R[$HS]=exec_bytes;
	R[$HE]=exec_bytes+(cl.heapRequest*1024)-1;

	R[$SS]=exec_bytes+(cl.heapRequest*1024);
	R[$TOP]=total_bytes-1; 

	appMetaData.fileSize = fileSize;
	appMetaData.szSymTbl=hr.szSymTbl;
	appMetaData.szStrTbl=hr.szStrTbl;
	appMetaData.szByteCode=hr.szByteCode;
	appMetaData.bCodeFileStart=bytecodeStart;
	appMetaData.bCodeFileEnd=bytecodeEnd;
	appMetaData.heapSize = cl.heapRequest*1024;
	appMetaData.stackSize= cl.stackRequest*1024;
	appMetaData.ram = total_bytes;

	/*4) load bytecode into RAM[] */

	/*load bytecode into RAM*/

	rewind(fptr);	/* rewind to start of file */

	/* 
	note: fseek starts index at zero, so our scheme is correct
	note: (U4) cast, limits us to 4GB file
	*/

	if(fseek(fptr,(U4)bytecodeStart,SEEK_SET))
	{
		ERROR0_LVL2("initHEC(): could not find bytecode start");
		return(FALSE);
	}

	i=0;
	for(index=bytecodeStart;index<=bytecodeEnd;index++)
	{
		read_byte = fgetc(fptr);
		if(read_byte==EOF)
		{
			ERROR0_LVL2("initHEC(): error reading bytecode file");
			FCLOSE(fptr);
			return(FALSE);
		}
		RAM[i]=(U1)read_byte;
		i++;
	}

	FCLOSE(fptr);
	return(TRUE);

}/*end initHEC*/
