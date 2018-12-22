%{
#include "ic.h"
Quad *quadTable=NULL;
BBlock *bblockTable=NULL;
Label *labelTable=NULL;
char str[10];
int i;
char *ptr;
int numDim;
char labelpending[10];
char tempvar[10];
char tempLabel[10];
char switchExpResult[10];
char switchFlagName[10];
short int dataType;
short int signType;
short int storeType;
short int qualifierType;
short int dataSize;
int dimArray[DIMSIZE+1];
int totArrayElements;
extern int lineNo;
extern LNode* findSymbolHash(char*);
LNode* L;
Attr opnd1,opnd2,result;
Attr noAttr, tempAttr,constAttr,constAttr1,constAttr2,constAttr3;
Attr attr1,tempAttr1,tempAttr2,tempAttr3;
short int noDimensions;
typedef struct TFLABEL
{	
	char trueLabel[10];
  	char falseLabel[10];
}TFLABEL;
TFLABEL tflabel;
%}
%union
{
  char name[NAMESIZE];
  int no;
  Attr attr;
  
  /*Label label;*/
  struct tfLabel
  {	
	char trueLabel[10];
  	char falseLabel[10];
  }tflabel;
 struct ForLabel
 {
	char cond2Label[10];
	char cond3Label[10];
	char cond2TestLabel[10];
	char nextStmtLabel[10];
 }forLabel;
};

%left _or
%left _and
%left _eq _ne
%left _lt _le _gt _ge
%left _minus _plus
%left _mul _div _modulo
%nonassoc _uminus 
%token _char _int _float  _double  _const  _signed  _unsigned  _short _long
%token _goto _else _switch _break _continue _case _struct
%token _union _typedef _main _assign _default
%token _leftb _rightb _leftp _rightp _leftsp _rightsp _comma _semicolon _colon
%token _void _auto _register _static _extern _volatile 
%token _eofile
%token <attr> _id
%token <attr> _charcons
%token  <attr> _num
%token  <attr> _dnum
%type <attr> cons
%type <attr> LValue
%type <attr> assignExpr
%type <attr> Expr
%type <attr> arrayDims
%type <attr> condition
%token <tflabel> _if
%token <tflabel> _while
%token <tflabel> _do
%token <forLabel> _for
%type <no> dType
%type <no> sType
%type <no> qType
%type <no> sign
%%
Pgm     : _main{
			initHash(HASHSIZE);
			createQuadList(&quadTable);
			createBBlockList(&bblockTable);
			createLabelsListHeader(&labelTable);
			strcpy(labelpending," ");
		} 
	  _leftp _rightp _leftb Stmts _rightb _eofile 
		{
			if(errCount == 0)
			{
				printCode(quadTable);
				split2BB(quadTable,&bblockTable);
				printBB(bblockTable);
				constructFlowGraph(bblockTable);
				printFlowGraph(bblockTable);
				printLabelsList(labelTable);
				printf("\n");
				//dispSymbolTableHash();
			}
			destroySymbolTable(HASHSIZE);
			return;
		}

Stmts   : Stmts Stmt | Stmt	
	;

Stmt    : dStmt|eStmt
	;

eStmt	: assignStmt 
	| ifStmt
	| whileStmt 
	| doStmt 
	| forStmt 
	| breakStmt 
	| continueStmt
	| switchStmt
	| gotoStmt
	;

dStmt   : sType qType sign dType varList _semicolon
	;

varList : varList _comma id	
	 |id
	;
