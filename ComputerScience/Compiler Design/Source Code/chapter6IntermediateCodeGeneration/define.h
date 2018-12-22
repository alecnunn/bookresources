#define CONST 1
#define INTCONST 2
#define REALCONST 3
#define CHARCONST 4
#define ID 5
#define TID 6
#define DEFAULT 0
static char *stores[] = {"auto","auto","register","static","extern","typedef"};
#define  AUTO 1
#define  REGISTER 2
#define  STATIC 3
#define  EXTERN 4
#define  TYPEDEF 5
static char *types[] = {"int","char","short","int","long","float","double","void"};
static char *types1[] = {"basic","basic","array"};
static int sizes[] = {sizeof(int),sizeof(char),sizeof(short),sizeof(int),sizeof(long)+1,sizeof(float)+2,sizeof(double),-1};
//static int sizes[] = {sizeof(int),sizeof(char),sizeof(short),sizeof(int),sizeof(long),sizeof(float),sizeof(double),-1};
#define  CHAR 1
#define  SHORT 2
#define  INT 3
#define  LONG 4
#define  FLOAT 5
#define  DOUBLE 6
#define  VOID 7 
#define BASIC 1
#define ARRAY 2 
#define OFFSET 3 
#define  STRING 9
#define  ADDR 10
static char *signs[] = {"signed","signed","unsigned"};
#define  SIGNED 1
#define  UNSIGNED 2
static char *qualifiers[] = {"","const","volatile"};
#define  CONST 1
#define  VOLATILE 2
#define  FALSE 0 
#define  TRUE 1 
#define  FLAG 4 
#define  LABEL 3
