/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  store.c - this file implements store instructions                +
+				SB, SW, SD, SQ, SF1, SF2                            +
+                                                                   +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	SB $r1, $r2 ->  BBB 

	$r1 = where byte is stored
	$r2 = address where we want to place byte
*/


#define HANDLE_SB();	DBG_RUN0("Optimized HANDLE_SB\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);RAM[R[RAM[R[$IP]+2]]] = (S1)R[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;

/*-----------------------------------------------------------------*/


#define HANDLE_SW();	DBG_RUN0("Optimized HANDLE_SW\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);*((S2*)&RAM[R[RAM[R[$IP]+2]]] ) = (S2)R[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;

/*-----------------------------------------------------------------*/


#define HANDLE_SD();	DBG_RUN0("Optimized HANDLE_SD\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);*((S4*)&RAM[R[RAM[R[$IP]+2]]] ) = (S4)R[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;

/*-----------------------------------------------------------------*/


#define HANDLE_SQ();	DBG_RUN0("Optimized HANDLE_SQ\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);*((S8*)&RAM[R[RAM[R[$IP]+2]]] ) = (S8)R[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;

/*-----------------------------------------------------------------*/

/* SF1 $f, $r */

#define HANDLE_SF1();	DBG_RUN0("Optimized HANDLE_SF1\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);*((F4*)&RAM[R[RAM[R[$IP]+2]]] ) = Rf[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;

/*-----------------------------------------------------------------*/

/* SF2 $d, $r */

#define HANDLE_SF2();	DBG_RUN0("Optimized HANDLE_SF2\n");badAddress(R[RAM[R[$IP]+2]],R[$IP]);*((F8*)&RAM[R[RAM[R[$IP]+2]]] ) = Rd[RAM[R[$IP]+1]];R[$IP] = R[$IP]+3;