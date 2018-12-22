/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ dbgcmd.c - implements command-line debugging                      +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define MAX_DBG_CHARS	256
#define BAD_RANGE		-1
#define VALID_RANGE		1
#define DEBUG_ON		1
#define DEBUG_OFF		0
#define BAD_CMD();		printf("%s is invalid, type \'?\' for help\n",debugLine);	

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ globals                                                           +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char debugLine[MAX_DBG_CHARS];

struct DbgRange
{
	U8 start;
	U8 stop;
	int ok;
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototype                                                  +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void readDbgCmd();
void printDbgInstr(U8 addr);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private prototypes                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printHelp();
void toggleDebugging(int state);
void printAppData();
struct DbgRange parseRangeStr();
void dumpRamRng(U8 strt, U8 stp);

void searchForStr();
U1 cmpRAMStr(U1 *ptr1, U1 *ptr2, U4 len);

void getIdentifierMetaData();
U1 searchForGlobal(char *str);
U1 matchProcName(char *str, U4 ind);
U1 searchCurrentProc(char *str, U4 ind);

S8 showCurrentProc(U1 display); 

void printRawBytes(U1 *ptr, U1 len);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void readDbgCmd()
{
	int size;

	printf("\n");

	while(1)
	{
		printf("debug>");
		scanf("%s",debugLine);

		size = strlen(debugLine);
		if(size>=MAX_DBG_CHARS)
		{ 
			ERROR0_LVL2("error: input command out of bounds"); 
		}

		switch(debugLine[0])
		{
			case '?':
			{
				if(debugLine[1]=='\0'){ printHelp(); }
				else{ BAD_CMD(); }

			}break;
			case 'f':
			case 'F':
			{
				if(debugLine[1]=='\0'){ printAppData(); }
				else{ BAD_CMD(); }

			}break;
			case 'q':
			case 'Q':
			{
				if(debugLine[1]=='\0')
				{ 
					toggleDebugging(DEBUG_OFF); 
					return; 
				}
				else{ BAD_CMD(); }

			}break;
			case 'd':
			case 'D':
			{
				if(debugLine[1]=='\0')
				{ 
					struct DbgRange rng;
					rng.ok=VALID_RANGE;
					rng = parseRangeStr();
					if(rng.ok!=BAD_RANGE)
					{
						dumpRamRng(rng.start,rng.stop);
					}
				}
				else{ BAD_CMD(); }

			}break;
			case 'S':
			case 's':
			{
				if(debugLine[1]=='\0'){ searchForStr();	}
				else{ BAD_CMD(); }

			}break;
			case 'L':
			case 'l':
			{
				if(debugLine[1]=='\0'){ getIdentifierMetaData(); }
				else{ BAD_CMD(); }

			}break;
			case 'P':
			case 'p':
			{
				if(debugLine[1]=='\0'){ showCurrentProc(TRUE); }
				else{ BAD_CMD(); }

			}break;
			case 'R':
			case 'r':
			{
				if((debugLine[1]=='i')&&(debugLine[2]=='\0'))
				{ 
					printBasicRegisters();
					printGeneralRegisters();
				}
				else if((debugLine[1]=='f')&&(debugLine[2]=='\0'))
				{ 
					printFloatRegisters(); 
				}
				else if((debugLine[1]=='d')&&(debugLine[2]=='\0'))
				{ 
					printDoubleRegisters(); 
				}
				else{ BAD_CMD(); }

			}break;
			case 'T':
			case 't':
			{
				if(debugLine[1]=='\0'){ return; }
				else{ BAD_CMD(); }

			}break;
			default:{ BAD_CMD(); }

		}/*end switch*/

		if(fflush(stdin)){ ERROR0_LVL2("error: could not flush STDIN"); }

	}/*end while*/
	
	return;

}/*end readDbgCmd*/

/*-----------------------------------------------------------------*/

void printHelp()
{
	printf("\nHEng debug mode help\n");
	printf("----------------------\n");
	printf("?\ndisplay this help screen\n\n");
	printf("d start stop\ndump bytes of memory in the range [start, stop]\n\n");
	printf("f\ndisplay executable file information\n\n");
	printf("l string\nquery meta-data for a program identifier\n\n");
	printf("p\nquery current procedure\n\n");
	printf("q\nexit debug mode and return to production mode\n\n");
	printf("ri\ndisplay integer registers\n\n");
	printf("rf\ndisplay float registers\n\n");
	printf("rd\ndisplay double registers\n\n");
	printf("s start stop string\nsearch memory range [start, stop] for string\n\n");
	printf("t\ntrace through next instruction\n\n");
	return;

}/*end printHelp*/

/*-----------------------------------------------------------------*/

void toggleDebugging(int state)
{
	if(state==DEBUG_ON){ debug = TRUE; }
	else if(state==DEBUG_OFF){ debug = FALSE; }
	else
	{ 
		ERROR0_LVL2("invalid state change request");
		debug = TRUE;
	}
	return;

}/*end toggleDebugging*/

/*-----------------------------------------------------------------*/

/*using global data from cmdline.c*/

void printAppData()
{
	printf("\nEXECUTABLE FILE----------------------\n");
	printf("file name->%s\n",appMetaData.fileName);
	printf("file size->"); pU8(appMetaData.fileSize);
	printf("\nfile symbol table size->"); pU8(appMetaData.szSymTbl);
	printf("\nfile string table size->"); pU8(appMetaData.szStrTbl);
	printf("\nfile bytecode size->"); pU8(appMetaData.szByteCode);
	printf("\nbytecode offset begin in file->"); pU8(appMetaData.bCodeFileStart);
	printf("\nbytecode offset end in file->"); pU8(appMetaData.bCodeFileEnd);
	printf("\n\nMEMORY LAYOUT------------------------\n");
	printf("heap allocated->"); pU8(appMetaData.heapSize);
	printf("\nstack allocated->"); pU8(appMetaData.stackSize);
	printf("\ntotal ram->"); pU8(appMetaData.ram);
	printf("\n\n");
	return;

}/*end printFileData*/

/*-----------------------------------------------------------------*/

struct DbgRange parseRangeStr()
{
	struct DbgRange rng;
	int ret;

