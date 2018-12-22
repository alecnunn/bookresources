#include <stdio.h>

printf_bin(int a)
{
	int b;
	for(b = 15; b >= 0; b--) printf("%x",(a & (1<<b))?1:0);printf(" %x\n",a);
}


//gen_table(

char encoder(int data)
{
	int a;
	int power = 1;
	int long_crc = 0;

	printf("DATA:"); printf_bin(data);
	printf("CRC :"); printf_bin(data % 32);
	return data % 32;

}

char decoder(int data, char crc)
{
	printf("(%x)\n",data /32);
	printf("%d\n", ((data % 31) + ((data/31) % 31)) - crc);
}

main()
{
	int CRC;
	int a;
	CRC = encoder(0x0);
	
	for (a=0;a < 16; a++)
    decoder(0x0 | (1<<a), CRC); 

}