#include <math.h>
#include <stdio.h>

#define POLY (1<<15 | 1<<1 | 1<<0)
main()
{
	int a,b,t;
	int reg=1;
	int poly=POLY;
	for (a=0; a < 128; a++)
	{
		printf("POLY:");for (b=15; b >= 0; b--) printf("%x",(POLY & (1<<b))?1:0);printf("\n");
   		printf("REG=:");for (b=15; b >= 0; b--) printf("%x",(reg & (1<<b))?1:0);printf("\n");
		reg = reg ^ POLY;
		
        printf("REG^:");for (b=15; b >=0; b--) printf("%x",(reg & (1<<b))?1:0);printf("\n");
        t=reg;
        //reg = reg | reg<<16;
        reg = reg >> 1; 





        printf("REG>:");for (b=15; b >=0; b--) printf("%x",(reg & (1<<b))?1:0);printf("\n");
		printf("%d                   %x\\%x\n",a, (reg & 1) ^ (POLY & 1), (t & 1) ^ (POLY & 1));
  		printf("\n");


	}

}