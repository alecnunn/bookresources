#define FALSE 1
#define TRUE 2
#define ID 5
#define CONST 6
#define LABEL 7
#define DEFAULT 0
static char *types[] = {"int","void","char","int","float","double","short","long","signed","unsigned","bool","complex","structorunion","string","unknown"};
static char *types1[] = {"basic","basic","array","struct","union"};
static int sizes[] = {sizeof(int),0,sizeof(char),sizeof(int),sizeof(float),sizeof(double),sizeof(short),sizeof(long),sizeof(signed),sizeof(unsigned),1,2*sizeof(int),sizeof(int),sizeof(int),-1};
//Data types
#define VOID 1
#define  CHAR 2
#define  INT 3
#define  FLOAT 4
#define  DOUBLE 5
#define SHORT 6 
#define LONG 7
#define SIGNED 8
#define UNSIGNED 9
#define BOOL 10
#define COMPLEX 11
#define STRUCTORUNION 12
#define  STRING 13
#define  UNKNOWN 14
//Storage Type
#define AUTO 1
#define REGISTER 2
#define STATIC 3
#define EXTERN 4
#define TYPEDEF 5
#define ENUM 6
//Qualifier type
#define RESTRICT 2
#define VOLATILE 3
//Basic data or derived datatypes
#define BASIC 1
#define ARRAY 2
#define STRUCT 3
#define UNION 4

