/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+	cmdline.cpp - processes the command-line                        +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Macros                                                            +                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define CMD_LINE_DEBUG		1*/

#ifdef	CMD_LINE_DEBUG
#define	CMD_LINE_DEBUG0(arg);				printf("handleArgs(): ");printf(arg);
#define CMD_LINE_DEBUG1(arg1,arg2);			printf("handleArgs(): ");printf(arg1,arg2);
#define CMD_LINE_DEBUG2(arg1,arg2,arg3);	printf("handleArgs(): ");printf(arg1,arg2,arg3);
#else
#define	CMD_LINE_DEBUG0(arg);
#define CMD_LINE_DEBUG1(arg1,arg2);
#define CMD_LINE_DEBUG2(arg1,arg2);
#endif

#define FNAME_SIZE	255		/*maximum size of file name*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class CommandLine
{
	/* file extensions */

	char IN_EXT[5];		/*source*/
	char TMP_EXT[5];	/*intermediate*/
	char LST_EXT[5];	/*listing*/
	char OUT_EXT[5];	/*executable*/

	void addFileSuffix(char *out, char *in, char *ext);
	void printUsage();

	public:
	char inputFile[FNAME_SIZE];		/*source file*/
	char listFile[FNAME_SIZE];		/*listing file*/
	char tempFile[FNAME_SIZE];		/*temporary bytecode*/
	char outputFile[FNAME_SIZE];	/*compilation unit*/

	char nullName[FNAME_SIZE];	/*default name of file*/

	U1 omitDebugData;			/*TRUE if omit debug data*/
	U1 listing;					/*TRUE if desire a listing*/
	int *maxErrors;				/*maximum number of errors*/				

	CommandLine(int *merros);
	U1 handleArgs(int argc,char *arg[]);
	
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

CommandLine::CommandLine(int *merrors)
{
	strcpy(IN_EXT,".ASM");	
	strcpy(TMP_EXT,".TMP");	
	strcpy(LST_EXT,".LST");	
	strcpy(OUT_EXT,".RUN");	

	strcpy(nullName,"UN-NAMED");

	inputFile[0]='\0';
	listFile[0]='\0';
	tempFile[0]='\0';
	outputFile[0]='\0';

	omitDebugData = FALSE;
	listing = FALSE;
	maxErrors = merrors;
	*maxErrors = 10;
	
	return;

}/*end CommandLine*/

/*-----------------------------------------------------------------*/
/*
	in = original file 
	out = new file name
	ext = new extension
*/

void CommandLine::addFileSuffix(char *out, char *in, char *ext)
{
	char *cptr;
	int index;

	if((out==NULL)||(in==NULL)||(ext==NULL))
	{
		ERROR0("addFileSuffixe(): null pointer args\n");
		FATAL_ERROR();
	}

	cptr = strrchr(in,'.');

	/*if no extension, then just add suffix*/

	if(cptr==NULL)
	{
		strcpy(out,in);
		strcat(out,ext);
	}

	/*otherwise, copy until we hit '.' and then add suffx*/

	else
	{
		index =0;
		while((in[index]!='.')&&(index<FNAME_SIZE-4))
		{
			out[index]=in[index];
			index++;
		}
		out[index]='\0';
		strncat(out,ext,4);
	}
	return;

}/*end addFileSuffix*/

/* ----------------------------------------------------------------*/

void CommandLine::printUsage()
{
	printf("\nHASM file.ASM [options]\n\n");
	printf("\tOptions:\n\n");
	
	printf("\t-d\n\t\tomit debug information\n");
	printf("\t-e=XXX\n\t\tnumber of errors to report\n");
	printf("\t-l\n\t\tproduce a listing file\n");
	printf("\t-o=XXX\n\t\tname of output file (compilation unit)\n");

	printf("\n\tthere are no spaces between option characters\n\n");
	return;

}/*end printUsage*/

/* ----------------------------------------------------------------*/

/*
	processes the command-line arguments. 

	Note: argv[0] = name of executable
		  argv[1] = first command line argument
		  argv[2] = second command line argument
		  ... etc. 
*/

U1 CommandLine::handleArgs(int argc,char *argv[])
{
	int i;

	/* no arguments */

	if(argc==1)
	{
		CMD_LINE_DEBUG0("No Arguments\n");
		printUsage();
		return(FALSE);
	}

	/* get input file name "filename.RUN" */

	if(argv[1][0]!='-')
	{
		int len;
		strncpy(inputFile,argv[1],FNAME_SIZE);
		inputFile[FNAME_SIZE-1]='\0';
		len = strlen(inputFile);
			
		if(len<4)
		{
			ERROR0("handleArgs(): expecting .ASM file\n");
			return(FALSE);
		}
			
		if(!((inputFile[len-4]=='.')&&
			((inputFile[len-3]=='A')||(inputFile[len-3]=='a'))&&
			((inputFile[len-2]=='S')||(inputFile[len-2]=='s'))&&
			((inputFile[len-1]=='M')||(inputFile[len-1]=='m'))))
		{
			ERROR0("handleArgs(): expecting .ASM file\n");
			return(FALSE);
		}
	}
	else
	{
		printUsage();
		return(FALSE);
	}

	/* process options */	

	for(i=2;i<argc;i++)
	{ 
		if(argv[i][0]=='-')
		{
			switch(argv[i][1])
			{	
				case 'd':
				case 'D':
				{
					omitDebugData=TRUE;

					if(argv[i][2]!='\0')
					{
						ERROR0("handleArgs(): bad \'-d\' switch\n");
						printUsage();
						return(FALSE);
					}

					CMD_LINE_DEBUG0("omitDebugData set to TRUE\n");

				}break;
				case 'e':
				case 'E':
				{
					char intstr[32];

					if(argv[i][2]=='=')
					{
						strcpy(intstr, &argv[i][3]);
						*maxErrors = atoi(intstr);
						*maxErrors = abs(*maxErrors);
					}
					else
					{
						ERROR0("handleArgs(): bad \'-e\' switch\n");
						printUsage();
						return(FALSE);
					}

					CMD_LINE_DEBUG0("number of errors set\n");

				}break;
				case 'l':
				case 'L':
				{
					listing = TRUE;

					if(argv[i][2]!='\0')
					{
						ERROR0("handleArgs(): bad \'-l\' switch\n");
						printUsage();
						return(FALSE);
					}

					CMD_LINE_DEBUG0("ListFile set to TRUE\n");

				}break;
				case 'o':
				case 'O':
				{
					/* output file must look like ( file.RUN ) */

					if(argv[i][2]=='=')
					{
						int len;

						strncpy(outputFile, &argv[i][3], FNAME_SIZE);
						outputFile[FNAME_SIZE-1]='\0';
						len = strlen(outputFile);

						if(len<4)
						{
							ERROR0("handleArgs(): bad -o=file\n");
							return(FALSE);
						}
			
						if(!((outputFile[len-4]=='.')&&
						(outputFile[len-3]=='R')&&
						(outputFile[len-2]=='U')&&
						(outputFile[len-1]=='N')))
						{
							ERROR0("handleArgs(): bad -o=file\n");
							return(FALSE);
						}
							
						CMD_LINE_DEBUG0("output file name set\n");
					}
					else
					{
						ERROR0("handleArgs(): bad \'-o\' switch\n");
						printUsage();
						return(FALSE);
					}

				}break;
				default:
				{
					ERROR1("handleArgs(): %s bad option\n",argv[i]);
					printUsage();
					return(FALSE);
				}

			}/*end-switch*/

		}/*end if*/
		/*not an option, does not begin with dash*/
		else
		{
			printUsage();
			return(FALSE);
		}

	}/*end for-loop*/

	if(outputFile[0]=='\0')
	{ 
		addFileSuffix(outputFile,inputFile,OUT_EXT); 
	}

	addFileSuffix(tempFile,outputFile,TMP_EXT); 
	addFileSuffix(listFile,outputFile,LST_EXT);
	
	CMD_LINE_DEBUG0("Command line summary\n");
	CMD_LINE_DEBUG1("\tfile to load=%s\n",inputFile);
	CMD_LINE_DEBUG1("\toutput file=%s\n",outputFile);
	CMD_LINE_DEBUG1("\ttemp file=%s\n",tempFile);
	CMD_LINE_DEBUG1("\tlist file=%s\n",listFile);
	CMD_LINE_DEBUG1("\terrors to report=%d\n",*maxErrors);
	
	if(listing == TRUE)
	{ 
		CMD_LINE_DEBUG0("\tlisting is ON\n"); 
	}
	else
	{ 
		CMD_LINE_DEBUG0("\tlisting is OFF\n"); 
	}
	
	if(omitDebugData == TRUE)
	{ 
		CMD_LINE_DEBUG0("\tomitting debug data\n\n"); 
	}
	else
	{ 
		CMD_LINE_DEBUG0("\tdebug data included\n\n"); 
	}

	return(TRUE);

}/*end handleArguments*/