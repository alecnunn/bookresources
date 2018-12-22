/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  fltmath.c - this file implements single-precision IEEE math      +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	Some instructions require an integer register to hold
	an address or a double register for conversion
	( i.e. LF, SF, FCI, FCD, FCF )
	all others use strictly float registers!
*/

/* CAST_IF $r, $f */
#define HANDLE_CAST_IF();		DBG_RUN0("Optimized CAST_IF\n"); \
						R[RAM[R[$IP]+1]] = (S8)Rf[RAM[R[$IP]+2]]; \
						R[$IP] = R[$IP]+3;

/* CAST_ID $r, $d */
#define HANDLE_CAST_ID();		DBG_RUN0("Optimized CAST_ID\n"); \
						R[RAM[R[$IP]+1]] = (S8)Rd[RAM[R[$IP]+2]]; \
						R[$IP] = R[$IP]+3;

/* CAST_FI $f, $i */
#define HANDLE_CAST_FI();		DBG_RUN0("Optimized CAST_FI\n"); \
						Rf[RAM[R[$IP]+1]] = (F4)((S8)R[RAM[R[$IP]+2]]); \
						R[$IP] = R[$IP]+3;

/* CAST_FD $f, $d */
#define HANDLE_CAST_FD();		DBG_RUN0("Optimized CAST_FD\n"); \
						Rf[RAM[R[$IP]+1]] = (F4)Rd[RAM[R[$IP]+2]]; \
						R[$IP] = R[$IP]+3;

/* CAST_DI $d, $r */
#define HANDLE_CAST_DI();		DBG_RUN0("Optimized CAST_DI\n"); \
						Rd[RAM[R[$IP]+1]] = (F8)((S8)R[RAM[R[$IP]+2]]); \
						R[$IP] = R[$IP]+3;

/* CAST_DF $d, $f */
#define HANDLE_CAST_DF();		DBG_RUN0("Optimized CAST_DF\n"); \
						Rd[RAM[R[$IP]+1]] = (F8)Rf[RAM[R[$IP]+2]]; \
						R[$IP] = R[$IP]+3;


/*-----------------------------------------------------------------*/

/* FADD $f1, $f2, $f3 */
#define HANDLE_FADD();	DBG_RUN0("Optimized HANDLE_FADD\n"); \
						Rf[RAM[R[$IP]+1]] = Rf[RAM[R[$IP]+2]]+Rf[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* FSUB $f1, $f2, $f3 */
#define HANDLE_FSUB();	DBG_RUN0("Optimized HANDLE_FSUB\n"); \
						Rf[RAM[R[$IP]+1]] = Rf[RAM[R[$IP]+2]]-Rf[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* FMULT $f1, $f2, $f3 */
#define HANDLE_FMULT();	DBG_RUN0("Optimized HANDLE_FMULT\n"); \
						Rf[RAM[R[$IP]+1]] = Rf[RAM[R[$IP]+2]]*Rf[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* FDIV $f1, $f2, $f3 */
#define HANDLE_FDIV();	DBG_RUN0("Optimized HANDLE_FDIV\n"); \
						Rf[RAM[R[$IP]+1]] = Rf[RAM[R[$IP]+2]]/Rf[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* FSLT $f1, $f2, $f3 */
#define HANDLE_FSLT();	DBG_RUN0("Optimized HANDLE_FSLT\n");\
		if(Rf[RAM[R[$IP]+2]] < Rf[RAM[R[$IP]+3]]){ Rf[RAM[R[$IP]+1]] = 1.0; }\
		else{ Rf[RAM[R[$IP]+1]] = 0.0; }\
		R[$IP]=R[$IP]+4;
