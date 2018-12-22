#include<stdio.h>
#include<stdlib.h>

#define U1 unsigned char

void printBinary(U1 nbr)
{
	U1 i;
	printf("[");
	for(i=128;i>0;i=i/2)
	{
		if(nbr&i){ printf("1"); }
		else{ printf("0"); }
	}
	printf("]");
	return;
}/*end printBinary*/

/*-----------------------------------------------------------------*/

void printFloat(char *str)
{
	float f;
	U1 *uptr;
	int i;

	f = (float)atof(str);
	uptr = (U1*)&f;

	printf("FLOAT=%f\n",f);
	
	printf("DEC [0]->[3]\t");
	for(i=0;i<4;i++){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("DEC [3]->[0]\t");
	for(i=3;i>=0;i--){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("HEX [0]->[3]\t");
	for(i=0;i<4;i++){ printf("[%4x]",uptr[i]); }
	printf("\n");

	for(i=0;i<4;i++)
	{ 
		printf("BIN [%u]\t",i);
		printBinary(uptr[i]);
		printf("\n");
	}
	printf("\n");

	printf("FLOAT=%f\n",*((float*)uptr));

	return;
}/*end printFloat*/

/*-----------------------------------------------------------------*/

void printDouble(char *str)
{
	double d;
	U1 *uptr;
	int i;

	d = atof(str);
	uptr = (U1*)&d;

	printf("DOUBLE=%f\n",d);
	
	printf("DEC [0]->[7]\t");
	for(i=0;i<8;i++){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("DEC [7]->[0]\t");
	for(i=7;i>=0;i--){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("HEX [0]->[7]\t");
	for(i=0;i<8;i++){ printf("[%4x]",uptr[i]); }
	printf("\n");

	for(i=0;i<8;i++)
	{ 
		printf("BIN [%u]\t",i);
		printBinary(uptr[i]);
		printf("\n");
	}
	printf("\n");

	printf("DOUBLE=%f\n",*((double*)uptr));
	return;
}/*end printDouble*/

/*-----------------------------------------------------------------*/

void printInteger(char *str)
{
	__int64 I;
	U1 *uptr;
	int i;

	I = _atoi64(str);
	uptr = (U1*)&I;

	printf("S8=%I64d\n",I);
	
	printf("DEC [0]->[7]\t");
	for(i=0;i<8;i++){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("DEC [7]->[0]\t");
	for(i=7;i>=0;i--){ printf("[%+4d]",uptr[i]); }
	printf("\n");

	printf("HEX [0]->[7]\t");
	for(i=0;i<8;i++){ printf("[%4x]",uptr[i]); }
	printf("\n");

	for(i=0;i<8;i++)
	{ 
		printf("BIN [%u]\t",i);
		printBinary(uptr[i]);
		printf("\n");
	}
	printf("\n");

	printf("S8=%I64d\n",*((__int64*)uptr));
	return;
}/*end printInteger*/

/*-----------------------------------------------------------------*/

void printChar(char *str)
{
	char u;
	U1 *uptr;

	u = str[0];
	uptr = &u;

	printf("CHAR=%c\n",u);

	printf("DEC\t");
	printf("[%-4d]",uptr[0]);
	printf("\n");

	printf("HEX\t");
	printf("[%-4x]",uptr[0]); 
	printf("\n");

	printf("BIN\t");
	printBinary(uptr[0]);
	printf("\n");

	printf("CHAR=%c\n",*uptr);
	return;
}/*end printChar*/

/*-----------------------------------------------------------------*/

void printWideChar(char *str)
{
	__int64 I;
	wchar_t wc;
	U1 *uptr;

	I = _atoi64(str);
	wc = (wchar_t)I;
	uptr = (U1*)&wc;

	wprintf(L"CHAR=%c\n",wc);

	printf("DEC [0][1]\t");
	printf("[%-4d]",uptr[0]);
	printf("[%-4d]",uptr[1]);
	printf("\n");

	printf("HEX [0][1]\t");
	printf("[%-4x]",uptr[0]); 
	printf("[%-4x]",uptr[1]); 
	printf("\n");

	printf("BIN [0][1]\t");
	printBinary(uptr[0]);
	printBinary(uptr[1]);
	printf("\n");

	return;
}/*end printChar*/

/*-----------------------------------------------------------------*/

void printUsage()
{
	printf("\n\tusage: bformat type  value\n\n");
	printf("\tdisplays byte-wise decomposition of value\n\n");
	printf("\ttype:\n");
	printf("\td=double\n");
	printf("\tf=float\n");
	printf("\ti=integer\n");
	printf("\tc=ASCII char\n");
	printf("\tw=16-bit char\n\n");
	return;
}/*end printUsage*/

/*-----------------------------------------------------------------*/

void main(int argc, char*arg[])
{
	char opcode;
	if((arg[1]==NULL)||(arg[2]==NULL))
	{
		printf("Not enough arguments\n");
		printUsage();
		return;
	}
	opcode = arg[1][0];
	switch(opcode)
	{
		case 'f':{printFloat(arg[2]);}break;
		case 'd':{printDouble(arg[2]);}break;
		case 'i':{printInteger(arg[2]);}break;
		case 'c':{printChar(arg[2]);}break;
		case 'w':{printWideChar(arg[2]);}break;
		default:
		{
			printf("Bad opcode");
			printUsage();
			return;
		}
	}
	return;

}/*end main*/