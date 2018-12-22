/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ iset.c - this file stores the instruction set-integer mappings    +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* DATA TRANSFER --------------------------------------------------*/

#define	LBI		0	
#define LWI		1
#define	LDI		2
#define	LQI		3
#define LF1I	4
#define LF2I	5

#define LAD		6
#define LAI		7

#define LB		8	
#define	LW		9
#define	LD		10
#define	LQ		11	
#define LF1		12
#define LF2		13

#define	SB		14
#define SW		15
#define SD		16
#define SQ		17
#define SF1		18
#define SF2		19

#define PUSHB	20	
#define PUSHW	21
#define	PUSHD	22
#define	PUSHQ	23
#define PUSHF1	24
#define PUSHF2	25

#define	POPB	26	
#define	POPW	27
#define	POPD	28
#define	POPQ	29
#define POPF1	30
#define POPF2	31

#define MOV		32
#define MOVF	33
#define MOVD	34

/* PROGRAM FLOW CONTROL--------------------------------------------*/	

#define	JMP		35
#define	JE		36  
#define	JNE		37
#define SLT		38	
#define	INT		39
#define	DI		40
#define	EI		41
#define HALT	42
#define NOP		43

/* BITWISE --------------------------------------------------------*/

#define AND		44	
#define	OR		45
#define	XOR		46
#define	NOT		47	
#define	BT		48  
#define	BS		49  

/* SHIFT ----------------------------------------------------------*/

#define SRA		50
#define	SRL		51
#define	SL		52

/* INTEGER ARITHMETIC ---------------------------------------------*/

#define ADD		53	
#define	SUB		54
#define	MULT	55
#define DIV		56

/* CONVERSION -----------------------------------------------------*/

#define CAST_IF		57
#define CAST_ID		58	
#define CAST_FI		59
#define CAST_FD		60
#define CAST_DI		61
#define CAST_DF		62

/* FLOATING-POINT MATH --------------------------------------------*/

#define	FADD	63
#define FSUB	64
#define	FMULT	65
#define	FDIV	66
#define	FSLT	67

#define	DADD	68
#define DSUB	69
#define	DMULT	70
#define	DDIV	71
#define	DSLT	72

#define BAD		-1	/* not an instruction */

char *I_Set[73] = 
	{
		"LBI","LWI","LDI","LQI","LF1I","LF2I",
		"LAD","LAI",
		"LB","LW","LD","LQ","LF1","LF2",		
		"SB","SW","SD","SQ","SF1","SF2",				
		"PUSHB","PUSHW","PUSHD","PUSHQ","PUSHF1","PUSHF2",	
		"POPB","POPW","POPD","POPQ","POPF1","POPF2",		
		"MOV","MOVF","MOVD",
		"JMP","JE","JNE","SLT","INT","DI","EI","HALT","NOP", 
		"AND","OR","XOR","NOT","BT","BS",	
		"SRA","SRL","SL",			
		"ADD","SUB","MULT","DIV",
		"CAST_IF","CAST_ID","CAST_FI","CAST_FD","CAST_DI","CAST_DF",
		"FADD","FSUB","FMULT","FDIV","FSLT", 
		"DADD","DSUB","DMULT","DDIV","DSLT"
	};