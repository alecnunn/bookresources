/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  jump.c - this file implements misc. branch instructions          +
+				JMP, JE, JNE, SLT, INT, DI, EI, NOP                 +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	REMEMBER: JUMPING involves relative displacement!
*/

/* JMP $r1 */
#define HANDLE_JMP();	DBG_RUN0("Optimized HANDLE_JMP\n"); \
						badAddress(R[RAM[R[$IP]+1]],R[$IP]);\
						R[$IP] = R[RAM[R[$IP]+1]];

/* JE $r1, $r2, $r3*/
#define HANDLE_JE();		DBG_RUN0("Optimized HANDLE_JE\n"); \
						if(R[RAM[R[$IP]+1]]==R[RAM[R[$IP]+2]])\
						{\
							badAddress(R[RAM[R[$IP]+3]],R[$IP]);\
							R[$IP] = R[RAM[R[$IP]+3]];\
						}\
						else{ R[$IP]=R[$IP]+4; }

/* JNE $r1, $r2, $r3 */
#define HANDLE_JNE();	DBG_RUN0("Optimized HANDLE_JNE\n"); \
						if(R[RAM[R[$IP]+1]]!=R[RAM[R[$IP]+2]])\
						{\
							badAddress(R[RAM[R[$IP]+3]],R[$IP]);\
							R[$IP] = R[RAM[R[$IP]+3]];\
						}\
						else{ R[$IP]=R[$IP]+4; }

/* SLT $1, $r2, $r3 */
#define HANDLE_SLT();	DBG_RUN0("Optimized HANDLE_SLT\n"); \
						if(R[RAM[R[$IP]+2]]<R[RAM[R[$IP]+3]])\
						{\
							R[RAM[R[$IP]+1]] = (U8)0x1;\
						}else{ R[RAM[R[$IP]+1]] = (U8)0; }\
						R[$IP]=R[$IP]+4;

/* INT byte*/
#define HANDLE_INT();	DBG_RUN0("Optimized HANDLE_INT\n"); \
						handleInt((U1)RAM[R[$IP]+1]); \
						R[$IP]=R[$IP]+2;

/* DI */
#define HANDLE_DI();	DBG_RUN0("Optimized HANDLE_DI\n"); \
						interruptOn=FALSE; \
						R[$IP]=R[$IP]+1;

/* EI */
#define HANDLE_EI();		DBG_RUN0("Optimized HANDLE_EI\n"); \
						interruptOn=TRUE; \
						R[$IP]=R[$IP]+1;

/* NOP */
#define HANDLE_NOP();	DBG_RUN0("Optimized HANDLE_NOP\n"); \
						R[$IP]=R[$IP]+1;


