/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  intmath.c - this file implements SIGNED integer arithmetic       +
+				ADD, SUB, MULT, DIV                                 +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* ADD $r1, $r2, $r3*/

#define HANDLE_ADD();	DBG_RUN0("Optimized HANDLE_ADD\n");R[RAM[R[$IP]+1]] = ((S8)R[RAM[R[$IP]+2]]) + ((S8)R[RAM[R[$IP]+3]]);R[$IP]=R[$IP]+4;

/* SUB $r1, $r2, $r3*/

#define HANDLE_SUB();	DBG_RUN0("Optimized HANDLE_SUB\n");R[RAM[R[$IP]+1]] = ((S8)R[RAM[R[$IP]+2]]) - ((S8)R[RAM[R[$IP]+3]]);R[$IP]=R[$IP]+4;

/* MULT $r1, $r2, $r3*/

#define HANDLE_MULT();	DBG_RUN0("Optimized HANDLE_MULT\n");R[RAM[R[$IP]+1]] = ((S8)R[RAM[R[$IP]+2]]) * ((S8)R[RAM[R[$IP]+3]]);R[$IP]=R[$IP]+4;

/* DIV $r1, $r2, $r3, $r4*/

#define HANDLE_DIV();	DBG_RUN0("Optimized HANDLE_DIV\n"); if(R[RAM[R[$IP]+4]]==0){ERROR0_LVL2("Divide by zero!");R[RAM[R[$IP]+1]]=R[RAM[R[$IP]+2]]=0xFFFFFFFFFFFFFFFF; }else{ R[RAM[R[$IP]+1]] = ((S8)R[RAM[R[$IP]+3]]) / ((S8)R[RAM[R[$IP]+4]]);R[RAM[R[$IP]+2]] = ((S8)R[RAM[R[$IP]+3]]) % ((S8)R[RAM[R[$IP]+4]]);}R[$IP]=R[$IP]+5;



