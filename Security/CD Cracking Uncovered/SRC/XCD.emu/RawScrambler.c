/*---------------------------------------------------------------------------
 *
 *				GENERATES THE SEQUENCE FOR CD SCRAMBLER
 *				========================================
 *
 * build 0x001 @ 07.06.2003
----------------------------------------------------------------------------*/
#include <stdio.h>

// Control fragment of the genuine sequence for testing the program
// -----------------------------------------------------------------------
//0x8001,0x6000,0x2800,0x1e00,0x0880,0x0660,0x02a8,0x81fe,0x6080,0x2860,0x1e28,
//0x889e,0x6668,0xaaae,0x7ffc,0xe001,0x4800,0x3600,0x1680,0x0ee0,0x04c8,0x8356,
//0xe17e,0x48e0,0x3648,0x96b6,0xeef6,0xccc6,0xd552,0x9ffd,0xa801,0x7e00,0x2080,

printf_bin(int a)
{
	int b;
	for(b = 15; b >= 0; b--) printf("%x",(a & (1<<b))?1:0);printf(" %x\n",a);
}

main()
{
	int a, tmp;
	int reg = 0x8001; // first element of the scrambler sequence

	for(a = 1; a < 1170/* length of the scrambled sector part in words*/; a++)
	{
		// Printing
		printf_bin(reg);
		if ((a % 8) == 0) printf(".............%03d.................\n",a /8);

		// Modulo-2 addition with shift
		tmp = reg >> 1; tmp = reg ^ tmp; reg = tmp >> 1;

		// Processing the x^15+x+1 polynomial, which is equivalent to. 1<<15 + 1<<1 + 1<<0
		if (reg & 1<<1) reg = reg ^  (1<<15);
		if (reg & 1<<0) reg = reg ^ ((1<<15) | (1<<14));
	}
}