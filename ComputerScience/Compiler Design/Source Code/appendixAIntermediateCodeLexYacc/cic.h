#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"def.h"
#include"op.h"
#define STACKSIZE 100
#define MAXLABEL 10
#define HASHSIZE 100
#define NAMESIZE 30
#define MAXCASES 20 
#define DIMSIZE 30
#define LABELSIZE 30
#define ERROR -1
#define testc {printf("Test and continue\n");}
#define teste {printf("Test and exit\n");exit(-1);}
long int relAddr;
char srcFileName[40];
int errCount;
int lineNo;
int cstackTop;
int bstackTop;
char cstack[STACKSIZE][MAXLABEL];
char bstack[STACKSIZE][MAXLABEL];
char caseLabels[MAXCASES][MAXLABEL];
char caseExprs[MAXCASES][NAMESIZE];
typedef struct Attr
{
        short int type;
        short int subtype;
	char label1[LABELSIZE];
	char label2[LABELSIZE];
	//short int datatype;             //char, integer, long, float,double
        char name[NAMESIZE];
        union                           //constant values if it is
        {
                char cVal;
                int iVal;
                float fVal;
		double dVal;
		char *sVal;
                int relAddress;
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

typedef struct Symbol
{
        short int type;                 //ID or Const
        short int subtype;              //Basic, Array, Pointer, Referernce, File, Function
        //short int datatype;             //char, integer, long, float,double
        short int nameLen;              //Lengh of the ID if it is
        char *name;                     //ID lexeme
        short int storage;              //Storage type: auto, register, static, typedef
        short int qualifier;            //Qualifier: Constant, Volatile
        short int sign;                 //unsigned, signed
        short int width;                //width of the data if applicable
        union                           //constant values if it is
        {
                char cVal;
                int iVal;
                float fVal;
                char *string;
        }value;
        int *dimArray;                  //dimArray[0] refers to how many dimenstions dimArray[1] .. dimArray[2] .. size of each dimension
        int relAddr;                    //Relative address of each symbol
}Symbol;


Symbol sym;


typedef struct LNode
{
        Symbol symbol;
        struct LNode *next;
} LNode;

typedef LNode *PtrToLNode ;
typedef PtrToLNode List;
int hsize;

typedef struct ST
{
	struct ST *prevst;
	List *HashTable;
}ST;
ST *prevst,*st,*cst;
void initHash(ST*,int);

Symbol copySymbol(ST *st,Attr attr, char name[NAMESIZE]);

Symbol createTempSymbolWithType(ST *st,Attr attr1,Attr attr2, Attr attr3);

short int findDType(ST *st,Attr attr);

int  addSymbolHash(ST *st,Symbol symbol);

int findHash(char[]);

void dispSymbolTableHash(ST *st);

void destroySymbolTable(ST *st,int size);

void dispNodeHash(LNode *p);

LNode *findSymbolHash(ST *st,char *name);

int noElements;

LNode *head;

int addSymbolLL(LNode *,Symbol symbol);

int dispSymbolTableLL(LNode *);

/*-------------------semantic actions prototype declarations-------------*/
short int cstackpush(char *label);
short int cstackpop();
short int bstackpush(char *label);
short int bstackpop();
void addCode(Quad *quadListHeader,char *label,int operator,Attr operand1,Attr operand2,Attr result);


void printCode(Quad *quadListHeader);

void itoa(int number, char *name);

void createTemp(char temp[]);
