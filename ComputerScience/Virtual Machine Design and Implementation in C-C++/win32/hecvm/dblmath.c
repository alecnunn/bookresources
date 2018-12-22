/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  dblmath.c - this file implements double-precision IEEE math      +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* DADD $d1, $d2, $d3 */
#define HANDLE_DADD()	DBG_RUN0("Optimized HANDLE_DADD\n"); \
						Rd[RAM[R[$IP]+1]] = Rd[RAM[R[$IP]+2]]+Rd[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* DSUB $d1, $d2, $d3 */
#define HANDLE_DSUB()	DBG_RUN0("Optimized HANDLE_DSUB\n"); \
						Rd[RAM[R[$IP]+1]] = Rd[RAM[R[$IP]+2]]-Rd[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* DMULT $d1, $d2, $d3 */
#define HANDLE_DMULT()	DBG_RUN0("Optimized HANDLE_DMULT\n"); \
						Rd[RAM[R[$IP]+1]] = Rd[RAM[R[$IP]+2]]*Rd[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* DDIV $d1, $d2, $d3 */
#define HANDLE_DDIV()	DBG_RUN0("Optimized HANDLE_DDIV\n"); \
						Rd[RAM[R[$IP]+1]] = Rd[RAM[R[$IP]+2]]/Rd[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* DSLT $d1, $d2, $d3 */
#define HANDLE_DSLT()	DBG_RUN0("Optimized HANDLE_DSLT\n");\
		if(Rd[RAM[R[$IP]+2]] < Rd[RAM[R[$IP]+3]]){ Rd[RAM[R[$IP]+1]] = 1.0; }\
		else{ Rd[RAM[R[$IP]+1]] = 0.0; }\
		R[$IP]=R[$IP]+4;