	ret = rU8(&rng.start);
	if((ret==EOF)||(ret==0))
	{
		printf("bad start range\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}

	ret = rU8(&rng.stop);
	if((ret==EOF)||(ret==0))
	{
		printf("bad end range\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}

	if((rng.start<0)||(rng.stop<0))
	{
		printf("negative range values not allowed\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}
	else if(rng.start > rng.stop)
	{
		printf(" ( start > stop ) not allowed\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}

	if(rng.start > R[$TOP])
	{
		printf("range start value is greater than $TOP\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}

	if(rng.stop > R[$TOP])
	{
		printf("range stop value is greater than $TOP\n");
		rng.ok=BAD_RANGE;
		return(rng);
	}

	return(rng);

}/*end parseRangeStr*/

/*-----------------------------------------------------------------*/

void dumpRamRng(U8 strt, U8 stp)
{
	U8 i;
	U8 j;
	j=0;
	printf("\n");
	for(i=strt;i<=stp;i++)
	{ 
		PRINT_MEM(i); printf("\t");
		if( (RAM[i]>32)&&(RAM[i]<127) )
		{
			printf("ASCII=%c ",RAM[i]);
		}
		else
		{
			switch(RAM[i])
			{
				case ' ':{ printf("ASCII=<SP>"); }break;
				case '\n':{ printf("ASCII=<TAB>");}break;
				case '\r':{ printf("ASCII=<CR>");}break;
				case '\t':{ printf("ASCII=<NL>");}break;
				default:{ printf("ASCII= control char(%lu)",RAM[i]); }
			}
		}
		printf("\n");
		j++;
	}
	printf("\n");
	return;

}/*end dumpRamRng*/

/*-----------------------------------------------------------------*/

void searchForStr()
{
	int ret;
	struct DbgRange rng;
	U8 i;
	U4 size;
	U1 found;

	found = FALSE;

	rng.ok = VALID_RANGE;
	rng = parseRangeStr();
	if(rng.ok==BAD_RANGE){ return; }

	ret = scanf("%s",debugLine);
	if((ret==EOF)||(ret==0))
	{
		printf("bad search string entered\n");
		return;
	}

	/* search rng.start->rng.stop for debugLine */
		
	size = strlen(debugLine);
	if(size==0)
	{
		printf("bad search string entered\n");
		return;
	}

	for(i=rng.start;i<=rng.stop;i++)
	{
		if(RAM[i]==debugLine[0])
		{
			ret = cmpRAMStr(debugLine,&RAM[i],size);
			if(ret==TRUE)
			{
				printf("Match-> address="); 
				pU8(i);
				printf("\n");
				found = TRUE;
			}
		}
	}

	if(found==FALSE)
	{ 
		printf("no such string in memory range ");
		pU8(rng.start);
		printf("->");
		pU8(rng.stop);
		printf("\n");
	}
	return;

}/*end searchForStr*/

/*-----------------------------------------------------------------*/

U1 cmpRAMStr(U1 *ptr1, U1 *ptr2, U4 len)
{
	U4 i;
	for(i=0;i<len;i++)
	{
		if(ptr1[i]!=ptr2[i]){ return(FALSE);}
	}
	return(TRUE);

}/*end cmpRAMStr*/

/*-----------------------------------------------------------------*/

void getIdentifierMetaData()
{
	int ret;
	S8 proc_ind;

	ret = scanf("%s",debugLine);
	if((ret==EOF)||(ret==0))
	{
		printf("bad identifier entered\n");
		return;
	}

	/* look through globals */

	ret = searchForGlobal(debugLine);
	if(ret==TRUE){ return; }

	/* if not in globals get current proc */

	proc_ind = showCurrentProc(FALSE);
	if(proc_ind==-1)
	{
		printf("symbol not global, currently outside all procedures\n");
		return;
	}
	
	ret = matchProcName(debugLine,(U4)proc_ind);
	if(ret==TRUE){ return; }

	/* if not proc name, look for ret, args, locals, and labels */
	
	ret = searchCurrentProc(debugLine,(U4)proc_ind);
	if(ret==TRUE){ return; }

	printf("could not resolve symbol\n");

	return;

}/*end getIdentifierMetaData*/

/*-----------------------------------------------------------------*/

U1 searchForGlobal(char *str)
{
	U4 i;
	U8 j;
	U8 addr;
	char *temp;

	for(i=0;i<debugData.contents.nGlobVarRec;i++)
	{
		temp = &(debugData.strTbl[debugData.gvRec[i].text]);
		if(strcmp(str,temp)==0)
		{
			printf("global variable->%s\n",str);
			printf("type->%s\n",globSz[debugData.gvRec[i].dType]);
			printf("length->"); pU8(debugData.gvRec[i].len); printf("\n");
			printf("total size->"); pU8(debugData.gvRec[i].size); printf("\n");
			printf("line->"); pU4(debugData.gvRec[i].line); printf("\n");
			printf("values:\n");
			addr = R[$TOP] - debugData.gvRec[i].offset;
			for(j=0;j<debugData.gvRec[i].size;j++)
			{
				printf("\t"); pU8(j+1); printf(") address-> ");
				pU8(addr+j);
				printf(" value->%u\n",RAM[addr+j]);
			}
			return(TRUE);
		}
	}
	return(FALSE);

}/*end searchForGlobal*/

/*-----------------------------------------------------------------*/

U1 matchProcName(char *str, U4 ind)
{
	char *procname;
	procname = &debugData.strTbl[debugData.pRec[ind].text];
	if(strcmp(str,procname)==0)
	{
		printf("function->%s\n",str);
		printf("address->"); pU8(debugData.pRec[ind].address); printf("\n");
		printf("line="); pU4(debugData.pRec[ind].line); printf("\n");
		return(TRUE);
	}
	return(FALSE);

}/*end searchForProc*/

/*-----------------------------------------------------------------*/

U1 searchCurrentProc(char *str, U4 ind)
{
	U4 i;
	char *temp;
	U8 addr;
	U1 narg;
	U1 nloc;
	U2 nlbl;
	narg = debugData.pRec[ind].nArg;
	nloc = debugData.pRec[ind].nLocal;
	nlbl = debugData.pRec[ind].nLabel;
	
	if(debugData.pRec[ind].nRet==1)
	{
		temp = &(debugData.strTbl[debugData.pRec[ind].ret.text]);
		if(strcmp(str,temp)==0)
		{
			printf("return value->%s\n",str);
			printf("address->");
			addr = R[$FP]+debugData.pRec[ind].ret.fpOffset;
			pU8(addr);
			printf("\n");
			return(TRUE);
		}
	}
	for(i=0;i<narg;i++)
	{
		temp = &(debugData.strTbl[debugData.pRec[ind].arg[i].text]);
		if(strcmp(str,temp)==0)
		{
			printf("procedure argument->%s\n",str);
			printf("address->");
			addr = R[$FP]+debugData.pRec[ind].arg[i].fpOffset;
			pU8(addr);
			printf("\n");
			return(TRUE);
		}
	}
	for(i=0;i<nloc;i++)
	{
		temp = &(debugData.strTbl[debugData.pRec[ind].local[i].text]);
		if(strcmp(str,temp)==0)
		{
			printf("local storage->%s\n",str);
			printf("address->");
			addr = R[$FP]+debugData.pRec[ind].local[i].fpOffset;
			pU8(addr);
			printf("\n");
			return(TRUE);
		}
	}
	for(i=0;i<nlbl;i++)
	{
		temp = &(debugData.strTbl[debugData.pRec[ind].label[i].text]);
		if(strcmp(str,temp)==0)
		{
			printf("label->%s\n",str);
			printf("address->");
			pU8(debugData.pRec[ind].label[i].address);
			printf("\n");
			return(TRUE);
		}
	}
	return(FALSE);

}/*end searchCurrentProc*/

/*-----------------------------------------------------------------*/

S8 showCurrentProc(U1 display)
{
	U4 i;
	U4 nprocs;
	nprocs = debugData.contents.nProcRec;
	for(i=0;i<nprocs;i++)
	{
		if(((U8)R[$IP]) > debugData.pRec[nprocs-1].address )
		{
			if(display==TRUE)
			{
				printf("current proc->%s\n",&(debugData.strTbl[debugData.pRec[nprocs-1].text]));
				printf("address->"); pU8(debugData.pRec[nprocs-1].address);
				printf("\n");
				printf("line->"); pU4(debugData.pRec[nprocs-1].line);
				printf("\n");
			}
			return(nprocs-1);
		}
		else if((debugData.pRec[i].address <= ((U8)R[$IP]))&&
			    (debugData.pRec[i+1].address > ((U8)R[$IP])))
		{
			if(display==TRUE)
			{
				printf("current proc->%s\n",&(debugData.strTbl[debugData.pRec[i].text]));
				printf("address->"); pU8(debugData.pRec[i].address);
				printf("\n");
				printf("line->"); pU4(debugData.pRec[i].line);
				printf("\n");
			}
			return(i);
		}
	}
	printf("could not find current function\n");
	return(-1);

}/*end showCurrentProc*/

/*-----------------------------------------------------------------*/

void printDbgInstr(U8 addr)
{
	/*display in assembler and in binary*/
	
	U8 current_byte;	/*lookahead from addr*/
	U1 raw[11];			/*buffer to hold binary representation*/

	current_byte = addr;
		
	printf("address->"); pU8(addr); printf("\n");

	switch(RAM[current_byte])
	{
		case LBI:	/* LBI $r1, byte constant  BBB */
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%d\n",(S1)RAM[current_byte]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case LWI:	/* LWI $r1, word constant  BBW */
		{
			S2 *si;

			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf(" %s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];

			si = ((S2*)(&RAM[current_byte-1]));
			pS2(*si); printf("\n");

			printRawBytes(raw,4);

		}break;
		case LDI:	/* LDI $r1, dword constant  BBD */
		{
			S4 *li;

			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];

			li = (S4 *)&RAM[current_byte-3];
			pS4(*li); printf("\n");

			printRawBytes(raw,6);

		}break;
		case LQI:	/* LQI $r1, qword constant  BBQ*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];
			current_byte++;
			raw[6] = RAM[current_byte];
			current_byte++;
			raw[7] = RAM[current_byte];
			current_byte++;
			raw[8] = RAM[current_byte];
			current_byte++;
			raw[9] = RAM[current_byte];

			pS8(*((S8 *)&RAM[current_byte-7]));
			printf("\n");
			printRawBytes(raw,10);	

		}break;
		case LF1I:  /* LF1I $f, float */
		{
			F4 *fi;

			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];

			fi = (F4 *)&RAM[current_byte-3];
			printf("%g\n",*fi);

			printRawBytes(raw,6);

		}break;
		case LF2I:	/* LF2I $d, double */
		{
			F8 *di;

			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];
			current_byte++;
			raw[6] = RAM[current_byte];
			current_byte++;
			raw[7] = RAM[current_byte];
			current_byte++;
			raw[8] = RAM[current_byte];
			current_byte++;
			raw[9] = RAM[current_byte];

			di = (F8 *)&RAM[current_byte-7];
			printf("%g\n",*di);

			printRawBytes(raw,10);	
			
		}break;
		case LAD:	/* LAD $r1, address = BBQ */ 
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			raw[2] = RAM[current_byte];
			current_byte++;
			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];
			current_byte++;
			raw[6] = RAM[current_byte];
			current_byte++;
			raw[7] = RAM[current_byte];
			current_byte++;
			raw[8] = RAM[current_byte];
			current_byte++;
			raw[9] = RAM[current_byte];

			pU8(*((U8 *)&RAM[current_byte-7]));
			printf("\n");
			printRawBytes(raw,10);	
			
		}break;
		case LAI:   /* LAI $r1, $r2, qword     BBBQ*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
			current_byte++;

			raw[3] = RAM[current_byte];
			current_byte++;
			raw[4] = RAM[current_byte];
			current_byte++;
			raw[5] = RAM[current_byte];
			current_byte++;
			raw[6] = RAM[current_byte];
			current_byte++;
			raw[7] = RAM[current_byte];
			current_byte++;
			raw[8] = RAM[current_byte];
			current_byte++;
			raw[9] = RAM[current_byte];
			current_byte++;
			raw[10] = RAM[current_byte];

			pS8(*((S8 *)&RAM[current_byte-7]));
			printf("\n");
			printRawBytes(raw,11);	

		}break;
		case LB:	/* LB $r1,$r2	BBB */
		case LW:
		case LD:
		case LQ:
		case SB:	/* SB $r1,$r2   BBB*/ 
		case SW:
		case SD:
		case SQ:
		case MOV:	/* MOV $r1, $r2    BBB */
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);	

		}break;
		case LF1:	/* LF1  $f, $r */
		case SF1:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);	

		}break;
		case LF2:	/* LF2  $d, $r */
		case SF2:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);	

		}break;
		case MOVF:	/*MOVF  $f1, $f2*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rf_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);	

		}break;
		case MOVD:	/*MOVD  $d1, $d2*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rd_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);	

		}break;
		case PUSHB:	/* PUSHB $r1  BB */
		case PUSHW:
		case PUSHD:
		case PUSHQ:
		case POPB:
		case POPW:
		case POPD:
		case POPQ:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];

