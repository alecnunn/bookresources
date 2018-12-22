/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ error.c - facilities used to handle errors                        +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define FATAL_ERROR();		shutDown(SHUTDOWN_ERROR);
#define FATAL_ERROR1(str);	printf(str);printf("\n");xmlBegin();fprintf(errPtr,str);xmlEnd();shutDown(SHUTDOWN_ERROR);

#define ERROR_LVL1(str);			FATAL_ERROR1(str);	
#define ERROR0_LVL2(str);			printf(str);printf("\n");xmlBegin();fprintf(errPtr,str);xmlEnd();
#define ERROR1_LVL2(str,arg1);		printf(str,arg1);printf("\n");xmlBegin();fprintf(errPtr,str,arg1);xmlEnd();
#define ERROR2_LVL2(str,arg1,arg2);	printf(str,arg1,arg2);printf("\n");xmlBegin();fprintf(errPtr,str,arg1,arg2);xmlEnd();

#define FCLOSE(arg);	if(fclose(arg)){ printf("could not close executable file\n"); }

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private variables                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

FILE *errPtr = NULL;
char errFileName[128];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private prototypes                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void xmlBegin(); /* print beginning of XML error message markup*/
void xmlEnd(); /* print end of XML error message markup*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void setUpErrorFile();	
void shutDown(U1 code);
	
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void setUpErrorFile()
{
	time_t now;
	struct tm local_time;
	char digits[16];

	int retval;

	now = time(NULL);
	local_time = *(localtime(&now));

	strcpy(errFileName,"VM_ERROR_");
	
	sprintf(digits,"%u",(local_time.tm_mon+1));
	strcat(errFileName,digits);
	strcat(errFileName,"_");

	sprintf(digits,"%u",local_time.tm_mday);
	strcat(errFileName,digits);
	strcat(errFileName,"_");

	sprintf(digits,"%lu",(local_time.tm_year+1900));
	strcat(errFileName,digits);
	strcat(errFileName,"_");

	sprintf(digits,"%u",local_time.tm_hour);
	strcat(errFileName,digits);
	strcat(errFileName,"_");

	sprintf(digits,"%u",local_time.tm_min);
	strcat(errFileName,digits);
	strcat(errFileName,"_");

	sprintf(digits,"%u",local_time.tm_sec);
	strcat(errFileName,digits);

	strcat(errFileName,".XML");

	if((errPtr=fopen(errFileName,"wb"))==NULL)
	{
		printf("setUpErrorFile(): error opening %s\n",errFileName);
		FATAL_ERROR();
	}

	retval = fprintf(errPtr,"<ERRORS>\r\n");
	if(retval < 0)
	{
		printf("setUpErrorFile(): "); 
		printf("error writing to %s\n",errFileName);
		FATAL_ERROR();
	}

	return;

}/*end setUpErrorFile*/

void shutDown(U1 code)
{
	if(RAM != NULL){ free(RAM); }
	else{  printf("shutDown(): no RAM[] allocated\n"); }
	
	if(errPtr!=NULL)
	{
		int retval;
		retval = fprintf(errPtr,"</ERRORS>");
		if(retval < 0)
		{
			printf("shutDown(): "); 
			printf("error writing </ERROR> to %s\n",errFileName);
		}
		if(fclose(errPtr))
		{
			printf("shutDown(): error closing %s\n",errFileName);
		}
	}
	
	exit(code);

}/*end shutDown*/

void xmlBegin()
{
	char str1[]="<Message>\r\n\t";
	char str2[]="<time>%s</time>\r\n\t<content>";
	char datestr[64];

	time_t now;
	int retval;
	int i;

	retval = fprintf(errPtr,str1);
	if(retval < 0)
	{
		printf("xmlBegin(): "); 
		printf("error writing to %s\n",errFileName);
		return;
	}
	
	/*get rid of '\n' at end of datestr created by ctime()*/
	now = time(NULL);
	strcpy(datestr,ctime(&now));
	retval=strlen(datestr);
	for(i=0;i<retval;i++){ if(datestr[i]=='\n'){ datestr[i]='\0'; } }

	retval = fprintf(errPtr,str2,datestr);
	if(retval < 0)
	{
		printf("xmlBegin(): "); 
		printf("error writing to %s\n",errFileName);
	}
	return;

}/*end xmlBegin*/

void xmlEnd()
{
	int retval;

	retval = fprintf(errPtr,"</content>\r\n</Message>\r\n");
	if(retval < 0)
	{
		printf("xmlEnd(): "); 
		printf("error writing to %s\n",errFileName);
	}
	return;

}/*end xmlEnd*/



