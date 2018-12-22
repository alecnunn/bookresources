#include <stdio.h>

#define MAX_STR_SIZE 0x666			// Max. string length
#define MIN_STR_SIZE 0x5			// Min. string length

int main(int argc, char **argv)
{
	FILE *f;
	int ch, p=0;
	char buf[MAX_STR_SIZE];

	if ((argc > 0) && (f=fopen(argv[1], "rb")))
	while (( (ch=getc(f)) != EOF ))
	{
		buf[p]=ch;
		if (p==MAX_STR_SIZE) ch = 0;

		if (((ch < ' ') || (ch > 'z')))
		{
		 	if (p>=MIN_STR_SIZE)
		 		printf("%08X:%s\n",ftell(f),buf, buf[p] = 0);
			p = 0;
		}
		else
        	p++;
	}

}		
		
		

