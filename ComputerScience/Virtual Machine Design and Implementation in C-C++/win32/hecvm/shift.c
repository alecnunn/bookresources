/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  shift.c - this file implements bitwise shifting instructions     +
+				SRA, SRL, SL                                        +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* SRA $r1, $r2, $r3*/

#define HANDLE_SRA();	DBG_RUN0("Optimized HANDLE_SRA\n"); \
						R[RAM[R[$IP]+1]] = ((S8)R[RAM[R[$IP]+2]]) >> ((S8)R[RAM[R[$IP]+3]]);\
						R[$IP]=R[$IP]+4;

/* SRL $r1, $r2, $r3*/

#define HANDLE_SRL();	DBG_RUN0("Optimized HANDLE_SRL\n"); \
						R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+2]] >> R[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

/* SL $r1, $r2, $r3*/

#define HANDLE_SL();	DBG_RUN0("Optimized HANDLE_SL\n"); \
						R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+2]] << R[RAM[R[$IP]+3]];\
						R[$IP]=R[$IP]+4;

