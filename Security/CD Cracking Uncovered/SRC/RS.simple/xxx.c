#include <stdio.h>

char table[16][17];
#define magic  23

init_table()
{
	int a, b, c;
	for (a = 0; a < 16; a++)
	{
		c = a * magic;
		for (b = 15; b >= 0; b--)
		{
			table[a][15-b] = (c & (1<<b))?'1':'0';
		} table[a][16] = 0;
	}
}

printf_bin(int a, int len)
{
	int b;
	for(b = len; b >= 0; b--) printf("%x",(a & (1<<b))?1:0);
}

my_compare(char *bit_string)
{
	int a, b, c, c_min, idx; c_min = 999;
	printf("%s\n",bit_string);
	for (a = 0; a < 16; a++)
	{
		printf("%d:",a); printf_bin(a * magic, 15);
		c = 0;
		for (b = 0; b < 16; b++)
			if (bit_string[b] != table[a][b]) c++;
		printf("(%d)\n", c);
		if (c_min > c) { c_min = c; idx = a; }
	} 
	return idx;
}


main(int argc, char** argv)
{
	int a;
	int c;
	if (argc <2)
	{
		printf("USAGE:\nxxx.exe +NUM (for encoder)\nxxx.exe bit_string (for decoder)\n");
		printf("for example:\nxxx.exe +12\nxxx.exe 000000001101001\n");
		return -1;
	}


	if (argv[1][0]=='+')
	{
		a = atol(argv[1]);
		printf("%d * %d ==", a, magic); printf_bin(a * magic, 15); printf("\n");	
		return 0;
	}

    init_table();
    for (a=0; a< 16; a++) printf("%s\n",table[a]);
    printf("recorver: %d\n", my_compare(argv[1]));

}