id:	_id 			{
					sym.nameLen = strlen($1.name);
                                        sym.name = malloc(sym.nameLen+1);
					strcpy(sym.name,$1.name);
					sym.storage = storeType;
					sym.qualifier = qualifierType;
					sym.sign=signType;
				 	sym.subtype = BASIC; 
				 	sym.datatype = dataType; 
					sym.type = ID;
					sym.width = dataSize; 
					sym.relAddr=relAddr;
					relAddr = relAddr + sym.width;
					addSymbolHash(sym);
				}
	| _id dimList
				{
					sym.nameLen = strlen($1.name);
                                        sym.name = malloc(sym.nameLen+1);
					strcpy(sym.name,$1.name);
					sym.storage = storeType;
					sym.qualifier = qualifierType;
					sym.sign=signType;
					sym.type = ID;
				 	sym.subtype = ARRAY; 
				 	sym.datatype = dataType; 
					sym.width = dataSize; 
					totArrayElements = 1;
					if(sym.type == ARRAY)
						sym.dimArray = malloc(sizeof(int) * (dimArray[0]+1));
					totArrayElements = 1;
					for(i = 1;i <= dimArray[0];i++)
						totArrayElements *= dimArray[i];
					sym.relAddr = relAddr;
					sym.dimArray = malloc((dimArray[0]+1)*sizeof(int));
					memcpy(sym.dimArray,dimArray,(dimArray[0]+1) * sizeof(int));
					relAddr=relAddr+totArrayElements * sym.width;
					addSymbolHash(sym);
					numDim = 0;
				}
	;
dimList	: dimList _leftsp _num _rightsp 	{ 
					dimArray[0]=++numDim;
					dimArray[numDim] = $3.value.iVal;// atoi($3.name);
				}
	 |_leftsp _num _rightsp	{
				  	dimArray[0]=++numDim;
					dimArray[numDim] = $2.value.iVal;//atoi($2.name);
				}
	; 
dType   : 
	 _char			{
					dataType = CHAR;
					dataSize=sizeof(char);
				} 
	| _short		{
					dataType = SHORT;
					dataSize=sizeof(short int);
				}
	| _int 			{
					dataType  = INT;
					dataSize=sizeof(int);
				}
	| _long			{
					dataType  = LONG;
					dataSize=sizeof(long int);
				}
	| _float 		{
					dataType  = FLOAT;
					dataSize=sizeof(float);
				}
	| _double		{
					dataType  = DOUBLE;
					dataSize = sizeof(double);
				}
	;
sign: 				{
					signType = DEFAULT;
				}
	| _signed 		{
					signType = SIGNED;
				}
	| _unsigned 		{
					signType = UNSIGNED;
				}

sType	:			{
					storeType = DEFAULT;
				}
	| _auto 		{
					storeType = AUTO;
				}
	|_register		{
					storeType = REGISTER;
				}
	|_static		{
					storeType = STATIC;
				}
	|_extern		{
					storeType = EXTERN;
				}
	|_typedef		{
					storeType = TYPEDEF;
				}
	;
qType	:			{
					qualifierType = DEFAULT;
				}	
	|_const			{
					qualifierType = CONST;
				}
	| _volatile		{
					qualifierType = VOLATILE;
				}
	;
assignExpr: LValue  _assign  Expr {
					$$ = $1;
					opnd2.type = NOP;
					if($1.subtype==BASIC)
					{
						addCode(quadTable,labelpending,ASSIGN,$3,opnd2,$1); 
					}
					else if($1.subtype == ARRAY || $1.subtype == OFFSET) //we have clarify how to differentiate bw ARRAY and OFFSET
					{
						opnd2 = $1;
						strcpy(opnd2.name,$1.offsetName);
						opnd2.type = TID;
						//opnd2.subtype = BASIC;
						result = $1;
						strcpy(result.name,$1.name);
						result.type = TID;
						result.subtype = BASIC;
						addCode(quadTable,labelpending,LASSIGN,$3,opnd2,result); 
						//format:  []= 'rightside value' 'offset' 'baseaddr'
	 				}
					
			      	}
	| LValue _assign assignExpr { 
					$$=$1;
					opnd2.type = NOP;
					if(strcmp($1.offsetName,"")==0)
					{
						addCode(quadTable,labelpending,ASSIGN,$3,opnd2,$1); 
					}
					else
					{
						addCode(quadTable,labelpending,LASSIGN,$1,$3,$1); 
	 				}
			      	}

	;

assignStmt: assignExpr _semicolon
	;

