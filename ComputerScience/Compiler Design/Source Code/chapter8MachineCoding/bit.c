#include<stdio.h>
#include<stdlib.h>
#define MAXCODE 100
typedef unsigned char USC; 
USC code[MAXCODE]; 
USC data[4];
short int dataIdx;
unsigned int cc=0;
unsigned int startCount,endCount;
			

//do effective Address Calculation
void doEAC(short int mod,short int rm)
{
	dataIdx=0;
	switch(mod)
	{
		case 3:
			break;
		case 0:
			if(rm == 6) //110 - direct address
			{
				code[cc++] = data[dataIdx++];
				code[cc++] = data[dataIdx++];
			}
			break;
		case 1:
			code[cc++] = data[dataIdx++];
			break;
		case 2:
			code[cc++] = data[dataIdx++];
			code[cc++] = data[dataIdx++];
			break;
	}
}

//register or memory to or from register
void moveRM2R(USC d,USC w,USC mod,USC reg, USC rm)
{
	USC c;
	// 1 0 0 0 1 0 d w - mod reg r/m
	c = 0x88;
	c  = c | (d << 1);
	c  = c | w;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | (reg << 3);
	c = c | rm ;
	code[cc++] = c;
	doEAC(mod,rm);
}		


//Immediate register or memory
void moveImm2RM(USC w, USC mod, USC rm)
{
	USC c;
	c = 0xc6;
	c = c | w;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | rm;
	code[cc++] = c;
	doEAC(mod,rm);
	code[cc++] = data[dataIdx++];
	if(w==1)
		code[cc++] = data[dataIdx++];
}

//move immediate to register
void moveImm2R(USC w,USC reg)
{
USC c;
	c = 0xb0;
	c = c | (w << 3);
	c = c | reg;
	code[cc++]=c;
	dataIdx = 0;
	code[cc++] = data[dataIdx++];
	if(w==1)
		code[cc++] = data[dataIdx++];
}	
	
//Move Memory to accumulator
void moveMem2Acc(USC w)
{
USC c;
	c = 0xb0;
	c = c | w;
	code[cc++] = c;
	dataIdx = 0;
	code[cc++] = data[dataIdx++];
	code[cc++] = data[dataIdx++];
}	
	
//Move accumulator to memory
void moveAcc2Mem(USC w)
{
USC c;
	c = 0xb2;
	c = c | w;
	code[cc++] = c;
	dataIdx = 0;
	code[cc++] = data[dataIdx++];
	code[cc++] = data[dataIdx++];
}	

//Move register or memory to segment register
void moveRM2SegR(USC mod, USC sreg, USC rm)
{
USC c;
	c = 0x8e;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | (sreg << 3);
	c = c | rm;
	code[cc++]=c;
	doEAC(mod,rm);
}	

//Move  segment register to register or memory 
void moveSegR2RM(USC mod, USC sreg, USC rm)
{
USC c;
	c = 0x8c;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | (sreg << 3);
	c = c | rm;
	code[cc++]=c;
	doEAC(mod,rm);
}	

//push data on reg or memory  on stack
void pushRM(USC mod, USC rm)
{
USC c;
	c = 0xff;
	code[cc++] = c;
	c = 0x00;
	c =  c | (mod << 6);
	c =  c | (0x06 << 3); //mod 110 rm
	c =  c | rm;
	code[cc++] = c;
	doEAC(mod,rm);
}
	
//push data on reg on stack
void pushR(USC reg)
{
USC c;
	c = 0x40;
	c =  c | reg;
	code[cc++] = c;
}
//push segment reg on stack
void pushSR(USC sreg)
{
USC c;
	c = 0x0;
	c =  c | (sreg << 3);
	c =  c | 0x6;        //000 reg 110
	code[cc++] = c;
}

//pop stack into reg or mem
//1000 1111 - mod 000 r/m
void pop2RM(USC mod, USC rm)
{
USC c;
	c = 0x8f;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | rm;
	code[cc++] = c;
	doEAC(mod,rm);
}

//pop into a reg
//0101 1reg
void pop2R(USC reg)
{
USC c;
	c = 0x58;
	c = c | reg;
	code[cc++] = c;
}

//pop into a seg reg
//000 sreg 111
void pop2SR(USC sreg)
{
USC c;
	c = 0x0;
	c = c | (sreg << 3);
	c = c | 0x07;
	code[cc++] = c;
}

//exchange register or memory with register
// 1000 011w - mod reg rm
void exchangeRM2R(USC w, USC mod, USC reg, USC rm)
{
USC c;
	c = 0x86;
	c = c | w;
	code[cc++] = c;
	c = 0x0;
	c = c | (mod << 6);
	c = c | (reg << 3);
	c = c | reg;
	code[cc++] = c;
	doEAC(mod,rm);
}

