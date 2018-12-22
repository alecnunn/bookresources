/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ error.c - centralize syntax and symantic error handling           +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int maxErrors;  /*maximum # errors before shutdown, default = 10 */
int nErrors;	/*keep track of number of errors*/

/*
	suffix indicates number of arguments in printf()
*/

#define ERROR0(str);				printf("error: "); printf(str); incErrors();
#define ERROR1(str,arg1);			printf("error: "); printf(str,arg1); incErrors();
#define ERROR2(str,arg1,arg2);		printf("error: "); printf(str,arg1,arg2); incErrors();
#define ERROR3(str,arg1,arg2,arg3);	printf("error: "); printf(str,arg1,arg2,arg3); incErrors();
#define ERROR4(str,arg1,arg2,arg3,arg4);		printf("error: "); printf(str,arg1,arg2,arg3,arg4); incErrors();
#define ERROR5(str,arg1,arg2,arg3,arg4,arg5);	printf("error: "); printf(str,arg1,arg2,arg3,arg4,arg5); incErrors();
#define FATAL_ERROR();				printf("shutting down\n");shutDown(SHUTDOWN_ERROR);	

void incErrors()
{
	nErrors++;
	if(nErrors >= maxErrors)
	{
		printf("incErrors(): Have hit maximum number of errors ->(%d)\n",maxErrors);
		FATAL_ERROR();
	}
	return;

}/*end incErrrors*/