LValue	: _id { 
			if(findSymbolHash($1.name) == NULL)
			{
				printf("%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,$1.name);
				errCount++;
			}
			$$=$1;
			$$.subtype = BASIC;
			strcpy($$.offsetName,"");
		}
	| arrayDims {
			L = findSymbolHash($1.name);  //$1.name holds the lexeme of _id
			if(L == NULL)
			{
				printf("%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,$1.name);
				errCount++;
			}

	   		$$.type = TID;
			$$.subtype = ARRAY;
			$$.datatype = $1.datatype;

			opnd2.type = CONST;			//opnd2 holds the multiplier
			opnd2.subtype = BASIC;
			opnd2.datatype = INT;
			opnd2.value.iVal = L->symbol.width;

			createTemp(result.name);		//result holds the displacement expr
			result.type = TID;
			result.subtype = BASIC;
			result.datatype = INT;
			strcpy($$.offsetName,result.name);	

	 		addCode(quadTable,labelpending,MUL,$1,opnd2,result);
			copyAttr2Symbol(result,&sym);
	   		addSymbolHash(sym);
	
			opnd1.type = CONST;			//opnd1 holds the base address from symbol table
			opnd1.subtype = BASIC; 		
			opnd1.datatype = LONG; 		
			opnd1.value.lVal = L->symbol.relAddr;

			opnd2.type=NOP;
	
			createTemp(result.name);		//base address from opnd1 is stored in temp id (result)
			result.type = TID;
			result.subtype = BASIC;
			result.datatype = INT;
	 		addCode(quadTable,labelpending,ASSIGN,opnd1,opnd2,result);
			
			sym = initSymbol();
			copyAttr2Symbol(result,&sym);
	   		addSymbolHash(sym);

			strcpy($$.name,result.name);	
	    	}
	
	;	
arrayDims	: arrayDims _leftsp Expr _rightsp 	
		{ 
			noDimensions += 1;
			createTemp($$.offsetName);
			strcpy($$.name,$1.name);
			$$.type = $1.type;
			//$$.subtype = ARRAY;
			$$.subtype = OFFSET;
			$$.datatype = $1.datatype;
			L = findSymbolHash($1.name);
			if(L == NULL)
			{
				printf("%s: %d:Error %s: Undeclared Identifier\n",srcFileName,lineNo-1,$1.name);
				errCount++;
			}
			createTemp(opnd2.name);
			opnd2.type = CONST;
			opnd2.datatype = INT;
			opnd2.value.iVal=L->symbol.dimArray[noDimensions];
	 		addCode(quadTable,labelpending,MUL,$1,opnd2,$$);
	 		addCode(quadTable,labelpending,PLUS,$$,$3,$$);
		}
	 |_id _leftsp Expr _rightsp
		{
			strcpy($$.name,$1.name);   //to propagate the id name
			//createTemp($$.offsetName); 
			$$.type=$1.type;
			$$.subtype=OFFSET;
			$$.datatype=$1.datatype;
			strcpy($$.offsetName,$3.name);
			opnd2.type = NOP;
			noDimensions = 1;
		}
	; 

Expr	: Expr _plus Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,PLUS,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			  	}
	| Expr _minus Expr	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,MINUS,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			  	}
 
	| Expr _mul Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,MUL,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
                         	}
	| Expr _div Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,DIV,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			 	}
	| Expr _modulo Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,MOD,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			    	}
	| _uminus Expr 		{
			  		createTemp($$.name);
					opnd2.type = NOP;
			  		addCode(quadTable,labelpending,UMINUS,$2,opnd2, $$);
			   		$$.type = TID;
			   		sym=copySymbol($2,$$.name);
			   		addSymbolHash(sym);
		       		}
	| Expr _lt Expr 	{
			  		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,LT,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
				}
	   
	| Expr _le Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,LE,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
				}
	| Expr _ge Expr 	{ 
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,GE,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
				}
	| Expr _gt Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,GT,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
				}
	| Expr _eq Expr 	{ 
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,EQ,$1,$3,$$);
			        	$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			    	}
	| Expr _ne Expr  	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,NE,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			      	}			
	| Expr _or Expr 	{
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,OR,$1,$3,$$);
			   		$$.type = TID;
			   		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
				}
	| Expr _and Expr 	{ 
			   		createTemp($$.name);
	 		   		addCode(quadTable,labelpending,AND,$1,$3,$$);
			   		$$.type = TID;
			  		sym=createTempSymbolWithType($1,$3,$$);
			   		addSymbolHash(sym);
			 	}
	| _leftp  Expr _rightp 	{
					$$ = $2;
			   		$$.type = TID;
				}
	| LValue 		{
					if($1.subtype==BASIC)
						$$=$1;
					else if($1.subtype == ARRAY)
					{
						opnd1 = $1;			//opnd1 is the base address of array
						strcpy(opnd1.name,$1.name);
						opnd1.type = TID;
						opnd1.subtype = BASIC;
						opnd1.datatype = $1.datatype;

						opnd2 = $1;		       //opnd2 is the displacement of array
						strcpy(opnd2.name,$1.offsetName);
						opnd2.type = TID;
						opnd2.subtype = BASIC;
						opnd2.datatype = $1.datatype;

						$$ = $1;
			   			createTemp($$.name);
						$$.type=TID;
						$$.subtype=ARRAY;
						$$.datatype=$1.datatype;
						printf("RASSIGN = %d\n",RASSIGN);
	 		   			addCode(quadTable,labelpending,RASSIGN,opnd1,opnd2,$$);
						sym = initSymbol();
						copyAttr2Symbol($$,&sym);
			   			addSymbolHash(sym);
						
					}
				}
	| _num 			{
					$$ = $1;
				}		
	| _dnum 		{
					$$=$1;
				}	
        ;

