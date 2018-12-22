/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  load.c - this file implements load instructions                  +
+				LBI, LWI, LDI, LQI, LF1I, LF2I						+
+				LAD, LAI											+
+               LB, LW, LD, LQ, LF1, LF2			     			+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  macros                                                           +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define DBG_RUN		1*/

#ifdef DBG_RUN
#define DBG_RUN0(str);				printf("run(): "); printf(str);
#define DBG_RUN1(str,arg1);			printf("run(): "); printf(str,arg1);
#else
#define DBG_RUN0(str);
#define DBG_RUN1(str,arg1);				
#endif	

/*
	LBI $r1, byte constant ->  BBB  
	
	first byte	= instruction
	second byte = register to put constant into
	third byte	= constant to place in register
*/

#define HANDLE_LBI();	DBG_RUN0("Optimized HANDLE_LBI\n"); \
						R[RAM[R[$IP]+1]]=(S1)RAM[R[$IP]+2]; R[$IP]=R[$IP]+3;
					  
/*-----------------------------------------------------------------*/


#define HANDLE_LWI();	DBG_RUN0("Optimized HANDLE_LWI\n"); \
						R[RAM[R[$IP]+1]]=(S2)*((S2*)&RAM[R[$IP]+2]);R[$IP]=R[$IP]+4;

/*-----------------------------------------------------------------*/


#define HANDLE_LDI();	DBG_RUN0("Optimized HANDLE_LDI\n"); \
						R[RAM[R[$IP]+1]]=(S4)*((S4*)&RAM[R[$IP]+2]);R[$IP]=R[$IP]+6;

/*-----------------------------------------------------------------*/


#define HANDLE_LQI();	DBG_RUN0("Optimized HANDLE_LQI\n"); \
						R[RAM[R[$IP]+1]]=(S8)*((S8*)&RAM[R[$IP]+2]);R[$IP]=R[$IP]+10;

/*-----------------------------------------------------------------*/

#define HANDLE_LF1I();	DBG_RUN0("Optimized HANDLE_LF1I\n"); \
						Rf[RAM[R[$IP]+1]]=(F4)*((F4*)&RAM[R[$IP]+2]);R[$IP]=R[$IP]+6;

/*-----------------------------------------------------------------*/

#define HANDLE_LF2I();	DBG_RUN0("Optimized HANDLE_LF2I\n"); \
						Rd[RAM[R[$IP]+1]]=(F8)*((F8*)&RAM[R[$IP]+2]);R[$IP]=R[$IP]+10;

/*-----------------------------------------------------------------*/

/* LAD $r1, address */ 

#define HANDLE_LAD();	DBG_RUN0("Optimized HANDLE_LAD\n"); \
						R[RAM[R[$IP]+1]]=(U8)*((U8*)&RAM[R[$IP]+2]);\
						badAddress(R[RAM[R[$IP]+1]],R[$IP]); R[$IP]=R[$IP]+10;

/*-----------------------------------------------------------------*/

/* LAI $r1, $r2, qword*/ 

#define HANDLE_LAI();	DBG_RUN0("Optimized HANDLE_LAI\n"); \
						R[RAM[R[$IP]+1]]=(R[RAM[R[$IP]+2]])+((S8)*((S8*)&RAM[R[$IP]+3]));\
						badAddress(R[RAM[R[$IP]+1]],R[$IP]); R[$IP]=R[$IP]+11;

/*-----------------------------------------------------------------*/

/*
	LB $r1, $r2 ->  BBB  
	
	first byte = instruction
	second byte = register to put byte into
	third byte = register containing address where byte is located 
*/

#define HANDLE_LB();	DBG_RUN0("Optimized HANDLE_LB\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						R[RAM[R[$IP]+1]] = (S8)(*((S1*)&RAM[R[RAM[R[$IP]+2]]])); \
						R[$IP]=R[$IP]+3;

/*-----------------------------------------------------------------*/

#define HANDLE_LW();	DBG_RUN0("Optimized HANDLE_LW\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						R[RAM[R[$IP]+1]] = (S8)(*((S2*)&RAM[(U8)R[RAM[R[$IP]+2]]])); \
						R[$IP]=R[$IP]+3;

/*-----------------------------------------------------------------*/

#define HANDLE_LD();	DBG_RUN0("Optimized HANDLE_LD\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						R[RAM[R[$IP]+1]] = (S8)(*((S4*)&RAM[(U8)R[RAM[R[$IP]+2]]])); \
						R[$IP]=R[$IP]+3;

/*-----------------------------------------------------------------*/


#define HANDLE_LQ();	DBG_RUN0("Optimized HANDLE_LQ\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						R[RAM[R[$IP]+1]] = (S8)(*((S8*)&RAM[(U8)R[RAM[R[$IP]+2]]])); \
						R[$IP]=R[$IP]+3;

/*-----------------------------------------------------------------*/

/* LFI $f, $r */

#define HANDLE_LF1();	DBG_RUN0("Optimized HANDLE_LF1\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						Rf[RAM[R[$IP]+1]] = *((F4*)&RAM[(U8)R[RAM[R[$IP]+2]]]); \
						R[$IP]=R[$IP]+3;	

/*-----------------------------------------------------------------*/

/* LF2 $d, $r */

#define HANDLE_LF2();	DBG_RUN0("Optimized HANDLE_LF2\n");\
						badAddress(R[RAM[R[$IP]+2]],R[$IP]); \
						Rd[RAM[R[$IP]+1]] = *((F8*)&RAM[(U8)R[RAM[R[$IP]+2]]]); \
						R[$IP]=R[$IP]+3;