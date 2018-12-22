/*----------------------------------------------------------------------------
 *
 *								print subcode
 *								=============
 *		prints subcodeds in convenient form (12 bytes per row)
 * 
--------------------------------------------------------------------------- */
#include <stdio.h>

#define _LEN_ (12)
main(int argc, char ** argv)
{
	int a,c,p;
	FILE *f;
	
	if (argc < 2) return 0;

	f=fopen(argv[1],"rb"); if (f==0) return 0;

	a=0; p=0;
	while ((c=fgetc(f))!=EOF)
	{
		if (a==0) printf("%08X: ",p);
		printf(" %02X",c); 
		if (a++==(_LEN_-1))
		{
			a=0; p+=_LEN_;
			printf("\n");
		}
	}
	
}