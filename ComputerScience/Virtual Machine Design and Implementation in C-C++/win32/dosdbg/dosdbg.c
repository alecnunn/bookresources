#include<stdio.h>

/*global variables-------------------------------------------------*/

char INT1_STR[]="->SINGLED-STEP<-\n"; 
char INT3_STR[]="->BREAKPOINT<-\n"; 

unsigned short rCS,rSS,rDS;
short rAX,rBX,rCX,rDX;
unsigned short rIP;

unsigned char traceOn=0;
    
/*prototypes-------------------------------------------------------*/

void procCmd(int *lptr);

/*definitions------------------------------------------------------*/

void main()
{
	unsigned short oldInt1Seg;
	unsigned short oldInt1Offset;
	unsigned short oldInt3Seg;
	unsigned short oldInt3Offset;
	
	goto past_interrupts;
	   
	/* Handle INT 1 -----------------------------------------------*/ 
	
	int1:
	__asm
	{ 
		STI
		MOV rCS,CS
		MOV rSS,SS
		MOV rDS,DS
		MOV rAX,AX
		MOV rBX,BX
		MOV rCX,CX
		MOV rDX,DX
		
		POP CX
		MOV rIP,CX
		POP DX
		POP AX
		AND AX,65279
		PUSH AX
		PUSH DX
		PUSH CX	 
	}
	
	printf("%s",INT1_STR);
	printf("next instruction at IP=%u\n",rIP);  
	{
		int loop = 1;
	
		while(loop)
		{
			procCmd(&loop);	
		}
	}
	
	if(traceOn)
	{
		__asm
		{
			POP CX
			POP DX
			POP AX
			OR AX,256
			PUSH AX
			PUSH DX
			PUSH CX	 	
		}	
	}
	
	__asm
	{
		MOV AX,rAX
		MOV BX,rBX
		MOV CX,rCX
		MOV DX,rDX	
		IRET 
	}
	   
	/* Handle INT 3 -----------------------------------------------*/
	
	int3:
	__asm
	{ 
		STI
		MOV rCS,CS
		MOV rSS,SS
		MOV rDS,DS
		MOV rAX,AX
		MOV rBX,BX
		MOV rCX,CX
		MOV rDX,DX	
		
		POP CX
		POP DX
		POP AX
		AND AX,65279
		PUSH AX
		PUSH DX
		PUSH CX	  
	}
	
	printf("%s",INT3_STR);  
	{
		int loop = 1;
	
		while(loop)
		{
			procCmd(&loop);	
		}
	}
	
	if(traceOn)
	{
		__asm
		{
			POP CX
			POP DX
			POP AX
			OR AX,256
			PUSH AX
			PUSH DX
			PUSH CX	 	
		}	
	}
	
	__asm
	{
		MOV AX,rAX
		MOV BX,rBX
		MOV CX,rCX
		MOV DX,rDX	
		IRET 
	}
	
	/* Execution path begins here  --------------------------------*/
	
	past_interrupts:
	
	printf("Save old interrupts SEG:OFF\n"); 
    
	__asm
	{
		MOV AH,0x35
		MOV AL,0x1
		INT 0x21
		MOV oldInt1Seg,ES
		MOV oldInt1Offset,BX
		
		MOV AH,0x35
		MOV AL,0x3
		INT 0x21
		MOV oldInt3Seg,ES
		MOV oldInt3Offset,BX
	}  
	
	printf("Load new interrupts SEG:OFF\n");    
	
	__asm
	{
		MOV AH,0x25
		MOV AL,0x1
		PUSH DS
		MOV CX,CS
		MOV DS,CX
		MOV DX,OFFSET int1
		INT 0x21
		POP DS
		
		MOV AH,0x25
		MOV AL,0x3
		PUSH DS
		MOV CX,CS
		MOV DS,CX
		MOV DX,OFFSET int3
		INT 0x21
		POP DS
	}
	 
	/* actually do something here to provoke debugger -------------*/ 
	
	__asm
	{
		INT 3
		
		MOV DX,20  
		INC DX
		NOP
		MOV DX,3501
		MOV DX,72  
		DEC DX
	}
    
    printf("Re-loading old interrupts SEG:OFF\n");   
    
	__asm
	{   
		PUSH DS
		MOV AH,0x25
		MOV AL,0x1
		MOV DS,oldInt1Seg
		MOV DX,oldInt1Offset 
		INT 0x21
		POP DS
		
		PUSH DS
		MOV AH,0x25
		MOV AL,0x3
		MOV DS,oldInt3Seg
		MOV DX,oldInt3Offset  
		INT 0x21
		POP DS
	}
	return; 
}/*end main*/  

void procCmd(int *lptr)
{   
	char ch;
	    
	traceOn=0;  
	
	printf("dbg>");
	scanf("%c",&ch); 
	fflush(stdin);   
	
	switch(ch)
	{
		case 'a':
		{
			printf("INT1_STR address=%u\n",INT1_STR);	
		}break;
		case 'd':
		{
			printf("CS=%u\n",rCS);
			printf("SS=%u\n",rSS);
			printf("DS=%u\n",rDS);
			printf("AX=%d\n",rAX);
			printf("BX=%d\n",rBX);
			printf("CX=%d\n",rCX);
			printf("DX=%d\n",rDX);
		}break;
		case 'i':
		{
			rAX++;
			printf("AX=%d\n",rAX);
		}break;
		case 'm':
		{   
			int i;
			unsigned long address; 
			unsigned long limit = rCS+65535;
			unsigned char *sptr;
			printf("memory address>");
			scanf("%lu",&address);
			fflush(stdin);
			sptr = (unsigned char*)address;
			printf("address=%u\n",sptr); 
			if(address > limit)
			{
				printf("address is beyond .COM segment\n");
			}
			else
			{
				for(i=0;i<16;i++)
				{ 
					if((sptr[i]>0x20)&&(sptr[i]<0x7F))
					{ 
						printf("byte[%lu]=%c\n",address+i,sptr[i]); 
					}
					else
					{
						printf("byte[%lu]=%X\n",address+i,sptr[i]); 	
					}
				}
			}
		}break;
		case 'q':
		{ 
			*lptr=0; 
		}break;   
		case 't':
		{          
			traceOn=1; 
			printf("trace flag set\n");
			*lptr=0;
		}break;
		default:
		{ 
			printf("not valid command\n"); 
		}
	} 
	return;
}/*end procCmd*/