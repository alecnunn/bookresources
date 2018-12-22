/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ hashtbl.cpp - the hash table                                      +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	The hash table is the gateway to the entire process of creating
	a symbol table and resolving indentifiers during assembly

	During Pass1 ( directives populate Proc and GlobVar arrays )
	------------
	
	1) encounter a identifier declaration
	2) take hash and query hash table
		2a) if exists ( query proc returns non-NULL ) 
			print error, redefinition
		2b) if does not exist ( query proc returns NULL )
			add entry to string table
			add entry to symbol table
			add entry to hash table

	During Pass2 ( gen. bytecode )
	------------
	
	1) encounter a identifier in an instruction
	2) take hash and query hash table
		2a) if exists ( query proc returns non-NULL ) 
			generate bytecode
		2b) if does not exist ( query proc returns NULL )
			print error, referencing non-existent identifier
*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*hash table type*/

#define GLOBAL_VAR	0	
#define PROC		1
#define PROC_RET	2
#define PROC_ARG	3
#define PROC_LOC	4
#define PROC_LBL	5

char *SymTypeStr[]={"GLOBAL_VAR",
                    "PROC",
                    "PROC_RET","PROC_ARG","PROC_LOC","PROC_LBL"};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct HashTbl
{
	U1 empty;		/*indicates if entry is used or not*/
    U8 text;		/*index to strTbl*/
	U1 type;		/*GLOBAL_VAR -> PROC_LBL*/
	U4 index;		/*index to globVar, proc arrays in symbTbl*/
	U4 subIndex;	/*subindex for PROC_RET, ... , PROC_LBL*/
    struct HashTbl *left;
    struct HashTbl *right;
};

#define PRIME 547
//#define PRIME 5

class HashTable
{
	StringTable *strTbl;

	int hashpjw(unsigned char *s);

	/*collision resolution BST routines*/
	struct HashTbl* findNode(struct HashTbl* link, char *val);
	void insertNode(struct HashTbl** link, char *val, U8 text, U1 type, U4 ind, U4 subInd, U4 line);
	void printTree(struct HashTbl* link, int level);

	void deleteAllNodes(struct HashTbl** link);

	public:
	struct HashTbl hashTbl[PRIME];

	HashTable(StringTable *st);
	~HashTable();

	/*hash table routine calls corresponding BST rotuine*/
	struct HashTbl* queryHashTbl(char *str);
	void addHashTblEntry(char *val, U8 text, U1 type, U4 ind, U4 subInd, U4 line );
	void printHashTbl();

