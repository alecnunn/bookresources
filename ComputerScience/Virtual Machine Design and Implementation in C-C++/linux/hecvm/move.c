/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  move.c - this file implements misc. moves instruction            +
+			MOV, MOVF, MOVD           								+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* MOV $r1, $r2 */
#define HANDLE_MOV();	DBG_RUN0("Optimized HANDLE_MOV\n");R[RAM[R[$IP]+1]]=R[RAM[R[$IP]+2]];R[$IP] = R[$IP]+3;

/* MOVF $f1, $f2 */
#define HANDLE_MOVF();	DBG_RUN0("Optimized HANDLE_MOVF\n");Rf[RAM[R[$IP]+1]]=Rf[RAM[R[$IP]+2]];R[$IP] = R[$IP]+3;

/* MOVD $d1, $d2 */
#define HANDLE_MOVD();	DBG_RUN0("Optimized HANDLE_MOVD\n");Rd[RAM[R[$IP]+1]]=Rd[RAM[R[$IP]+2]];R[$IP] = R[$IP]+3;

