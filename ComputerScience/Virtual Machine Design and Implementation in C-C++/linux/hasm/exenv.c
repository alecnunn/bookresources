/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ exenv.c - this file stores info about the execution environmnet   +
+           ( i.e. RAM, registers )									+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* memory pointer, allocate memory on boot ------------------------*/

U1 *RAM;	/* [0,$BE]=code [$HS,$HE]=heap [$SS,$TOP]=stack */

/* REGISTERS ------------------------------------------------------*/

/*
	R[]		is used to represent the registers
	R_STR[] prints out register strings
*/

U8 R[32];  

#define $IP		0	/* address of next instruction to execute */
#define $SP		1	/* address of most recent value popped on stack */
#define $FP		2	/* stack frame pointer */
#define $BE		3	/* address of last byte of bytecode section */

#define $HS		4   /* address of first ( lowest ) byte of heap */
#define $HE		5   /* address of last ( highest ) byte used by heap */

#define $SS		6	/* address of bottom of stack ( lowest ) */
#define $TOP	7	/* address of top-of-stack ( highest )*/

/* General purpose registers */ 

#define $R1		8
#define $R2		9
#define $R3		10
#define $R4		11
#define $R5		12
#define $R6		13
#define $R7		14
#define $R8		15
#define $R9		16
#define $R10	17
#define $R11	18
#define $R12	19
#define $R13	20
#define $R14	21
#define $R15	22
#define $R16	23
#define $R17	24
#define $R18	25
#define $R19	26
#define $R20	27
#define $R21	28
#define $R22	29
#define $R23	30
#define $R24	31

/* to get the string version of a register, use  R_STR[$IP]="$IP" */

char *R_STR[] = 
{
	"$IP","$SP","$FP","$BE","$HS","$HE","$SS","$TOP",
	"$R1","$R2","$R3","$R4","$R5","$R6","$R7","$R8","$R9","$R10",
	"$R11","$R12","$R13","$R14","$R15","$R16","$R17","$R18","$R19","$R20",
	"$R21","$R22","$R23","$R24"
};

F4 Rf[10];
char *Rf_STR[]={"$F1","$F2","$F3","$F4","$F5","$F6","$F7","$F8","$F9","$F10"};

#define $F1		0
#define $F2		1
#define $F3		2
#define $F4		3
#define $F5		4
#define $F6		5
#define $F7		6
#define $F8		7
#define $F9		8
#define $F10    9

F8 Rd[10];
char *Rd_STR[]={"$D1","$D2","$D3","$D4","$D5","$D6","$D7","$D8","$D9","$D10"};   

#define $D1		0
#define $D2		1
#define $D3		2
#define $D4		3
#define $D5		4
#define $D6		5
#define $D7		6
#define $D8		7
#define $D9		8
#define $D10    9

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ function prototypes                                               +                                                                  
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printMemorySection(U8 address,U8 bytes);
void printAllRAM();
void printBasicRegisters();
void printGeneralRegisters();
void printFloatRegisters();
void printDoubleRegisters();

void testRegisterPrinting();

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ function definitions                                              +                                                                  
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printMemorySection(U8 address,U8 bytes)
{
	U8 index;
	for(index=address;index<address+bytes;index++)
	{
		PRINT_MEM(index);printf("\n");
	}
	return;

}/*end printMemorySection*/

/*-----------------------------------------------------------------*/

void printAllRAM()
{
	U8 index;
	for(index=0;index<(U8)R[$TOP];index++)
	{
		PRINT_MEM(index);printf("\n");
	}
	return;

}/*end printAllRAM*/

/*-----------------------------------------------------------------*/

void printBasicRegisters()
{
	printf("BASIC REGISTERS------------------------\n");
	PRINT_UREG(R_STR[$IP],R[$IP]); printf(" ");
	PRINT_UREG(R_STR[$SP],R[$SP]); printf("\n");
	PRINT_UREG(R_STR[$FP],R[$FP]); printf(" ");
	PRINT_UREG(R_STR[$BE],R[$BE]); printf("\n");
	PRINT_UREG(R_STR[$HS],R[$HS]); printf(" ");
	PRINT_UREG(R_STR[$HE],R[$HE]); printf("\n");
	PRINT_UREG(R_STR[$SS],R[$SS]); printf(" ");
	PRINT_UREG(R_STR[$TOP],R[$TOP]); printf("\n");
	printf("---------------------------------------\n");
	return;

}/*end printBasicRegisters*/

/*-----------------------------------------------------------------*/

