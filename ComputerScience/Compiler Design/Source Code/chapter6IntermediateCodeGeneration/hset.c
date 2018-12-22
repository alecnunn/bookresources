#include "ic.h"
void initHash(int size)
{
int ii;
	hsize = size;
	HashTable = (List*)malloc(sizeof(List) * hsize);
	if(HashTable == NULL) 
	{
		printf("Memory allocation problem\n");
		exit(-1);
	}
	for(ii=0;ii<hsize;ii++)
	{
		HashTable[ii]=(LNode*)malloc(sizeof(LNode));
		if(HashTable[ii] == NULL) 
		{ 
			printf("error in memory allocation for hash table\n");
			exit(-1);
		}
		HashTable[ii]->next = NULL;
		HashTable[ii]->symbol.name=NULL;
		HashTable[ii]->symbol.nameLen = 0;
		HashTable[ii]->symbol.storage = DEFAULT;
		HashTable[ii]->symbol.qualifier = DEFAULT;
		HashTable[ii]->symbol.sign = DEFAULT;
		HashTable[ii]->symbol.type = DEFAULT;
		HashTable[ii]->symbol.width = 0;
	}
}

void destroySymbolTable(int size)
{
int ii;
List L,tmp;
	for(ii = 0;ii<size;ii++)
	{
		L = HashTable[ii];
		while (L != NULL)
		{
			tmp = L;
			L = L->next;
			free(L);
		}
	}
}
		
void dispSymbolTableHash()
{
List L;
int ii;
	printf("Name\tQualfier\tStorage\tSign\tCategory\tType\tWidth\tMem relAddr\n");
	for(ii=0;ii<hsize;ii++)
	{
		L = HashTable[ii];
		while(L->next != NULL)
		{
			dispNodeHash(L->next);
			L = L->next;
		}
	}
}
	
void dispNodeHash(LNode *p)
{
	 printf("%s\t%s\t\t%s\t%s\t%s\t\t%s\t%d\t%ld\n",p->symbol.name,qualifiers[p->symbol.qualifier],stores[p->symbol.storage],signs[p->symbol.sign],types1[p->symbol.subtype],types[p->symbol.datatype],p->symbol.width,p->symbol.relAddr);

	/*switch(p->symbol.type)
	{
		case CHAR:
			printf("%c ",p->symbol.value.cVal);
			break;
		case INT:
			printf("%d ",p->symbol.value.iVal);
			break;
		case FLOAT:
			printf("%f ",p->symbol.value.fVal);
			break;
		case LONG:
			printf("%ld ",p->symbol.value.lVal);
			break;
		case DOUBLE:
			printf("%lf ",p->symbol.value.dVal);
			break;
		case STRING:
			printf("%s",p->symbol.oVal);
			break;
	}*/
	printf("\n");
}

LNode* findSymbolHash(char *name)
{
List L;
int idx;
	idx = findHash(name);
	L = HashTable[idx];
	while(L->next != NULL)
	{
		if(strcmp(name,L->next->symbol.name) == 0)
			return L->next;
		L = L->next;
	}
	return NULL;
}

int assignValueHash(char *name,char value[])
{
	LNode *p;
	char *str;
	p=findSymbolHash(name);
	if(p!=NULL)
	{
		switch(p->symbol.type)
		{
			case CHAR:
				p->symbol.value.cVal = *value;
				break;
			case INT:
				p->symbol.value.iVal = atoi(value);
				break;
			case FLOAT:
				p->symbol.value.fVal = atof(value);
				break;
			case LONG:
				p->symbol.value.lVal = atol(value);
				break;
			case DOUBLE:
				p->symbol.value.dVal = atof(value);
				break;
			case STRING:
				str = (char*)malloc(strlen(value));
				strcpy(str,value);
				p->symbol.value.string=str;
				break;
		}
	}
	else 
	{
		printf("%s : Not found\n",name);
		return -1;
	}
	return 1;
}

int findHash(char *name)
{
int hashVal = 0;
	while(*name != '\0')
		hashVal += *name++;
	return hashVal % hsize;
}

