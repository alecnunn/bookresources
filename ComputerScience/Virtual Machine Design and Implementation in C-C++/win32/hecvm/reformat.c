/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  reformat.c - implements the verification stage                   +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros								                            + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define DBG_FMT		1*/

#ifdef DBG_FMT
#define DBG_FMT0(str);				printf("reformat(): "); printf(str);
#define DBG_FMT1(str,arg1);			printf("reformat(): "); printf(str,arg1);
#define DBG_FMT2(str,arg1,arg2);	printf("reformat(): "); printf(str,arg1,arg2);
#define DBG_FMT_PU8(str,arg);		printf("reformat(): "); printf(str); pU8(arg); printf("\n\n");
#define DBG_FMT_PS8(str,arg);		printf("reformat(): "); printf(str); pS8(arg); printf("\n\n");		
#else
#define DBG_FMT0(str);
#define DBG_FMT1(str,arg1);
#define DBG_FMT2(str,arg1,arg2); 
#define DBG_FMT_PU8(str,arg);
#define DBG_FMT_PS8(str,arg);
#endif	

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

U1 reformat();

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private prototypes                                                + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void badIntReg(U1 arg, U8 currentbyte);		
void badFltReg(U1 arg, U8 currentbyte);
void badDblReg(U1 arg, U8 currentbyte);	

/* use when have raw bytes instead of U8 ( would use badAddress )*/

void checkAddress(U1 *arr, U8 cb); 

/* make sure don't have incomplete instruction*/

void checkCurrentByte(U8 cb, U8 end);

/* end of an instruction is allowed to be at end of bytecode*/

void checkEndCurrentByte(U8 cb, U8 end);

/* following three are used only at runtime */

void badAddress(U8 arg, U8 currentbyte);
void badStack(U8 arg, U8 currentbyte);	
void badHeap(U8 arg, U8 currentbyte);	


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
+ function definitions                                              +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	re-format bytecode numeric operands to native 
	( prevents redundant runtime work ).

	This also performs verification to make sure that:
	1) instruction opcodes are valid
	2) register arguments are actual registers of necessary type
	3) immediate address values are in range [ 0 , $TOP ]
	4) Haven't prematurely hit end of bytecode instruction
*/

