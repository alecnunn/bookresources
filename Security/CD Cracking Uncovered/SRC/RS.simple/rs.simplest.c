/*----------------------------------------------------------------------------
 *
 *				SIMPLEST REED-SOLOMON ENCODER/DECODER
 *				======================================
 *
 * Build 0x001 @ 02.07.2003
----------------------------------------------------------------------------*/
// ATTENTION! This encoder/decoder is built on the basis of normal arithmetics,
// _not_ based on Galois fields, as a result its functionality is limited.
// Nevertheless, it is illustrative
#include <stdio.h>

#define SYM_WIDE	8		// Widths of the input information sybmol

#define DATAIN	0xFF			// output data (one byte)

#define ERR_POS	3		// number of bit that will be corrupted

// polynomial
#define MAG (1<<(SYM_WIDE*1) + 1<<(SYM_WIDE*0))


// -------------------------------------------------------------------------------
// Defining error position x by remainder k from division of the codeword by
// polynomial k = 2^x.
// The function receives k and returns x
// -------------------------------------------------------------------------------
int pow_table[9] = {1,2,4,8,16,32,64,128,256};
lockup(int x) {int a;for(a=0;a<9;a++) if(pow_table[a]==x)return a; return -1;}

main()
{
	int i; int g; int c; int e; int k;
	
	fprintf(stderr,"simplest Reed-Solomon endoder/decoder by Kris Kaspersky\n\n");
	i = DATAIN;				// Input data (codeword)
	g = MAG;				// Prime polynomial
	printf("i = %08x    (DATAIN)\ng = %08x    (POLYNOM)\n", i, g);
	
	// Reed-Solomom coder (primitive, but working)
	// computing the codeword intended for transmission
	c = i * g;	printf("c = %08x    (CODEWORD)\n", c);
	// end coder
	
	// transmit with errors
	e = c ^ (1<<ERR_POS); printf("e = %08x    (RAW RECIVED DATA+ERR)\n\n", e);
	/*       ^^^^ inverting one bit, simulating transmission error */
	
	// Reed-Solomon decoder
	// Checking for transmission errors

	if (e % g) 
	{
		// errors detected, trying to correct
		printf("RS decoder says: (%x) error detected\n{\n", e % g);
		k = (e % g);  // k = 2^x, where x is error position
		printf("\t0 to 1 err  position: %x\n", lockup(k));
		printf ("\trestored codeword is: %x\n}\n", (e ^= k));
	}
	printf("RECEIVED DATA IS: %x\n", e / g);
	// Decoder end
}