			printRawBytes(raw,2);

		}break;
		case PUSHF1:	/* PUSHF1 $f */
		case POPF1:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];

			printRawBytes(raw,2);

		}break;
		case PUSHF2:	/* PUSHF2 $d */
		case POPF2:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];

			printRawBytes(raw,2);

		}break;
		case JMP:	/* JMP $r1 */
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];

			printRawBytes(raw,2);

		}break;
		case JE:	/*JE $r1, $r2, $r3 BBBB */
		case JNE:
		case SLT:
		case AND:	/* AND $r1, $r2, $r3 */
		case OR:
		case XOR:
		case BT:
		case SRA:  /*SRA  $r1, $r2, $r3   BBBB */
		case SRL:
		case SL:
		case ADD: /* ADD $r1, $r2, $r3  */
		case SUB:
		case MULT:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[3] = RAM[current_byte];

			printRawBytes(raw,4);	

		}break; 
		case INT: /* INT #vector	BB */
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%u\n",RAM[current_byte]);
			raw[1] = RAM[current_byte];

			printRawBytes(raw,2);

		}break;	
		case EI:			/* EI	B */
		case DI:
		case HALT:
		case NOP:
		{
			printf("%s\n",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];

			printRawBytes(raw,1);

		}break;
		case NOT:  /* NOT $r1, $r2 */
		case BS:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
		
			printRawBytes(raw,3);

		}break;
		case DIV:  /* DIV $r1, $r2, $r3, $r4 */
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[3] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[4] = RAM[current_byte];

			printRawBytes(raw,5);		

		}break;
		case CAST_IF: /* CAST_IF $r, $f  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rf_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case CAST_ID: /* CAST_ID $r, $d  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",R_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rd_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case CAST_FI: /* CAST_FI $f, $r  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case CAST_FD: /* CAST_FD $f, $d  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rd_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case CAST_DI: /* CAST_DI $d, $r  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",R_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case CAST_DF: /* CAST_DF $d, $f  BBB*/
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rf_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];

			printRawBytes(raw,3);

		}break;
		case FADD: /* FADD $f1, $f2, $f3    BBBB */
		case FSUB:
		case FMULT:
		case FDIV:
		case FSLT:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rf_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rf_STR[RAM[current_byte]]);
			raw[3] = RAM[current_byte];

			printRawBytes(raw,4);	

		}break;
		case DADD: /* DADD $d1, $d2, $d3    BBBB */
		case DSUB:
		case DMULT:
		case DDIV:
		case DSLT:
		{
			printf("%s ",I_Set[RAM[current_byte]]);
			raw[0] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[1] = RAM[current_byte];
			current_byte++;

			printf("%s ",Rd_STR[RAM[current_byte]]);
			raw[2] = RAM[current_byte];
			current_byte++;

			printf("%s\n",Rd_STR[RAM[current_byte]]);
			raw[3] = RAM[current_byte];

			printRawBytes(raw,4);

		}break;
		default:
		{
			printf("instruction (%u), not handled\n",RAM[current_byte]);
		}
	}

	printBasicRegisters();
	return;

}/*end void printDbgInstr*/

/*-----------------------------------------------------------------*/

void printRawBytes(U1 *ptr, U1 len)
{
	U1 i;
	for(i=0;i<len;i++){ printf("[%3u]",ptr[i]); }
	printf("\n");
	return;

}/*end printRawBytes*/
