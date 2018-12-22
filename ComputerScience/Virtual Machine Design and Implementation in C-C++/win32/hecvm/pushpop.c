/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  pushpop.c - this file implements stack instructions              +
+				PUSHB, PUSHW, PUSHD, PUSHQ, PUSHF1, PUSHF2			+
+				POPB, POPW, POPD, POPQ, POPF1, POPF2				+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	PUSHB $r1  

	place lowest order byte of register on stack 

	what distinguishes instructions is how much of the register we push
	B - push lowest order byte ( assume there is only a byte in the register )
	W - push two lowest order bytes
	D - push three lower order bytes
	Q - push whole damn register to address
*/

#define HANDLE_PUSHB();	DBG_RUN0("Optimized HANDLE_PUSHB\n");\
						R[$SP]= R[$SP]-1; badStack(R[$SP],R[$IP]); \
						RAM[R[$SP]] = (S1)(R[RAM[R[$IP]+1]]);\
						R[$IP]=R[$IP]+2;

/*-----------------------------------------------------------------*/

#define HANDLE_PUSHW();	DBG_RUN0("Optimized HANDLE_PUSHW\n");\
						R[$SP]= R[$SP]-2; badStack(R[$SP],R[$IP]); \
						*((S2*)&RAM[R[$SP]]) = (S2)(R[RAM[R[$IP]+1]]);\
						R[$IP]=R[$IP]+2;
						
/*-----------------------------------------------------------------*/

#define HANDLE_PUSHD();	DBG_RUN0("Optimized HANDLE_PUSHD\n");\
						R[$SP]= R[$SP]-4; badStack(R[$SP],R[$IP]); \
						*((S4*)&RAM[R[$SP]]) = (S4)(R[RAM[R[$IP]+1]]);\
						R[$IP]=R[$IP]+2;

/*-----------------------------------------------------------------*/

#define HANDLE_PUSHQ();	DBG_RUN0("Optimized HANDLE_PUSHQ\n");\
						R[$SP]= R[$SP]-8; badStack(R[$SP],R[$IP]); \
						*((S8*)&RAM[R[$SP]]) = (S8)(R[RAM[R[$IP]+1]]);\
						R[$IP]=R[$IP]+2;

/*-----------------------------------------------------------------*/

#define HANDLE_PUSHF1();	DBG_RUN0("Optimized HANDLE_PUSHF1\n");\
						R[$SP]= R[$SP]-4; badStack(R[$SP],R[$IP]); \
						*((F4*)&RAM[R[$SP]]) = Rf[RAM[R[$IP]+1]];\
						R[$IP]=R[$IP]+2;

/*-----------------------------------------------------------------*/

#define HANDLE_PUSHF2();	DBG_RUN0("Optimized HANDLE_PUSHF2\n");\
						R[$SP]= R[$SP]-8; badStack(R[$SP],R[$IP]); \
						*((F8*)&RAM[R[$SP]]) = Rd[RAM[R[$IP]+1]];\
						R[$IP]=R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPB();		DBG_RUN0("Optimized HANDLE_POPB\n");\
							R[RAM[R[$IP]+1]]=(S8)*((S1*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+1;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPW();		DBG_RUN0("Optimized HANDLE_POPW\n");\
							R[RAM[R[$IP]+1]]=(S8)*((S2*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+2;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPD();		DBG_RUN0("Optimized HANDLE_POPD\n");\
							R[RAM[R[$IP]+1]]=(S8)*((S4*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+4;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPQ();		DBG_RUN0("Optimized HANDLE_POPQ\n");\
							R[RAM[R[$IP]+1]]=(S8)*((S8*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+8;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPF1();		DBG_RUN0("Optimized HANDLE_POPF1\n");\
							Rf[RAM[R[$IP]+1]]=(F4)*((F4*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+4;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;

/*-----------------------------------------------------------------*/

#define  HANDLE_POPF2();		DBG_RUN0("Optimized HANDLE_POPF2\n");\
							Rd[RAM[R[$IP]+1]]=(F8)*((F8*)&RAM[R[$SP]]);\
							R[$SP]=R[$SP]+8;\
							badStack(R[$SP],R[$IP]);R[$IP] = R[$IP]+2;
