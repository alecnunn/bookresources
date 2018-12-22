#include "cic.h"
void initHash(ST* st,int size)
{
int ii;
        hsize = size;
        st->HashTable = (List*)malloc(sizeof(List) * hsize);

        if(st->HashTable == NULL)
        {
                printf("Memory allocation problem\n");
                exit(-1);
        }
        for(ii=0;ii<hsize;ii++)
        {
                st->HashTable[ii]=(LNode*)malloc(sizeof(LNode));
                if(st->HashTable[ii] == NULL)
                {
                        printf("error in memory allocation for hash table\n");
                        exit(-1);
                }
                st->HashTable[ii]->next = NULL;
                st->HashTable[ii]->symbol.name=NULL;
                st->HashTable[ii]->symbol.nameLen = 0;
                st->HashTable[ii]->symbol.storage = DEFAULT;
                st->HashTable[ii]->symbol.qualifier = DEFAULT;
                st->HashTable[ii]->symbol.sign = DEFAULT;
                st->HashTable[ii]->symbol.type = DEFAULT;
		st->HashTable[ii]->symbol.subtype = DEFAULT;
		st->HashTable[ii]->symbol.widthType = DEFAULT;
                st->HashTable[ii]->symbol.width = 0;
		st->HashTable[ii]->symbol.structLink = NULL;
		st->HashTable[ii]->symbol.st = NULL;
        }
}

void destroySymbolTable(ST* st,int size)
{
int ii;
List L,tmp;
        for(ii = 0;ii<size;ii++)
        {
                L = st->HashTable[ii];
                while (L != NULL)
                {
                        tmp = L;
 			L = L->next;
                        free(L);
                }
        }
}

void dispSymbolTableHash(ST* st)
{
List L;
int ii;

	while(st!=NULL)
	{
	        
		printf("Name\tType\tsubtype\tWidth\n");
		for(ii=0;ii<hsize;ii++)
		{ 
		        L = st->HashTable[ii];
		        while(L->next != NULL)
		        {
		                dispNodeHash(L->next);
		                L = L->next;
		        }
		}
		st=st->prevst;
	}
}

int countSymbols(ST* st)
{
List L;
int ii;
int symCount=0;
	for(ii=0;ii<hsize;ii++)
	{ 
		L = st->HashTable[ii];
		while(L->next != NULL)
		{
		      symCount++; 
		      L = L->next;
		}
	}
	return symCount;
}

unsigned char  getStackPos(ST* st,char *name)
{
List L;
int ii;

		for(ii=0;ii<hsize;ii++)
		{ 
		        L = st->HashTable[ii];
		        while(L->next != NULL)
		        {
				if(strcmp(L->next->symbol.name, name) == 0)
					return L->next->symbol.stackPos;
		                L = L->next;
		        }
		}
}

void assignLocInStack(ST *st,int spaceInStack)
{
	
List L;
int ii;
unsigned char  stackPos;
	stackPos = spaceInStack;
	//stackPos = spaceInStack-4;
		for(ii=0;ii<hsize;ii++)
		{ 
		        L = st->HashTable[ii];
		        while(L->next != NULL)
		        {
		                L->next->symbol.stackPos = stackPos;
				stackPos -= 4; 
		                L = L->next;
		        }
		}
}

void dispNodeHash(LNode *p)
{
	if(p->symbol.type==STRUCT)
	{
		printf("The structure variables in %s are:\n",p->symbol.name);
		dispSymbolTableHash(p->symbol.structLink);
	}
	else
	{
		printf("%s\t%s\t%s\t%d\n",p->symbol.name,types1[p->symbol.type],types[p->symbol.subtype],p->symbol.width);
        	printf("\n");
	} 
}

//Has to be worked out for appropriate data type
void setSymbolValue(ST *st,char *name, int val)
{
	LNode *L;
	L = findSymbolHash(st,name);
	if(L==NULL)
	{
		printf("%s: Symbol Not found\n",name);
		exit(-1);
	}
	L->symbol.value.iVal=val;
	printf("val=%d\n",val);
}

int getSymbolValue(ST *st,char *name)
{
	LNode *L;
	printf("name=%s\n",name);
	L=findSymbolHash(st,name);
	if(L==NULL)
	{
		printf("%s: Symbol Not found\n",name);
		exit(-1);
	}
	return L->symbol.value.iVal;
}


LNode* findSymbolHash(ST* st,char *name)
{
List L;
int idx;

        idx = findHash(name);
        L = st->HashTable[idx];
        while(L->next != NULL)
        {
                if(strcmp(name,L->next->symbol.name) == 0)
                        return L->next;
                L = L->next;
 	}
        return NULL;
}


int findHash(char *name)
{
int hashVal = 0;
int ret;
        while(*name != '\0')
                hashVal += *name++;
        ret= hashVal % HASHSIZE;
	return ret;
}