condition: Expr 		{
					$$ = $1;
				}
	| assignExpr		{
					$$ = $1;
				}
	;

ifStmt  : _if _leftp condition	{
					createLabel($1.falseLabel);
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = 0;
					createTemp(result.name);
//					result.type = CONST;
					result.type = LABEL;
					strcpy(result.name,$1.falseLabel);
					//result.datatype = FALSE;
//					result.value.string = malloc(strlen($1.falseLabel)+1);
					addCode(quadTable,labelpending,IFEQ,$3,opnd2,result);
                              	} 
	  _rightp Stmtlist 	{
                                	strcpy(labelpending,$1.falseLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					result.type=NOP;
					addCode(quadTable,labelpending,NOP,opnd1,opnd2,result);
				}
	;

whileStmt: _while		{ 
					createLabel($1.trueLabel); 
printf("In while->%s: ....\n",$1.trueLabel);
					strcpy(labelpending,$1.trueLabel);
				}
	_leftp condition	{
				      	createLabel($1.falseLabel);
printf("In while->%s: ....\n",$1.falseLabel);
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = 0;
					createTemp(result.name);
					result.type = LABEL;
					strcpy(result.name,$1.falseLabel);
					addCode(quadTable,labelpending,IFEQ,$4,opnd2,result);
				}
	_rightp Stmtlist	{
					opnd1.type = NOP;
					opnd2.type = NOP;
					result.type = LABEL;
					strcpy(result.name,$1.trueLabel);
					addCode(quadTable," ",GOTO,opnd1, opnd2,result);
                                	strcpy(labelpending,$1.falseLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					result.type=NOP;
					addCode(quadTable,labelpending,NOP,opnd1,opnd2,result);
				}
	;

doStmt  : _do 			{
					createLabel($1.trueLabel);
					strcpy(labelpending, $1.trueLabel);
	      			}
	Stmtlist _while _leftp condition 
				{
					createLabel($1.falseLabel);
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = 0;
					createTemp(result.name);
					//result.type = CONST;
					result.type = LABEL;
					//result.datatype = FALSE;
					strcpy(result.name,$1.falseLabel);
					addCode(quadTable,labelpending,IFEQ,$6,opnd2,result);
					opnd1.type = NOP;
					opnd2.type = NOP;
					createTemp(result.name);
					//result.type = CONST;
					result.type = LABEL;
					//result.datatype = TRUE;
					strcpy(result.name,$1.trueLabel);
					addCode(quadTable," ",GOTO,opnd1,opnd2,result);
                        		strcpy(labelpending,$1.falseLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					result.type=NOP;
					addCode(quadTable,labelpending,NOP,opnd1,opnd2,result);
				}
	_rightp _semicolon
	;

forStmt : _for			{
					createLabel($1.cond2Label);
					createLabel($1.cond3Label);
					createLabel($1.cond2TestLabel);
					createLabel($1.nextStmtLabel);
				} 
  _leftp condition _semicolon 	{ 
					strcpy(labelpending,$1.cond2Label);
				} 
	condition _semicolon 	{
					result.type=LABEL;
					strcpy(result.name,$1.cond2TestLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					addCode(quadTable," ",GOTO,opnd1,opnd2,result);
					//addCode(quadTable," ",GOTO," "," ",$1.cond2TestLabel);
					strcpy(labelpending,$1.cond3Label);
				} 
	condition _rightp 	{
					result.type=LABEL;
					strcpy(result.name,$1.cond2Label);
					opnd1.type=NOP;
					opnd2.type=NOP;
					addCode(quadTable," ",GOTO,opnd1,opnd2,result);
					strcpy(labelpending,$1.cond2TestLabel);
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = 0;
					result.type=LABEL;
					strcpy(result.name,$1.nextStmtLabel);
		     			addCode(quadTable,labelpending,IFEQ,$7,opnd2,result);
				} 
	Stmtlist 		{
					opnd1.type = NOP;
					opnd2.type = NOP;
					result.type=LABEL;
					strcpy(result.name,$1.cond3Label);
					addCode(quadTable," ",GOTO,opnd1,opnd2,result); 
					strcpy(labelpending,$1.nextStmtLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					result.type=NOP;
					addCode(quadTable,labelpending,NOP,opnd1,opnd2,result);
				}	
	;

breakStmt: _break _semicolon
	;

continueStmt: _continue _semicolon
	;

switchStmt: _switch _leftp condition 	{
					createTemp(switchFlagName);
					strcpy(switchExpResult,$3.name); 
					result.type=LABEL;
					strcpy(result.name,switchFlagName);
					opnd1.type = CONST;
					opnd1.datatype = INT;
					opnd1.value.iVal = 0;
					opnd2.type = NOP;
					addCode(quadTable," ",EQ,opnd1,opnd2,result);
				}
  _rightp _leftb caseStmts _rightb
	;

caseStmtPrefix: _case cons 	{
					createTemp(result.name);
					result.type = TID;
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = $2.value.iVal;
					strcpy(opnd1.name,switchExpResult);
					opnd1.type = TID;
	 		      		addCode(quadTable,labelpending,NE,opnd1,opnd2,result);
			      		createTemp(result.name); //str);
					result.type = TID;
			      		createTemp(opnd1.name); //str);
					opnd1.type = TID;
					strcpy(opnd1.name,switchFlagName); 
	 		      		addCode(quadTable,labelpending,OR,opnd1,opnd2,result); 
                              		createLabel(tempLabel);
					result.type=LABEL;
					strcpy(result.name,tempLabel);
					opnd2.type = CONST;
					opnd2.datatype = INT;
					opnd2.value.iVal = 0;
		              		addCode(quadTable,labelpending,IFEQ,opnd1,opnd2,result);
	            		}
	_colon Stmtlist 	{ 
					createTemp(result.name);
					result.type = TID;
					strcpy(result.name,switchFlagName); 
					opnd1.type = CONST;
					opnd1.datatype = INT;
					opnd1.value.iVal = 1;
					opnd2.type=NOP;
					addCode(quadTable," ",EQ,opnd1,opnd2,result);
					strcpy(labelpending,tempLabel);
					opnd1.type=NOP;
					opnd2.type=NOP;
					result.type=NOP;
					addCode(quadTable,labelpending,NOP,opnd1,opnd2,result);
				}
	;

caseStmts: caseStmtPrefix caseStmts 
	|
	;

cons	: _num 			{
					$$ = $1;
				}
	| _charcons 		{
					$$ = $1;
				}
 	| _default 		{
					$$.type = CONST;
					$$.datatype = INT;
					$$.value.iVal=999;
				}
	;

Stmtlist: _leftb Stmts _rightb 
	| Stmt 
	| _semicolon
	;

gotoStmt: _goto _id _semicolon
	; 

%%

int yyerror(char *errmsg)
{
	printf("Error has occurred\n",errmsg);
	exit(-1);
}

