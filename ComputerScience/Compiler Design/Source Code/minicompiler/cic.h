#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"def.h"
#include"op.h"
#define STACKSIZE 100
#define MAXLABEL 10
#define HASHSIZE 1 //100
#define NAMESIZE 30
#define MAXCASES 20 
#define DIMSIZE 30
#define LABELSIZE 30
#define ERROR -1
#define testc {printf("Test and continue\n");}
#define teste {printf("Test and exit\n");exit(-1);}
#define MAXCODE 10000
int cc;
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
unsigned char mainCode[MAXCODE];
unsigned int mainCodeSize;
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
Quad *quadTable;
typedef struct Symbol
{
        short int type;                 //ID or Const, Basic, Array, Pointer, Referernce, File, Function
        short int subtype;              //char, integer, long, float,double
        short int nameLen;              //Lengh of the ID if it is
        char *name;                     //ID lexeme
        short int storage;              //Storage type: auto, register, static, typedef
        short int qualifier;            //Qualifier: Constant, Volatile
        short int sign;                 //unsigned, signed
        short int width;                //width of the data if applicable
	short int widthType;		//width type (short or long - applicable to integer
	unsigned char stackPos;
        union                           //constant values if it is
        {
                char cVal;
                int iVal;
                double dVal;
                char *sVal;
	}value;
	void *dimArray;  //dimArray[0] refers to how many dimenstions dimArray[1] .. dimArray[2] .. size of each dimension
	struct ST *structLink; 	//to hold the fields of the structure
	struct ST *st;
        int relAddr;                    //Relative address of each symbol
}Symbol;


Symbol sym,sym1;


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
	char funcName[NAMESIZE];
	struct ST *prevst;
	List *HashTable;
}ST;
ST *prevst,*st,*cst,*fst,*gst;
void initHash(ST*,int);

Symbol copySymbol(ST *st,Attr attr, char name[NAMESIZE]);

Symbol createTempSymbolWithType(ST *st,Attr attr1,Attr attr2, Attr attr3);

short int findDType(ST *st,Attr attr);

int  addSymbolHash(ST *st,Symbol symbol);

int findHash(char[]);

void setSymbolValue(ST *st,char *name,int val);
int getSymbolValue(ST *st,char *name);

void dispSymbolTableHash(ST *st);

void destroySymbolTable(ST *st,int size);

void dispNodeHash(LNode *p);

LNode *findSymbolHash(ST *st,char *name);

int noElements;

LNode *head;

int addSymbolLL(LNode *,Symbol symbol);

int dispSymbolTableLL(LNode *);

void assignLocInStack(ST *st,int spaceInStack);
int countSymbols(ST* st);
unsigned char  getStackPos(ST* st,char *name);


/*-------------------semantic actions prototype declarations-------------*/
short int cstackpush(char *label);
short int cstackpop();
short int bstackpush(char *label);
short int bstackpop();
void addCode(Quad *quadListHeader,char *label,int operator,Attr operand1,Attr operand2,Attr result);


void printCode(Quad *quadListHeader);
void genCodeFromQuad(Quad *quadListHeader);

void itoa(int number, char *name);

void createTemp(char temp[]);
