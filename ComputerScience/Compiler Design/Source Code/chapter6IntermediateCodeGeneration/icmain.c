#include "ic.h"
extern Label *labelTable;
extern BBlock *bblockTable;
void createLabelsListHeader(Label **labelListHeader)
{
 	*labelListHeader=(Label* )malloc(sizeof(Label));
 	strcpy((*labelListHeader)->name," ");
	(*labelListHeader)->quad=NULL;
	(*labelListHeader)->bblock=NULL;
	(*labelListHeader)->nextLabel=NULL;
	rearLabelPtr = *labelListHeader;
}

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
void createBBlockList(BBlock **bblockListHeader)
{
	*bblockListHeader=(BBlock *)malloc(sizeof(BBlock));
	(*bblockListHeader)->start=NULL;
	(*bblockListHeader)->end=NULL;
	(*bblockListHeader)->trueNext=NULL;
	(*bblockListHeader)->falseNext=NULL;
	(*bblockListHeader)->next=NULL;
	rearBBPtr = *bblockListHeader;
}

void addLabel(Label *labelListHeader,char *name, Quad *quad,BBlock *bblock) //int value,char type)
{
	Label *temp, *ptr;
 	temp=labelListHeader;
 	ptr=(Label*)malloc(sizeof(Label));
 	strcpy(ptr->name,name);
 	ptr->nextLabel=NULL;
 	ptr->quad=quad;
 	ptr->bblock=bblock;
	rearLabelPtr->nextLabel = ptr;
	rearLabelPtr = rearLabelPtr->nextLabel;
}


void addCode(Quad *quadListHeader,char *label,int operator,Attr operand1,Attr operand2,Attr result)
{
	int ret;
 	Quad *temp,*ptr;
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
                strcpy(label," ");              //reinitialize label to empty string
 	ptr->operator=operator;
 	memcpy(ptr->operand1,&operand1,sizeof(operand1));
 	memcpy(ptr->operand2,&operand2,sizeof(operand2));
 	memcpy(ptr->result,&result,sizeof(result));
 	temp->nextQuad=ptr;
}

void addBBlock(BBlock *bblockListHeader,Quad *start,Quad *end,char label[LABELSIZE])
{
 	BBlock *temp,*ptr;
	static blockCount=0;
 	temp=bblockListHeader;
 	ptr=(BBlock*)malloc(sizeof(BBlock));
	strcpy(ptr->label,label);
	ptr->blockNumber = ++blockCount;
 	ptr->start=start;
 	ptr->end=end;
 	ptr->trueNext=NULL;
 	ptr->falseNext=NULL;
 	ptr->next=NULL;
	if(label[0]=='L')
		addLabel(labelTable,label,start,ptr); //int value,char type)
	rearBBPtr->next = ptr;
	rearBBPtr = rearBBPtr->next;
}
void printLabelsList(Label *labelListHeader)
{
	Label *temp;
	printf("Printing Labels\n");
 	temp=labelListHeader->nextLabel;
 	while(temp!=NULL)
 	{
 		printf("%s\t",temp->name);
		temp=temp->nextLabel;
 	}
	printf("\n");
}

