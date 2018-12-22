/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ proc.cpp - procedure meta data repository                         +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ decalaration                                                      + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct Procedure
{
	U8 text;		/* index into StrTbl of where identifier begins */
	U8 address;		/* address of procedure */
	U4 line;		/* source code line containing declaration */

	struct StackFrame ret;
	U1 nRet;		/* 0 = void return, 1 = returns a value*/
	struct StackFrame *arg;
	U1 nArg;
	U1 iArg;
	struct StackFrame *local;
	U1 nLocal;
	U1 iLocal;
	struct Label *label;
	U2 nLabel;
	U2 iLabel;
};