int addSymbolHash(ST* st,Symbol symbol)
{
LNode *p,*sp;
int ret,idx;
char *str=NULL;
	if(symbol.type==CONST)
		return 1; 
	idx = findHash(symbol.name); 
        p = st->HashTable[idx];
	while(p->next != NULL)
        {
                ret = strcmp(symbol.name,p->next->symbol.name);
                if(ret == 0)
                {
                        printf("%s: %d: Error: %s is redefined\n",srcFileName,lineNo,symbol.name);
                        return -1;
                }
                p = p->next;
        }
        sp = malloc(sizeof(LNode));
        if(sp==NULL)
        {
                printf("Error in memory allocations\n");
                exit(-1);
        }
        sp->symbol.type = symbol.type;
        sp->symbol.qualifier = symbol.qualifier;
        sp->symbol.storage = symbol.storage;
        sp->symbol.sign = symbol.sign;
	sp->symbol.type = symbol.type;
        sp->symbol.subtype = symbol.subtype;
        sp->symbol.width = symbol.width;
	sp->symbol.widthType = symbol.widthType;
        sp->symbol.name = malloc(strlen(symbol.name)+1);
        sp->symbol.nameLen=symbol.nameLen;
        sp->symbol.st=symbol.st;
	switch(sym.subtype)
        {
        	case INT:
			sp->symbol.value.iVal = symbol.value.iVal;
                        break;
                case DOUBLE:
			sp->symbol.value.dVal = symbol.value.dVal;
                        break;
                case CHAR:
			sp->symbol.value.cVal = symbol.value.cVal;
                        break;
                case STRING:
			sp->symbol.value.sVal = symbol.value.sVal;
                        break;
        }

	sp->symbol.structLink=symbol.structLink;
        strcpy(sp->symbol.name,symbol.name);
        sp->next = NULL;
	//sp->symbol.structLink=NULL;
        p->next = sp;
        noElements = noElements + 1;
        return 1;
}

short int findDType(ST* st,Attr attr)
{
short int sType;
LNode *L;
	if(attr.type==CONST)
		return attr.subtype;
        L = findSymbolHash(st,attr.name);
        if(L == NULL)
        {
                return -1;
        }
        return L->symbol.subtype;
}

//Create a symbol record for the name and copy the information of attr.name to it
Symbol copySymbol(ST* st,Attr attr, char name[NAMESIZE])
{
        char *ptr;
	ST *cst;
        Symbol sym;
        LNode *L;
	short int found=FALSE;
	if(attr.type==CONST)
	{
		sym.nameLen = strlen(name);
	 	sym.name = malloc(strlen(name)+1);
		strcpy(sym.name,name);
		sym.storage = DEFAULT;
		sym.qualifier = DEFAULT;
		sym.sign=DEFAULT;
		sym.widthType=DEFAULT;
		sym.type = BASIC;  //In any case the resultant expression is an identifier
		sym.subtype = attr.subtype;
		switch(sym.subtype)
        	{
        		case INT:
				sym.value.iVal = attr.value.iVal;
                      		break;
                	case DOUBLE:
				sym.value.dVal = attr.value.dVal;
                        	break;
                	case CHAR:
				sym.value.cVal = attr.value.cVal;
                        	break;
                	case STRING:
				sym.value.sVal = attr.value.sVal;
                        	break;
        	}
		sym.width = sizes[attr.subtype];
		sym.relAddr=relAddr;
		relAddr = relAddr + sym.width;
	}
	else	        
	{
		cst = st;
		while(cst != NULL)
		{
			L = findSymbolHash(cst,attr.name);
			if(L != NULL)
			{
				found=TRUE;
				break;
			}
			else
			{
				cst=cst->prevst;
			}
		}
		if(found == FALSE)
		{
			printf("....%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,attr.name);
			errCount++;
			sym.type=ERROR;
			return sym;
		}
		else
		{
			sym.nameLen = strlen(name);
		 	sym.name = malloc(strlen(name)+1);
			strcpy(sym.name,name);
			sym.storage = L->symbol.storage;
			sym.qualifier = L->symbol.qualifier;
			sym.sign=L->symbol.sign;
			sym.type = BASIC;//L->symbol.type;  //In any case the resultant expression is an identifier
			sym.subtype = L->symbol.subtype;
			switch(sym.subtype)
        		{
        			case INT:
					sym.value.iVal = L->symbol.value.iVal;
                        		break;
                		case DOUBLE:
					sym.value.dVal = L->symbol.value.dVal;
                        		break;
                		case CHAR:
					sym.value.cVal = L->symbol.value.cVal;
					sym.value.cVal = attr.value.cVal;
                        		break;
                		case STRING:
					sym.value.sVal = L->symbol.value.sVal;
                        		break;
        		}
			sym.width = L->symbol.width;
			sym.st = L->symbol.st;
			sym.widthType = L->symbol.widthType;
			sym.relAddr=relAddr;
			relAddr = relAddr + sym.width;
		}
	}
        return sym;
}

Symbol createTempSymbolWithType(ST* st,Attr attr1,Attr attr2, Attr attr3)
{
        Symbol tSym;
        short stype1,stype2,dType1,dType2,dType3;
        
	int found = FALSE; 


	cst = st;
	while(cst != NULL)
	{
		dType1 = findDType(cst,attr1);
		if(dType1 != -1)
		{
			found=TRUE;
			break;
		}
		else
		{
			cst=cst->prevst;
		}
	}
	if(found == FALSE)
	{
		printf("....%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,attr1.name);
		errCount++; 
	}
	cst = st;

	while(cst != NULL)
	{
		dType2 = findDType(cst,attr2);
		if(dType2 != -1)
		{
			found=TRUE;
			break;
		}
		else
		{
			cst=cst->prevst;
		}
	}
	if(found == FALSE)
	{
		printf("....%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,attr2.name);
		errCount++;
	}
	
        dType3 = sizes[dType1] > sizes[dType2]? dType1:dType2;

        if(dType3 == dType1)
                tSym=copySymbol(st,attr1,attr3.name);
        else
                tSym=copySymbol(st,attr2,attr3.name);

	return tSym;
}