void printQuad(Quad *temp)
{
	if(strcmp(temp->label," "))
		printf("%s:\t",temp->label);
	else
		printf("\t");
 	if(temp->operator == NOP)
		printf("Nop\t");
	else
		printf("%s\t",ops[temp->operator]);
	//Printing Operand1
	if(temp->operand1->type==ID || temp->operand1->type==TID)
	{
		switch(temp->operand1->subtype)
		{
			case BASIC: 
				printf("%s\t",temp->operand1->name);
				break;
			case ARRAY:
				printf("%s\t",temp->operand1->name);
				break;
			case OFFSET:
				printf("%s\t",temp->operand1->offsetName);
				break;
			case ADDR:
				printf("%ld\t",temp->operand1->value.relAddr);
				break;
		}
	}
	else if(temp->operand1->type==NOP)
		printf("-\t");
	else if(temp->operand1->type==CONST)
	{
		switch(temp->operand1->datatype)
		{
			case CHAR:
                        	printf("%c\t",temp->operand1->value.cVal);
                        	break;
                	case INT:
	                        printf("%d\t",temp->operand1->value.iVal);
        	                break;
                	case FLOAT:
                        	printf("%f\t",temp->operand1->value.fVal);
                        	break;
                	case LONG:
                        	printf("%ld\t",temp->operand1->value.lVal);
                        	break;
                	case DOUBLE:
                        	printf("%lf\t",temp->operand1->value.dVal);
                        	break;
                	case STRING:
                        	printf("%s\t",temp->operand1->value.string);
                        	break;
		}
	}
	//Printing Operand2
	if(temp->operand2->type==ID || temp->operand2->type==TID)
	{
		switch(temp->operand1->subtype)
		{
			case BASIC: 
				printf("%s\t",temp->operand2->name);
				break;
			case ARRAY:
				printf("%s\t",temp->operand2->name);
				break;
			case OFFSET:
				printf("%s\t",temp->operand2->offsetName);
				break;
			case ADDR:
				printf("%ld\t",temp->operand2->value.relAddr);
				break;
		}
	}
	else if(temp->operand2->type==NOP)
		printf("-\t");
	else if(temp->operand2->type==CONST)
	{
		switch(temp->operand2->datatype)
		{
			case CHAR:
                        	printf("%c\t",temp->operand2->value.cVal);
                        	break;
                	case INT:
	                        printf("%d\t",temp->operand2->value.iVal);
        	                break;
                	case FLOAT:
                        	printf("%f\t",temp->operand2->value.fVal);
                        	break;
                	case LONG:
                        	printf("%ld\t",temp->operand2->value.lVal);
                        	break;
                	case DOUBLE:
                        	printf("%lf\t",temp->operand2->value.dVal);
                        	break;
                	case STRING:
                        	printf("%s\t",temp->operand2->value.string);
                        	break;
		}
	}
	//Printing result
	if(temp->result->type==ID || temp->result->type == TID || temp->result->type == LABEL)
		printf("%s\n",temp->result->name);
	else if(temp->result->type==NOP)
		printf("-\n");
	else if(temp->result->type==CONST)
	{
		switch(temp->result->datatype)
		{
			case CHAR:
                        	printf("%c ",temp->result->value.cVal);
                        	break;
                	case INT:
	                        printf("%d ",temp->result->value.iVal);
        	                break;
                	case FLOAT:
                        	printf("%f ",temp->result->value.fVal);
                        	break;
                	case LONG:
                        	printf("%ld ",temp->result->value.lVal);
                        	break;
                	case DOUBLE:
                        	printf("%lf ",temp->result->value.dVal);
                        	break;
                	case STRING:
                        	printf("%s",temp->result->value.string);
                        	break;
			case ADDR:
				printf("%ld",temp->result->value.relAddr);
				break;
		}
	}
	else
		printf("not id\n"); //to be expanded
}


void printCode(Quad *quadListHeader)
{
	Quad *temp;
 	temp=quadListHeader->nextQuad;
 	printf(" THE TABLE OF QUADRUPLES ARE\n\n");
 	printf("LABEL\tOPER\tOP1\tOP2\tRES\n\n");
 	while(temp!=NULL)
 	{
		printQuad(temp);
		temp=temp->nextQuad;
 	}	
}