	void test();
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                        +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

HashTable::HashTable(StringTable *st)
{
	int i;
	for(i=0;i<PRIME;i++)
	{ 
		hashTbl[i].empty = TRUE;
		hashTbl[i].text = 0;	
		hashTbl[i].left = NULL;
		hashTbl[i].right = NULL;
	}
	strTbl = st;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

HashTable::~HashTable()
{
	int i;
	for(i=0;i<PRIME;i++)
	{ 	
		deleteAllNodes(&(hashTbl[i].left));
		deleteAllNodes(&(hashTbl[i].right));
	}
	return;

}/*end destructor*/

/*-----------------------------------------------------------------*/

/*
if symbol exists in hash table, we get a pointer to the node
if a symbol does not exist in the hash table, we get NULL
*/

struct HashTbl* HashTable::queryHashTbl(char *str)
{
	int hash;

	hash = hashpjw((unsigned char*)str);
	if(hashTbl[hash].empty==TRUE){ return(NULL); }
	return(findNode(&(hashTbl[hash]), str));

}/*end queryHashTbl*/

/*-----------------------------------------------------------------*/

void HashTable::addHashTblEntry(char *val, 
								U8 text, 
								U1 type, 
								U4 ind, 
								U4 subInd,
								U4 line)
{
	struct HashTbl *ptr;
	int hash;

	hash = hashpjw((unsigned char*)val);
	
	if(hashTbl[hash].empty==TRUE)
	{ 
		hashTbl[hash].empty=FALSE;
		hashTbl[hash].text=text; 
		hashTbl[hash].type=type;
		hashTbl[hash].index=ind;
		hashTbl[hash].subIndex=subInd;
		hashTbl[hash].left = NULL;
		hashTbl[hash].right = NULL;
		return; 
	}

	ptr = &hashTbl[hash];
	insertNode(&ptr, val, text, type, ind, subInd, line);
	return;

}/*end addHashTblEntry*/

/*-----------------------------------------------------------------*/

void HashTable::printHashTbl()
{
	int i;
	printf("\nHASH TABLE--------------\n");
	for(i=0;i<PRIME;i++)
	{ 
		if(hashTbl[i].empty == FALSE)
		{
			printf("Hash Slot %d)------\n",i);
			printTree(&(hashTbl[i]), 0);
			printf("\n");
		}
	}
	printf("\n");
	return;

}/*end printHashTbl*/

/*-----------------------------------------------------------------*/

int HashTable::hashpjw(unsigned char *s)
{
	unsigned char * p;
	unsigned h = 0, g;

	for (p = s; *p != '\0'; p = p + 1)
	{
		h = (h << 4) + (*p);
		if (g = (h & 0xf0000000))
		{
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	return h % PRIME;

}/*end hashpjw*/

/*-----------------------------------------------------------------*/

struct HashTbl* HashTable::findNode(struct HashTbl* link, char *val)
{
    if(link==NULL)
    { 
        return(NULL); 
    }
    else if(strcmp(val,(char*)&((*strTbl).text[(*link).text]))==0)
    { 
        return(link); 
    }
    else if(strcmp(val,(char*)&((*strTbl).text[(*link).text]))>0 )
    { 
        return(findNode((*link).right,val)); 
    }
    else
    {
        return(findNode((*link).left,val));
    }

}/*end findNode*/

/*-----------------------------------------------------------------*/

void HashTable::insertNode(struct HashTbl** link, 
						   char *val, 
						   U8 text, 
						   U1 type, 
						   U4 ind, 
						   U4 subInd,
						   U4 line)
{
    if( (*link) == NULL )
    {
        (*link) = (struct HashTbl*)malloc(sizeof(struct HashTbl));
		(*(*link)).empty	= FALSE;
		(*(*link)).text		= text;
		(*(*link)).type     = type;
		(*(*link)).index    = ind;
		(*(*link)).subIndex = subInd;
        (*(*link)).left		= NULL;
        (*(*link)).right	= NULL;
    }
    else if( strcmp(val,(char*)&((*strTbl).text[(*(*link)).text])) == 0 )
    {
		ERROR2("re-defined identifier %s on line %lu\n",val,line);
        return;
    }
	else if( strcmp(val,(char*)&((*strTbl).text[(*(*link)).text])) < 0 )
    {
        insertNode( &((*(*link)).left) , val, text, type, ind, subInd, line);
    }
    else
    {
        insertNode( &((*(*link)).right) ,val, text, type, ind, subInd, line);
    }
    return;

}/*end insertNode*/

/*-----------------------------------------------------------------*/

void HashTable::deleteAllNodes(struct HashTbl** link)
{
	if(*link==NULL){ return; }
	
	deleteAllNodes(&(*(*link)).left);
	deleteAllNodes(&(*(*link)).right);

	//printf("freeing node %s\n",&((*strTbl).text[(*(*link)).text]));
	free(*link);
	*link=NULL;

	return;

}/*end deleteAllNodes*/

/*-----------------------------------------------------------------*/

/*
print tree by giving root to node and zero as args
*/

void HashTable::printTree(struct HashTbl* link, int level)
{
    int i;
    if(link==NULL)
    {
        return;
    }
    printTree((*link).right,level+1);
    for(i=0;i<level;i++){ printf("-"); }

    printf("id =%s\t",&((*strTbl).text[(*link).text]));
	printf("type=%s\t",SymTypeStr[(*link).type]);
	printf("(i,si)=(%d,%d)\n",(*link).index,(*link).subIndex);

    printTree((*link).left,level+1);
    return;

}/*end printTree*/

/*-----------------------------------------------------------------*/

void HashTable::test()
{
	U8 i;
	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("harold"); 
	addHashTblEntry("harold",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("andrew"); 
	addHashTblEntry("andrew",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("jake"); 
	addHashTblEntry("jake",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("bart"); 
	addHashTblEntry("bart",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("xavier"); 
	addHashTblEntry("xavier",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("randall"); 
	addHashTblEntry("randall",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("earl"); 
	addHashTblEntry("earl",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("greg"); 
	addHashTblEntry("greg",i,0,0,0,0);

	i = (*strTbl).iStr;
	(*strTbl).addStrTbl("damien"); 
	addHashTblEntry("damien",i,0,0,0,0);


	char name[20];
	
	strcpy(name,"randall");
	if((queryHashTbl(name))!=NULL){ printf("found %s\n",name); }
	else{ printf("NOT find %s\n",name); }

	strcpy(name,"herb");
	if((queryHashTbl(name))!=NULL){ printf("found %s\n",name); }
	else{ printf("NOT find %s\n",name); }
	
	strcpy(name,"earl");
	if((queryHashTbl(name))!=NULL){ printf("found %s\n",name); }
	else{ printf("NOT find %s\n",name); }
	
	strcpy(name,"pearl");
	if((queryHashTbl(name))!=NULL){ printf("found %s\n",name); }
	else{ printf("NOT find %s\n",name); }

	printHashTbl();
	return;

}/*end test*/