void printGeneralRegisters()
{
	printf("GENERAL REGISTERS----------------------\n");
	PRINT_SREG(R_STR[$R1],R[$R1]); printf(" ");
	PRINT_SREG(R_STR[$R2],R[$R2]); printf("\n");
	PRINT_SREG(R_STR[$R3],R[$R3]); printf(" ");
	PRINT_SREG(R_STR[$R4],R[$R4]); printf("\n");
	PRINT_SREG(R_STR[$R5],R[$R5]); printf(" ");
	PRINT_SREG(R_STR[$R6],R[$R6]); printf("\n");
	PRINT_SREG(R_STR[$R7],R[$R7]); printf(" ");
	PRINT_SREG(R_STR[$R8],R[$R8]); printf("\n");
	PRINT_SREG(R_STR[$R9],R[$R9]); printf(" ");
	PRINT_SREG(R_STR[$R10],R[$R10]); printf("\n");
	PRINT_SREG(R_STR[$R11],R[$R11]); printf(" ");
	PRINT_SREG(R_STR[$R12],R[$R12]); printf("\n");
	PRINT_SREG(R_STR[$R13],R[$R13]); printf(" ");
	PRINT_SREG(R_STR[$R14],R[$R14]); printf("\n");
	PRINT_SREG(R_STR[$R15],R[$R15]); printf(" ");
	PRINT_SREG(R_STR[$R16],R[$R16]); printf("\n");
	PRINT_SREG(R_STR[$R17],R[$R17]); printf(" ");
	PRINT_SREG(R_STR[$R18],R[$R18]); printf("\n");
	PRINT_SREG(R_STR[$R19],R[$R19]); printf(" ");
	PRINT_SREG(R_STR[$R20],R[$R20]); printf("\n");
	PRINT_SREG(R_STR[$R21],R[$R21]); printf(" ");
	PRINT_SREG(R_STR[$R22],R[$R22]); printf("\n");
	PRINT_SREG(R_STR[$R23],R[$R23]); printf(" ");
	PRINT_SREG(R_STR[$R24],R[$R24]); printf("\n");
	printf("---------------------------------------\n");
	return;

}/*end printGeneralRegisters*/

/*-----------------------------------------------------------------*/

void printFloatRegisters()
{
	printf("FLOAT REGISTERS------------------------\n");
	PRINT_FREG(Rf_STR[$F1],Rf[$F1]); printf(" ");
	PRINT_FREG(Rf_STR[$F2],Rf[$F2]); printf("\n");
	PRINT_FREG(Rf_STR[$F3],Rf[$F3]); printf(" ");
	PRINT_FREG(Rf_STR[$F4],Rf[$F4]); printf("\n");
	PRINT_FREG(Rf_STR[$F5],Rf[$F5]); printf(" ");
	PRINT_FREG(Rf_STR[$F6],Rf[$F6]); printf("\n");
	PRINT_FREG(Rf_STR[$F7],Rf[$F7]); printf(" ");
	PRINT_FREG(Rf_STR[$F8],Rf[$F8]); printf("\n");
	PRINT_FREG(Rf_STR[$F9],Rf[$F9]); printf(" ");
	PRINT_FREG(Rf_STR[$F10],Rf[$F10]); printf("\n");
	printf("---------------------------------------\n");
	return;

}/*end printFloatRegisters*/

/*-----------------------------------------------------------------*/

void printDoubleRegisters()
{
	printf("DOUBLE REGISTERS-----------------------\n");
	PRINT_DREG(Rd_STR[$D1],Rd[$D1]); printf(" ");
	PRINT_DREG(Rd_STR[$D2],Rd[$D2]); printf("\n");
	PRINT_DREG(Rd_STR[$D3],Rd[$D3]); printf(" ");
	PRINT_DREG(Rd_STR[$D4],Rd[$D4]); printf("\n");
	PRINT_DREG(Rd_STR[$D5],Rd[$D5]); printf(" ");
	PRINT_DREG(Rd_STR[$D6],Rd[$D6]); printf("\n");
	PRINT_DREG(Rd_STR[$D7],Rd[$D7]); printf(" ");
	PRINT_DREG(Rd_STR[$D8],Rd[$D8]); printf("\n");
	PRINT_DREG(Rd_STR[$D9],Rd[$D9]); printf(" ");
	PRINT_DREG(Rd_STR[$D10],Rd[$D10]); printf("\n");
	printf("---------------------------------------\n");
	return;

}/*end printDoubleRegisters*/

/*-----------------------------------------------------------------*/

void testRegisterPrinting()
{
	U1 i;
	i=0;

	for(i=$IP;i<=$R24;i++){ R[i] = i; }
	printBasicRegisters();
	printGeneralRegisters();

	for(i=$F1;i<=$F10;i++){ Rf[i] = i; }
	printFloatRegisters();

	for(i=$D1;i<=$D10;i++){ Rd[i] = i; }
	printDoubleRegisters();

	return;

}/*end testRegisterPrinting*/