void printBB(BBlock *bblockListHeader)
{
	Quad *start, *temp,*end;
	BBlock *bptr;
	int bcnt=0,ret;
	bptr = bblockListHeader;
	printf("..................................\n");
	printf("Printing the quadruples Blockwise\n");
	printf("..................................\n");
	while(bptr->next != NULL)
	{
		temp = bptr->next->start;
		end = bptr->next->end;
		printf("Block No:%d\n",bcnt++);
		while(1)
	        {
			printQuad(temp);
			if(temp == end)
				break;
			temp=temp->nextQuad;
		}
		bptr = bptr->next;
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
 	strcpy(temp,"L");
 	strcat(temp,no);
}

BBlock *findBlockForLabel(char label[LABELSIZE])
{
Label *temp;
BBlock *retPtr=NULL;
int retVal;
	//printf("searching the label for the Block\n");
	
 	temp=labelTable->nextLabel;
 	while(temp!=NULL)
 	{
		retVal = strcmp(temp->name,label);
		if(retVal == 0)
			return temp->bblock;
		temp=temp->nextLabel;
 	}
	return retPtr;
}
void split2BB(Quad *quadListHeader, BBlock **bblockListHeader)
{
	char label[5];
	BBlock *bptr;
	Quad *start,*end,*head,*qptr,*prev,*truePtr,*falsePtr;
	truePtr = NULL;
	falsePtr = NULL;
 	qptr=quadListHeader->nextQuad;
	prev = qptr;
	if(qptr == NULL) {printf("Empty Quad Table\n");return;}
	start = qptr;
	strcpy(label," ");
	while(qptr != NULL)
	{
		if(qptr->label[0] == 'L')
		{
			if(prev->operator != GOTO &&  prev->operator != IFEQ)
			{
				end = prev;
				addBBlock(*bblockListHeader,start,end,label);
			}
			strcpy(label,qptr->label);
			if(qptr->operator == NOP)
			{
				prev = qptr;
				qptr = qptr->nextQuad;
				start=qptr;
			}
			else
			{
				start = qptr;
			}
			if(qptr->operator == IFEQ || qptr->operator == GOTO)
			{
				end = qptr;
				addBBlock(*bblockListHeader,start,end,label);
				strcpy(label," ");
				prev = qptr;
				qptr = qptr->nextQuad;
				start = qptr;
			}
			else
			{
				prev = qptr;
				qptr = qptr->nextQuad;
			}
		}
		else if(qptr->operator == IFEQ) 
		{
			end = qptr;
			addBBlock(*bblockListHeader,start,end,label);
			strcpy(label," ");
			prev = qptr;
			qptr = qptr->nextQuad;
			start = qptr;
		}
		else if(qptr->operator == GOTO)
		{
			end = qptr;
			addBBlock(*bblockListHeader,start,end,label);
			strcpy(label," ");
			prev = qptr;
			qptr = qptr->nextQuad;
			start = qptr;
		}
		else
		{
			prev = qptr;
			qptr = qptr->nextQuad;
		}
	}
	end = prev;
	addBBlock(*bblockListHeader,start,end,label);
	return;
}

void printFlowGraph(BBlock *bblockListHeader)
{
BBlock *bptr;
	printf("Print Flow Graph\n");
	bptr =	bblockListHeader->next;
	if(bptr != NULL)
		printf("B%d\n",bptr->blockNumber);
	while(bptr != NULL)
	{
		if(bptr->trueNext != NULL)
			printf("B%d\t",bptr->trueNext->blockNumber);
		if(bptr->falseNext != NULL)
			printf("B%d",bptr->falseNext->blockNumber);
		printf("\n");
		bptr = bptr->next;
	}
}
void constructFlowGraph(BBlock *bblockListHeader)
{
BBlock *bptr;
	printf("Construct Flow Graph\n");
	bptr =	bblockListHeader->next;
	while(bptr != NULL)
	{
		if(bptr->end->operator == GOTO)
			bptr->trueNext = findBlockForLabel(bptr->end->result->name);
		else if(bptr->end->operator == IFEQ) 
		{
			bptr->trueNext = findBlockForLabel(bptr->end->result->name);
			bptr->falseNext = bptr->next;
		}
		else
			bptr->falseNext = bptr->next;
		bptr = bptr->next;
	}	
	return;
}

int main(int argc,char *argv[])
{
	extern FILE *yyin;
	strcpy(srcFileName,argv[1]);
	yyin=fopen(argv[1],"r");
	yyparse();
	return 1;
}
