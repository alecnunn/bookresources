#include<stdio.h>

#define U1 unsigned char
#define U8 __int64

#define TRUE		1
#define FALSE		0

#define FREE		0
#define RESERVED	1
char *statStr[] = {"FREE","RESERVED"};

/* Memory block header = 17 bytes = 8 previous + 8 next + status */

#define SIZE_HEADER	17

#define prev(i)		*((U8 *)&memory[i])
#define next(i)		*((U8 *)&memory[i+8])
#define status(i)	memory[i+16]
#define size(i)		next(i)-i-SIZE_HEADER

/* if going to split free block, need at least 8 bytes in new free part */

#define MIN_FREE_BYTES   8

U1 memory[1024];
U8 first;	/*stores address of first byte of heap*/
U8 last;	/*store address of last byte of heap + 1*/

void heapInit()
{
	first=5;
	last=1024;
	
	prev(first)=0;
	next(first)=0;
	status(first)=FREE;

	return;

}/*end heapInit*/

int currentNodeAlloc(U8 i,U8 nbytes)
{
	U8 size;

	/*handle case of current block being the last*/

	if(next(i)==0){ size = last-i-SIZE_HEADER; }
	else{ size = size(i); }

	/*either split current block, use entire current block, or fail*/

	if(size >= nbytes + SIZE_HEADER + MIN_FREE_BYTES)
	{
		U8 old_next;
		U8 old_block;
		U8 new_block;

		old_next = next(i);
		old_block = i;

		/*fix up original block*/

		next(i)=i+SIZE_HEADER+nbytes;	
		new_block = next(i);
		status(i)=RESERVED;

		/*set up new free block*/

		i = next(i);					
		next(i)=old_next;
		prev(i)=old_block;
		status(i)=FREE;

		/*right nieghbor must point to new free block*/

		if(next(i)!=0)			
		{
			i = next(i);
			prev(i)=new_block;
		}
		
		return(TRUE);
	}
	else if(size >= nbytes)
	{
		status(i)=RESERVED;
		return(TRUE);
	}

	return(FALSE);

}/*end currentNodeAlloc*/

U8 alloc(U8 nbytes)
{
	int ret;
	U8 i;

	i=first;

	if(status(i)==FREE)
	{
		ret = currentNodeAlloc(i,nbytes);
		if(ret==TRUE)
		{
			return(i+SIZE_HEADER);
		}
	}

	while(next(i)!=0)
	{
		i=next(i);
		if(status(i)==FREE)
		{
			ret = currentNodeAlloc(i,nbytes);
			if(ret==TRUE)
			{
				return(i+SIZE_HEADER);
			}
		}
	}

	return(0);

}/*end alloc*/

/*Note: disaster will strike if fed wrong address*/

void deAlloc(U8 address)
{
	U8 block;
	U8 lblock;
	U8 rblock;

	block = address-SIZE_HEADER;
	lblock = prev(block);
	rblock = next(block);

	/*
	4 cases: FFF->F, FFR->FR, RFF->RF, RFR 
	always want to merge free blocks 
	*/

	if((lblock!=0)&&(rblock!=0)&&(status(lblock)==FREE)&&(status(rblock)==FREE))
	{
		next(lblock)=next(rblock);
		status(lblock)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=lblock; }
	}
	else if((lblock!=0)&&(status(lblock)==FREE))
	{
		next(lblock)=next(block);
		status(lblock)=FREE;
		if(next(block)!=0){ prev(next(block))=lblock; }
	}
	else if((rblock!=0)&&(status(rblock)==FREE))
	{
		next(block)=next(rblock);
		status(block)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=block; }
	}
	else{ status(block)=FREE; }

	return;

}/*end deAlloc*/

void printMemory()
{
	U8 i;
	i=first;
	printf("[%I64u,%I64u,%I64u,%s]\n",prev(i),i,next(i),statStr[status(i)]);
	while(next(i)!=0)
	{
		i=next(i);
		printf("[%I64u,%I64u,%I64u,%s]\n",prev(i),i,next(i),statStr[status(i)]);
	}
	return;

}/*end printMemory*/

void main()
{
	U8 address[10];
	heapInit();

	printf("Allocating----------------\n");
	address[0]=alloc(10); printf("\taddress= %I64u\n",address[0]);
	address[1]=alloc(20); printf("\taddress= %I64u\n",address[1]);
	address[2]=alloc(15); printf("\taddress= %I64u\n",address[2]);
	address[3]=alloc(40); printf("\taddress= %I64u\n",address[3]);
	address[4]=alloc(5); printf("\taddress= %I64u\n",address[4]);
	address[5]=alloc(1); printf("\taddress= %I64u\n",address[5]);
	address[6]=alloc(2); printf("\taddress= %I64u\n",address[6]);
	address[7]=alloc(200); printf("\taddress= %I64u\n",address[7]);
	address[8]=alloc(1); printf("\taddress= %I64u\n",address[8]);
	printf("Dump map------------------\n");
	printMemory();
	printf("\nDeallocating address[8]+++\n");deAlloc(address[8]);printMemory();
	printf("\nDeallocating address[7]+++\n");deAlloc(address[7]);printMemory();
	printf("\nDeallocating address[1]+++\n");deAlloc(address[1]);printMemory();
	printf("\nDeallocating address[3]+++\n");deAlloc(address[3]);printMemory();
	printf("\nDeallocating address[5]+++\n");deAlloc(address[5]);printMemory();
	printf("\nDeallocating address[4]+++\n");deAlloc(address[4]);printMemory();
	printf("\nDeallocating address[0]+++\n");deAlloc(address[0]);printMemory();
	printf("\nDeallocating address[2]+++\n");deAlloc(address[2]);printMemory();
	printf("\nDeallocating address[6]+++\n");deAlloc(address[6]);printMemory();
	return;

}/*end main*/