void exchangeR2Acc(USC reg)
{
USC c;
	c = 0x90;
	c = c | reg;
	code[cc++] = c;
}
void printCode()
{
	int i;
	for(i=startCount;i<endCount;i++)
		printf("\t%0x\t",code[i]);
	printf("\n");
}
int main()
{ 
int i;
USC d,w,mod,reg,sreg,rm,data1,data2,addrlow,addrhigh;
	

	
	//register or memory to or from register
	//void moveRM2R(USC d,USC w,USC mod,USC reg, USC rm)
	startCount = cc;
	printf("\nMOV CX,AX Move AX register to CX register\n"); 
	d = 0x01;
	w = 0x01;
	mod = 0x11;
	rm = 0x01;
	reg = 0x00;
	moveRM2R(d,w,mod,reg,rm);
	endCount = cc;
	printCode();
	
	//Move Immediate data to register or memory
	//void moveImm2RM(USC w, USC mod, USC rm, USC data1, USC data2)
	startCount = cc;
	printf("\nMOV DX, #4050 - move an immediate data to DX reg\n");	
	w = 0x01;
	mod = 0x11;
	rm = 0x03;
	data[0] = 0x40;
	data[1]= 0x50;
	moveImm2RM(w, mod, rm);
	endCount=cc;
	printCode();

	//move immediate to register
	//void moveImm2R(USC w,USC reg)
	printf("\nMov SI, #4050 - Move a immediate number to SI index reg\n");
	startCount = cc;
	w = 1;
	reg = 0x06;
	data[0] = 0x40;
	data[1] = 0x50;
	moveImm2R(w,reg);
	endCount=cc;
	printCode();
	
	printf("\nMov AX, 2025 - Move the contents of the memory 2025 into accumulator\n");
	startCount = cc;
	w = 1;
	//Move Memory to accumulator
	//void moveMem2Acc(USC w)
	w = 0x01;
	data[0] = 0x25;    //addrlow
	data[1] = 0x20;    //addrhigh;
	moveMem2Acc(w);
	endCount=cc;
	printCode();
	
	//Move accumulator to memory
	//void moveAcc2Mem(USC w)
	printf("\nMove the content of acc to a mem addressed by 2025\n");
	startCount = cc;
	w = 0x01;
	data[0] = 0x25;    //addrlow
	data[1] = 0x20;    //addrhigh;
	moveAcc2Mem(w);
	endCount=cc;
	printCode();

	//Move register or memory to segment register
	//void moveRM2SegR(USC mod, USC sreg, USC rm)
	printf("\nMov SS, BX - Move the contents of BX register to DS\n");
	startCount=cc;
	mod = 0x03;
	sreg = 0x02;
	rm = 0x03;
	moveRM2SegR(mod,sreg,rm);
	endCount=cc;
	printCode();

	//Move  segment register to register or memory 
	//void moveSegR2RM(USC mod, USC sreg, USC rm)
	printf("\nMov BX,SS - Move the contents of SS to BX\n");
	startCount = cc;
	mod = 0x03;
	sreg = 0x02;
	rm = 0x03;
	moveSegR2RM(mod,sreg,rm);
	endCount=cc;
	printCode();
	
	//push data on reg or memory  on stack
	//void pushRM(USC mod, USC rm)
	printf("\nPush SI - Push SI contents onto stack\n");
	startCount = cc;
	mod = 0x03;
	rm = 0x06;
	pushRM(mod,rm);
	endCount=cc;
	printCode();

	//push data on reg on stack
	//void pushR(USC reg)
	printf("\nPush DI - Push DI contents onto Stack\n");
	startCount = cc;
	reg = 0x07;
	pushR(reg);
	endCount=cc;
	printCode();

	//push segment reg on stack
	//void pushSR(USC sreg)
	printf("\nPush DS - Push Segment Register onto stack\n");
	startCount = cc;
	sreg = 0x03;
	pushSR(sreg);
	endCount=cc;
	printCode();

	//pop stack into reg or mem
	//1000 1111 - mod 000 r/m
	//void pop2RM(USC mod, USC rm)
	printf("\nPop SI- Pop the top of the stack into SI reg\n");
	startCount = cc;
	mod = 0x03;
	rm = 0x06;
	pop2RM(mod,rm);
	endCount=cc;
	printCode();

	//pop into a reg
	//0101 1reg
	//void pop2R(USC reg)
	printf("\nPop DX - Pop the top into DX reg\n");
	startCount=cc;
	reg = 0x2;
	pop2R(reg);
	endCount=cc;
	printCode();

	//pop into a seg reg
	//000 sreg 111
	//void pop2SR(USC sreg)
	printf("\nPop ES - pop the top contents into ES reg\n");
	startCount = cc;
	sreg = 0x03;
	pop2SR(sreg);
	endCount=cc;
	printCode();

	startCount = cc;
	printf("\nXCNG DI -Exchange Register DI with Accumulator\n");
	reg = 0x07;
	exchangeR2Acc(reg);
	endCount = cc;
	printCode();

	//exchange register or memory with register
	// 1000 011w - mod reg rm
	//void exchangeRM2R(USC w, USC mod, USC reg, USC rm)
	printf("\nEXNG SI, DI -Exchange Register DI and SI\n");
	startCount = cc;
 	w = 0x1;
	mod = 0x03;
	reg = 0x6;
	rm  = 0x7;
	exchangeRM2R(w,mod,reg,rm);
	endCount=cc;
	printCode();
	return 1;
}