U1 reformat()
{
	U8 current_byte;
	U8 stop;

	current_byte = 0;
	stop = R[$HS];

	DBG_FMT0("initiating reformat\n\n");
	DBG_FMT_PU8("starting at address->",current_byte);
	DBG_FMT_PU8("stop address->",stop);

	while(current_byte < stop)
	{
		switch(RAM[current_byte])
		{
			case LBI:	/* LBI $r1, byte_constant  BBB */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				DBG_FMT1("byte %d\n\n",(S1)RAM[current_byte]);
				current_byte = current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LWI:	/* LWI $r1, word_constant  BBW */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_WORD(RAM,current_byte);
				DBG_FMT1("word %hd\n\n",*((S2 *)&RAM[current_byte]));
				current_byte = current_byte+2;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LDI:	/* LDI $r1, dword_constant  BBD */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_DWORD(RAM,current_byte);
				DBG_FMT1("dword %ld\n\n",*((S4 *)&RAM[current_byte]));
				current_byte = current_byte+4;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LQI:	/* LQI $r1, qword_constant  BBQ*/
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_QWORD(RAM,current_byte);
				DBG_FMT_PS8("qword ",*((S8 *)&RAM[current_byte]));
				current_byte = current_byte+8;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LF1I:	/* LF1I $f1, float_constant  BBD */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_DWORD(RAM,current_byte);
				DBG_FMT1("float %g\n\n",*((F4 *)&RAM[current_byte]));
				current_byte = current_byte+4;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LF2I:	/* LQI $d1, double_constant  BBQ*/
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_QWORD(RAM,current_byte);
				DBG_FMT1("float %g\n\n",*((F8 *)&RAM[current_byte]));
				current_byte = current_byte+8;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LAD:	/* LAD $r1, address = BBQ */ 
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_QWORD(RAM,current_byte);
				checkAddress(RAM,current_byte);
				DBG_FMT_PU8("address ",*((U8 *)&RAM[current_byte]));
				current_byte = current_byte+8;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LAI:   /* LAI $r1, $r2, qword     BBBQ*/
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				FORMAT_QWORD(RAM,current_byte);
				DBG_FMT_PS8("qword ",*((S8 *)&RAM[current_byte]));
				current_byte = current_byte+8;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LB:	/* LB $r1,$r2	BBB */
			case LW:
			case LD:
			case LQ:
			case SB:	/* SB $r1,$r2   BBB*/ 
			case SW:
			case SD:
			case SQ:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LF1:	/*  LF1  $f1, $r1   BBB */
			case SF1:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case LF2:	/*  LF2  $d1, $r1   BBB */
			case SF2:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case PUSHB:	/* PUSHB $r1  BB */
			case PUSHW:
			case PUSHD:
			case PUSHQ:
			case POPB:	/* POPB $r1   BB*/
			case POPW:
			case POPD:
			case POPQ:
			case JMP:	/* JMP $r1	BB*/
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case PUSHF1: /* PUSHF1 $f */
			case POPF1:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case PUSHF2:  /* PUSHF2 $d */
			case POPF2:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case MOVF:	/* MOVF $f1, $f2 */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case MOVD:  /* MOVF $d1, $d2 */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case MOV:	/* MOV $r1, $r2    BBB */
			case NOT:  
			case BS:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case JE:	/*JE $r1, $r2, $r3	BBBB */
			case JNE:
			case SLT:
			case AND:	/*AND $r1, $r2, $r3	BBBB*/
			case OR:
			case XOR:
			case BT:
			case SRA:	/*SRA  $r1, $r2, $r3  BBBB */
			case SRL:
			case SL:
			case ADD:	/* ADD $r1, $r2, $r3  BBBB*/
			case SUB:
			case MULT:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break; 
			case INT: /* INT #vector	BB */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				DBG_FMT1("vector %d\n\n",RAM[current_byte]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;	
			case EI:	/* EI	B */
			case DI:
			case HALT:
			case NOP:
			{
				DBG_FMT1("opcode %s\n\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case DIV:  /* DIV $r1, $r2, $r3, $r4 */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_IF:	/* CAST_IF  $r, $f */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_ID:	/* CAST_ID  $r, $d */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_FI: /* CAST_FI  $f, $r */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_FD: /* CAST_FD  $f, $d */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_DI: /* CAST_DI  $d, $r */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badIntReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",R_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case CAST_DF: /* CAST_DF  $d, $f */
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			case FADD: /* FADD $f1, $f2, $f3    BBBB */
			case FSUB:
			case FMULT:
			case FDIV:
			case FSLT:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badFltReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rf_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);
				
			}break;
			case DADD: /* DADD $d1, $d2, $d3    BBBB */
			case DSUB:
			case DMULT:
			case DDIV:
			case DSLT:
			{
				DBG_FMT1("opcode %s\n",I_Set[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkCurrentByte(current_byte,stop);

				badDblReg(RAM[current_byte],current_byte);
				DBG_FMT1("operand %s\n\n",Rd_STR[RAM[current_byte]]);
				current_byte++;
				checkEndCurrentByte(current_byte,stop);

			}break;
			default:
			{
				ERROR1_LVL2("reformat(): bad opcode %d",
					        RAM[current_byte]);
				return(FALSE);
			}

		}/*end switch*/

	}/*end while*/

	DBG_FMT0("reformatting complete\n");

	return(TRUE);

}/*end reformat*/

/*-----------------------------------------------------------------*/

/* 
some registers are read-only ( used by runtime system only )
*/

void badIntReg(U1 arg, U8 currentbyte)
{
	if(arg>$R24)
	{ 
		ERROR1_LVL2("badIntReg(): (%d) not integer register",arg);
		
		xmlBegin();
		fprintf(errPtr,"badIntReg(): bad register at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();
	}
	return;

}/*end badIntReg*/

/*-----------------------------------------------------------------*/

void badFltReg(U1 arg, U8 currentbyte)		
{
	if(arg>$F10) 
	{ 
		ERROR1_LVL2("badFltReg(): (%d) not float register",arg);

		xmlBegin();
		fprintf(errPtr,"badFltReg(): bad register at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();
		
		FATAL_ERROR();
	} 
	return;

}/*end badFltReg*/

/*-----------------------------------------------------------------*/

void badDblReg(U1 arg, U8 currentbyte)		
{
	if(arg>$D10) 
	{ 
		ERROR1_LVL2("badDblReg(): (%d) not double register",arg);

		xmlBegin();
		fprintf(errPtr,"badDblReg(): bad register at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();
		
		FATAL_ERROR();
	} 
	return;

}/*end badDblReg*/

/*-----------------------------------------------------------------*/

void checkAddress(U1 *arr, U8 cb)
{
	U8* addr; 
	addr = (U8*)&arr[cb]; 
	if(*addr > R[$TOP])
	{ 
		ERROR0_LVL2("checkAddress(): address out of bounds");

		xmlBegin(); 
		fprintf(errPtr,"checkAddress(): invalid address literal ");
		fpU8(errPtr,*addr);
		fprintf(errPtr," at memory location "); 
		fpU8(errPtr,cb); 
		xmlEnd();

		FATAL_ERROR();
	}
	return;

}/*end checkAddress*/ 

/*-----------------------------------------------------------------*/

/* 
don't want incomplete instruction (hit end bytecode before end 
of instruction) 
*/

void checkCurrentByte(U8 cb, U8 end)
{
	if(cb>=end)
	{ 
		ERROR0_LVL2("checkCurrentByte(): incomplete instruction"); 

		xmlBegin(); 
		fprintf(errPtr,"checkCurrentByte(): ");
		fprintf(errPtr,"incomplete instruction at address "); 
		fpU8(errPtr,cb); 
		xmlEnd();

		FATAL_ERROR(); 
	}
	return;

}/*end checkCurrentByte*/

/*-----------------------------------------------------------------*/

/* 
last byte of an instruction is allowed to be at end of bytecode, so 
use checkEndCurrentByte 
*/

void checkEndCurrentByte(U8 cb, U8 end)
{
	if(cb>end)
	{ 
		ERROR0_LVL2("checkEndCurrentByte(): incomplete instruction"); 
		
		xmlBegin(); 
		fprintf(errPtr,"checkEndCurrentByte(): ");
		fprintf(errPtr,"incomplete instruction at address "); 
		fpU8(errPtr,cb); 
		xmlEnd();

		FATAL_ERROR(); 
	}
	return;

}/*end checkEndCurrentByte*/

/*-----------------------------------------------------------------*/

void badAddress(U8 arg, U8 currentbyte)
{	
	if(arg>R[$TOP]) 
	{ 
		ERROR0_LVL2("badAddress(): invalid address literal");
		
		xmlBegin();
		fprintf(errPtr,"badAddress(): invalid address literal ");
		fpU8(errPtr,arg); 
		fprintf(errPtr," at memory location "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();
	}
	return;

}/*end badAddress*/

/*-----------------------------------------------------------------*/

void badStack(U8 arg, U8 currentbyte)
{
	if(arg<=R[$HE]) 
	{ 
		ERROR0_LVL2("badStack(): stack overflow into heap");

		xmlBegin();
		fprintf(errPtr,"badStack(): ");
		fpU8(errPtr,arg); 
		fprintf(errPtr," stack overflow into heap "); 
		fprintf(errPtr,"at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();
	}
	else if(arg>R[$TOP])
	{
		ERROR0_LVL2("badStack(): stack underflow beyond $TOP");

		xmlBegin();
		fprintf(errPtr,"badStack(): ");
		fpU8(errPtr,arg); 
		fprintf(errPtr," stack underflow beyond $TOP "); 
		fprintf(errPtr,"at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();
	}
	return;

}/*end badStack*/ 	

/*-----------------------------------------------------------------*/

void badHeap(U8 arg, U8 currentbyte)
{
	if(arg<R[$HS]) 
	{ 
		ERROR0_LVL2("badHeap(): below heap");

		xmlBegin();
		fprintf(errPtr,"badHeap(): ");
		fpU8(errPtr,arg); 
		fprintf(errPtr," below start of heap "); 
		fprintf(errPtr,"at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();
	}
	else if(arg>R[$HE])
	{
		ERROR0_LVL2("badHeap(): beyond end of heap");

		xmlBegin();
		fprintf(errPtr,"badHeap(): ");
		fpU8(errPtr,arg); 
		fprintf(errPtr," beyond end of heap "); 
		fprintf(errPtr,"at address "); 
		fpU8(errPtr,currentbyte); 
		xmlEnd();

		FATAL_ERROR();

	}
	return;

}/*end badHeap*/ 	