int addSymbolHash(Symbol symbol)
{
LNode *p,*sp;
int ret,idx;
char *str=NULL;
	idx = findHash(symbol.name);
	p = HashTable[idx];
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
        sp->symbol.datatype = symbol.datatype;
        sp->symbol.width = symbol.width;
	sp->symbol.name = malloc(strlen(symbol.name)+1);
	sp->symbol.nameLen=symbol.nameLen;
        strcpy(sp->symbol.name,symbol.name);
        sp->symbol.relAddr = symbol.relAddr;
        sp->symbol.dimArray = symbol.dimArray;
	sp->next = NULL;
	p->next = sp;
        noElements = noElements + 1;
	return 1;
}

short int findDType(Attr attr)
{
short int sType;
LNode *L;
	if(attr.type==CONST)
	{	if(attr.datatype == INT)
			return INT;
		else if(attr.datatype == DOUBLE)
			return DOUBLE;
		else if(attr.datatype == CHAR)
			return CHAR;
	}
	L = findSymbolHash(attr.name);
	//printf("The retrieved info from ST-->name = %s type = %d\n",L->symbol.name,L->symbol.type);
	if(L == NULL)
	{
		printf("%s: not available in the Symbol Table\n",attr.name);
		exit(-1);
	}
	return L->symbol.datatype;
} 


Symbol copySymbol(Attr attr, char name[NAMESIZE])
{
	char *ptr;
	Symbol sym;
	LNode *L;
	L = findSymbolHash(attr.name);
	sym.nameLen = strlen(name);
	sym.name = malloc(strlen(name)+1);
       	strcpy(sym.name,name);
        sym.storage = L->symbol.storage;
        sym.qualifier = L->symbol.qualifier; 
        sym.sign=L->symbol.sign;
        sym.type = L->symbol.type;
	if(attr.type == ARRAY)
		strcpy(sym.offsetName,attr.offsetName);
        sym.subtype = L->symbol.subtype;
        sym.datatype = L->symbol.datatype;
        sym.width = L->symbol.width;
        sym.relAddr=relAddr;
        relAddr = relAddr + sym.width;
	return sym;
}
Symbol initSymbol()
{
	Symbol sym;
	sym.nameLen = 0;
	sym.name = NULL;
        sym.storage = DEFAULT;
        sym.qualifier = DEFAULT;
        sym.sign=DEFAULT;
        sym.type =DEFAULT;
        sym.subtype = DEFAULT;
        sym.width = 0;
        sym.relAddr=0;
        strcpy(sym.offsetName,"");
	return sym;
}

void copyAttr2Symbol(Attr attr, Symbol *sym)
{
	sym->nameLen = strlen(attr.name);
        sym->name=malloc(strlen(attr.name)+1);
	strcpy(sym->name,attr.name);
        sym->storage = DEFAULT;
        sym->qualifier = DEFAULT;
        sym->sign=DEFAULT;
        sym->type =attr.type;
        sym->subtype = attr.subtype;
        sym->datatype = attr.datatype;
	switch(attr.datatype)
	{
		case CHAR: 
				sym->width = sizeof(char);
				break;	
		case INT: 
				sym->width = sizeof(int);
				break;	
		case LONG: 
				sym->width = sizeof(long);
				break;	
		case FLOAT: 
				sym->width = sizeof(float);
				break;	
		case DOUBLE: 
				sym->width = sizeof(double);
				break;	
		case STRING: 
				sym->width = strlen(attr.value.string);
				break;	
	}
        sym->relAddr=relAddr;
	relAddr += sym->width;
        strcpy(sym->offsetName,attr.offsetName);
}
Symbol createTempSymbolWithType(Attr attr1,Attr attr2, Attr attr3)
{
	Symbol tSym;	
	short stype1,stype2,dType1,dType2,dType3;
	dType1 = findDType(attr1);
	dType2 = findDType(attr2);
	dType3 = sizes[dType1] > sizes[dType2]? dType1:dType2;
	if(dType3 == dType1)
		tSym=copySymbol(attr1,attr3.name);
	else
		tSym=copySymbol(attr2,attr3.name);
	return tSym;
}	
