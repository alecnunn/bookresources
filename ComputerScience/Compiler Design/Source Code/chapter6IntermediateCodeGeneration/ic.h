#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"define.h"
#include"op.h"
#define HASHSIZE 100
#define NAMESIZE 30
#define DIMSIZE 30
#define LABELSIZE 5

static char *ops[] = {"&","&=","&&","&&=","=","^","^=","/","/=","==",">=","goto",">",">>=","If=","<=","<","<<=","*","*=","-","-=","--","%","%=","!=","||","||=","+","+=","++","sizeof","*","~","~=","-","+","=[]","[]="};

long int relAddr;
char srcFileName[40];
int errCount;
int lineNo;




typedef struct Attr
{
	short int type;
	short int subtype;
        short int datatype;		//char, integer, long, float,double
	char name[NAMESIZE];
        union				//constant values if it is
        {   
                char cVal;
                int iVal;
                float fVal;
                long lVal;
                double dVal;
		char *string;
		long int relAddr;
	}value;
	int dimArray[DIMSIZE+1];
	short int noDimensions;
	char offsetName[NAMESIZE];
}Attr;

typedef struct Quad{
	char *label;
	int operator;
	Attr *operand1;
	Attr * operand2;
	Attr *result;
	struct Quad *nextQuad;
}Quad;

//Quad *quadTable=NULL;

typedef struct BBlock
{
	char label[LABELSIZE];
	struct Quad *start;
	struct Quad *end;
	int blockNumber;
	struct BBlock *trueNext;
	struct BBlock *falseNext;
	struct BBlock *next;

}BBlock;
//BBlock *bblockTable=NULL;
BBlock *rearBBPtr;


typedef struct Symbol
{
        short int type;  		//ID or Const 
        short int subtype;		//Basic, Array, Pointer, Referernce, File, Function
        short int datatype;		//char, integer, long, float,double
        short int nameLen;		//Lengh of the ID if it is
        char *name;			//ID lexeme
        short int storage;		//Storage type: auto, register, static, typedef 
        short int qualifier;		//Qualifier: Constant, Volatile
        short int sign;			//unsigned, signed
        short int width;		//width of the data if applicable
        union				//constant values if it is
        {   
                char cVal;
                int iVal;
                float fVal;
                long lVal;
                double dVal;
		char *string;
        }value;
	int *dimArray;			//dimArray[0] refers to how many dimenstions dimArray[1] .. dimArray[2] .. size of each dimension
	char offsetName[NAMESIZE];
        long int relAddr;			//Relative address of each symbol
}Symbol;


Symbol sym;



typedef struct Label
{
	char name[NAMESIZE];
	Quad *quad;
	BBlock *bblock;
	struct Label *nextLabel;
}Label;
//Label *labelTable=NULL;
Label *rearLabelPtr;



typedef struct LNode
{
        Symbol symbol;
        struct LNode *next;
} LNode;

typedef LNode *PtrToLNode ;
typedef PtrToLNode List;

typedef struct DagNode
{
	int label;
	union {
		int operator;
		char IdOrConst[NAMESIZE];
	} val;
	List list;
}DagNode;
typedef struct TNode
{
        Symbol symbol;
        struct TNode *left;
        struct TNode *right;
} TNode;

int hsize;

List *HashTable;

void initHash(int);

Symbol initSymbol();
Symbol copySymbol(Attr attr, char name[NAMESIZE]);

Symbol createTempSymbolWithType(Attr attr1,Attr attr2, Attr attr3);

short int findDType(Attr attr);

int  addSymbolHash(Symbol symbol);

int findHash(char[]);

void dispSymbolTableHash();

void destroySymbolTable(int size);

void dispNodeHash(LNode *p);

LNode *findSymbolash(char *name);

int assignValueHash(char *name,char *value);

int noElements;

TNode *thead;



LNode *head;

int addSymbolLL(LNode *,Symbol symbol);

int dispSymbolTableLL(LNode *);

void assignValueLL(LNode *, char *name,char *value);

/*-------------------semantic actions prototype declarations-------------*/

//typedef Label *LabelsList; 

void createQuadList(Quad **quadListHeader);

void createBBlockList(BBlock **blockListHeader);

void createLabelsListHeader(Label **labelListHeader);

void insertLabel(Label *labelTable,char *name,Quad *quad);//, int value,char type);

void addCode(Quad *quadListHeader,char *label,int operator,Attr operand1,Attr operand2,Attr result);

void addBBlock(BBlock *bblockListHeader,Quad *start,Quad *end,char label[LABELSIZE]);

void split2BB(Quad *quadTable,BBlock **bblockTable);

void printBB(BBlock *bblockTable);
void constructFlowGraph(BBlock *bblockTable);

//void printLabelsList(Label *labelListHeader);

void printCode(Quad *quadListHeader);

void itoa(int number, char *name);

void createTemp(char temp[]);

void createLabel(char temp[]);
