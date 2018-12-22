/*----------------------------------------------------------------------------
 *
 *				XORs FILE CONTENTS BY AN ARBITRARY MASK
 *				============================================
 *
 * Build 0x001 @ 09.07.2003
----------------------------------------------------------------------------*/
#include <stdio.h>

#define MAX_LEN			666					// Max. mask legnth
#define MAX_BUF_SIZE	(100*1024)			// Read buffer size
#define FDECODE			"decrypt.dat"		// Name of decrupted file

main(int argc, char **argv)
{
	long	 a, b;
	long	 key_len;
	FILE	 *fin, *fout;
	long	 buf_size, real_size;
	unsigned char key[MAX_LEN];
	unsigned char buf[MAX_BUF_SIZE];

	if (argc<3)		// HELP on command-line options
	{
		fprintf(stderr,"USAGE: DeXOR.exe file_name AA BB CC DD EE...\n");
		return 0;
	}

	// determining the key length and setting buffer size as a multiple of the key length
	key_len = argc - 2;	buf_size = MAX_BUF_SIZE - (MAX_BUF_SIZE % key_len);

	// retrieving keys from command line into the key array
	for(a = 0; a <  key_len; a++)
	{
		// converting from HEX-ASCII to long
		b = strtol(argv[a+2],&" ",16);

		if (b > 0xFF)		// checking for maximum allowed value
			{ fprintf(stderr, "-ERR: val %x  not a byte\x7\n",b); return -1; }

		key[a] = b;			// saving the next byte of the key
	}

	printf("build a key:"); // displaying the key on screen
	for(a=0;a<key_len;a++) printf("%02X",key[a]); printf("\n");

	// opening files for reading and writing	
	fin = fopen(argv[1],"rb"); fout=fopen(FDECODE,"wb");
	if ((fin==0) || (fout==0))
		{ fprintf(stderr, "-ERR: file open error\x7\n"); return -1; }

	// main processing loop
	while(real_size=fread(buf,1, buf_size, fin))
	{
		// loop by buffer
		for (a=0; a<real_size; a+=key_len)
		{	
			// loop by key
			for(b=0; b < key_len; b++)
				buf[a+b] ^= key[b];
		}

		// flashing encrypted (decrypted) byffer to disk
		if (0 == fwrite(buf, 1, real_size, fout))
		{
			fprintf(stderr,"-ERR: file write error\x7\n");
			return -1;
		}
	}
	// exiting
}
