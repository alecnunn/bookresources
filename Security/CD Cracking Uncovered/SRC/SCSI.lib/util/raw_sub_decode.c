#include <stdio.h>

#define PACK_SIZE		12
#define N_SUB_BIT		8
#define RAW_PACK_SIZE	(PACK_SIZE*N_SUB_BIT)

main()
{
	FILE *f;
	int c;
	int a;
	int sub;
	int k;
	int pack = 0;
	unsigned char RW_SUB[PACK_SIZE][N_SUB_BIT];
	unsigned char buf[RAW_PACK_SIZE];

	f = fopen("sector.sub", "rw");

	if (f==0) return 0;

	while (fread(buf, 1, RAW_PACK_SIZE, f) == RAW_PACK_SIZE)
	{
	    memset(RW_SUB, 0,RAW_PACK_SIZE);
		for (k = 0; k < PACK_SIZE; k++)
		{	
			for (a = k*N_SUB_BIT; a < ((k+1)*N_SUB_BIT); a++)
			{
				for (sub = (N_SUB_BIT - 1); sub >= 0 ; sub--)
				{
		        	RW_SUB[k][sub] = RW_SUB[k][sub]<< 1;
					RW_SUB[k][sub] = RW_SUB[k][sub] | (buf[a] & 1);
					buf[a] = buf[a] >> 1;
				}		
			}
		
		}

		for (sub = 0; sub < N_SUB_BIT; sub++)
		{		
			printf("PACK%4d(%c):",pack,'P'+sub);
			for (a = 0; a < PACK_SIZE; a++) printf("%02X ", RW_SUB[a][sub]);
			printf("\n");
		}
		pack++;
	}
	
}
