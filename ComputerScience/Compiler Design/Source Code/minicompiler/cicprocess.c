#include "cic.h"

void createQuadList(Quad **quadListHeader)
{
	*quadListHeader=(Quad *)malloc(sizeof(Quad));
	(*quadListHeader)->label=NULL;
	(*quadListHeader)->operator=-1;
	(*quadListHeader)->operand1=NULL;
	(*quadListHeader)->operand2=NULL;
	(*quadListHeader)->result=NULL;
	(*quadListHeader)->nextQuad=NULL;
}


void addCode(Quad *quadListHeader,char *label,int operator,Attr operand1,Attr operand2,Attr result)
{
	Quad *temp,*ptr;
	static quadCount;
 	temp=quadListHeader;
 	
	while(temp->nextQuad!=NULL) 
		temp=temp->nextQuad;

 	ptr=(Quad*)malloc(sizeof(Quad));

 	ptr->label=(char *)malloc(strlen(label)+1);
	ptr->operand1 = malloc(sizeof(operand1));
	ptr->operand2 = malloc(sizeof(operand2));
	ptr->result = malloc(sizeof(result));
 	ptr->nextQuad=NULL;

 	strcpy(ptr->label,label);
 	if(strcmp(label," ")!=0)
		strcpy(label," ");
 	
	ptr->operator=operator;
 	memcpy(ptr->operand1,&operand1,sizeof(operand1));
 	memcpy(ptr->operand2,&operand2,sizeof(operand2));
 	memcpy(ptr->result,&result,sizeof(result));
 	temp->nextQuad=ptr;
	quadCount++;
}


void printCode(Quad *quadListHeader)
{
	Quad *temp;
	
 	temp=quadListHeader->nextQuad;
 	printf(" THE TABLE OF QUADRUPLES ARE\n\n");
 	printf("LABEL\tOPER\tOP1\tOP2\tRES\n\n");

 	while(temp!=NULL)
 	{	
		if(strcmp(temp->label," "))
			printf("%s\t",temp->label);
		else
			printf("\t");
 		printf("%s\t%s\t%s",ops[temp->operator],temp->operand1->name,temp->operand2->name);
		if(temp->result->type != FUNCTION)		
			printf("\t%s\n",temp->result->name);
		else
			printf("\n");
		temp=temp->nextQuad;
 	}
}


short int cstackpush(char *label)
{
	if(cstackTop < STACKSIZE)
	{
		strcpy(cstack[++cstackTop],label);
		return 1;
	}
	else
	{
		printf("Continue Statement Stack overflow..Unpredicted result may occur!\n");
		return -1;
	}
}

short int cstackpop()
{
	if(cstackTop >= 0)
	{
		cstackTop--;
		return 1;
	}
	else
	{
		printf("Continue Statment Stack underflow....Unpredicted result may occur!\n");
		return -1;
	}
}

short int bstackpush(char *label)
{
	if(bstackTop < STACKSIZE)
	{
		strcpy(bstack[++bstackTop],label);
		return 1;
	}
	else
	{
		printf("Break Statement Stack overflow..Unpredicted result may occur!\n");
		return -1;
	}
}

short int bstackpop()
{
	if(bstackTop >= 0)
	{
		bstackTop--;
		return 1;
	}
	else
	{
		printf("Break Stack underflow....Unpredicted result may occur!\n");
		return -1;
	}
}

void itoa(int n, char *name)
{
	int i=0,j;
 	int new=n;
 	if(n==0)
 		strcpy(name,"0");
 	else
 	{
 		while(n)
 		{
  			i++;
  			n=n/10;
		}
 		n=new;
 		for(j=i-1;j>=0;j--)
 		{
  			name[j]=n%10+48;
  			n=n/10;
 		}
 		name[i]='\0';
 	}	
}

void createTemp(char temp[])
{
	static int i=0;
 	char no[5];
 	i++;
 	itoa(i,no);
 	strcpy(temp,"t");
 	strcat(temp,no);
}
  
void createLabel(char temp[])
{
	static int i=0;
 	char no[5];
 	i++;
 	itoa(i,no);
 	strcpy(temp,"CL");
 	strcat(temp,no);
	strcat(temp,":");
}

int main(int argc,char *argv[])
{
	extern FILE *yyin;
	Quad *quadlist;

	strcpy(srcFileName,argv[1]);
	yyin=fopen(argv[1],"r");
	yyparse();
	genCodeFromQuad(quadTable);
	createElfFile(argv[2]);
	return 1;
}

