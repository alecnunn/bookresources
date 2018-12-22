/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  bitwise.c - this file implements bitwise instructions            +
+				AND, OR, XOR, NOT, BT, BS                           +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* AND $r1, $r2, $r3*/

#define HANDLE_AND();	DBG_RUN0("Optimized HANDLE_AND\n");R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+2]] & R[RAM[R[$IP]+3]];R[$IP]=R[$IP]+4;

/* OR $r1, $r2, $r3*/

#define HANDLE_OR();	DBG_RUN0("Optimized HANDLE_OR\n");R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+2]] | R[RAM[R[$IP]+3]];R[$IP]=R[$IP]+4;

/* XOR $r1, $r2, $r3*/

#define HANDLE_XOR();	DBG_RUN0("Optimized HANDLE_XOR\n");R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+2]] ^ R[RAM[R[$IP]+3]];R[$IP]=R[$IP]+4;

/* NOT $r1, $r2*/

#define HANDLE_NOT();	DBG_RUN0("Optimized HANDLE_NOT\n");R[RAM[R[$IP]+1]] = ~R[RAM[R[$IP]+2]];R[$IP]=R[$IP]+3;

/*-----------------------------------------------------------------*/

/* BT $r1, $r2, $r3  $r1=1 if $r3rd bit in $r2 is set, else 0*/

void handleBT()
{
	int index;
	U8 mask;
	U8 bit; /* 0-63 */

	DBG_RUN0("handleBT\n"); 
	bit = R[RAM[R[$IP]+3]]; 
	if(bit > 63)
	{
		ERROR0_LVL2("handleBT(): bit index is out of range");
		bit = 63;
	}
	mask = (U8)0x1;
	for(index=0;index<bit;index++)
	{
		mask = mask*2;
	}
	mask = R[RAM[R[$IP]+2]] & mask;
	if(mask > 0){ R[RAM[R[$IP]+1]] = 0x1; }
	else{ R[RAM[R[$IP]+1]] = 0x0; }
	R[$IP]=R[$IP]+4;
	return;

}/*end handleBT*/

/*-----------------------------------------------------------------*/

/* BS $r1, $r2	set $r2th bit in $r1 */

void handleBS()
{
	int index;
	U8 mask;
	U8 bit; /* 0-63 */

	DBG_RUN0("handleBS\n"); 
	bit = R[RAM[R[$IP]+2]];
	if(bit > 63)
	{
		ERROR0_LVL2("handleBS(): bit index is out of range");
		bit = 63;
	}
	mask = (U8)0x1;
	for(index=0;index<bit;index++)
	{
		mask = mask*2;
	}
	R[RAM[R[$IP]+1]] = R[RAM[R[$IP]+1]] | mask;
	R[$IP]=R[$IP]+3;
	return;

}/*end handleBS*/
