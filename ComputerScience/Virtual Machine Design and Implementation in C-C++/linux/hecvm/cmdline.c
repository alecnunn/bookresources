/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ cmdline.c - processes the command-line                            +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations                                                      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define DBG_CMDLINE		1*/

#ifdef DBG_CMDLINE
#define DBG_CMDLINE0(str)				printf("handleArguments(): "); printf(str);
#define DBG_CMDLINE1(str,arg1)			printf("handleArguments(): "); printf(str,arg1);
#define DBG_CMDLINE2(str,arg1,arg2)		printf("handleArguments(): "); printf(str,arg1,arg2);
#else
#define DBG_CMDLINE0(str)
#define DBG_CMDLINE1(str,arg1)
#define DBG_CMDLINE2(str,arg1,arg2) 
#endif	

#define DEF_HEAP		64		/* default heap = 64KB = 65,536 bytes */
#define DEF_STACK		64		/* default stack = 64KB = 65,536 bytes */

struct CmdLine
{
	char *binaryFile;		/* name of the bytecode file to execute */
	U8 heapRequest;			/* RAM to allocate for heap in KB*/
	U8 stackRequest;		/* RAM to allocate for stack in KB*/
	U1 debug;				/* if TRUE, start in debug mode */
	U1 ok;					/*TRUE or FALSE*/
};

#define MAX_PROGRAM_ARGS	32
struct ProgramArgs
{
	char *args[MAX_PROGRAM_ARGS];	/* program arguments */
	U1 nArgs;						/* number of arguments */
};
struct ProgramArgs programArgs;

struct ApplicationMetaData
{
	char *fileName;		/*cmdline.c in bytes*/
	U8 fileSize;		/*init.c*/
	U8 szSymTbl;		/*init.c*/
	U8 szStrTbl;		/*init.c*/
	U8 szByteCode;		/*init.c*/
	U8 bCodeFileStart;	/*init.c*//* file information */
	U8 bCodeFileEnd;	/*init.c*//*----------------------*/
	U8 ram;				/*init.c*//* memory information */
	U8 heapSize;		/*init.c in bytes*/
	U8 stackSize;		/*init.c in bytes*/
};

struct ApplicationMetaData appMetaData;	/*global used by debugger*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ private prototypes                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printUsage();

void badHeapSize(char *str);
void badHeapOption();

void badStackSize(char *str);
void badStackOption();

void badOptionChar(char ch);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct CmdLine handleArguments(int argc, char *argv[]);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void printUsage()
{
	printf("\n\tHECVM [options] file.RUN [arguments]\n\n");
	
	printf("\tvirtual machine options:\n\n");

	printf("\t-h=XXXX\tmemory to allocate for heap in KB\n");
	printf("\t-s=XXXX\tmemory to allocate for stack in KB\n");
	printf("\t-d\tenable debug mode\n");
	printf("\t-?\tprint help\n\n");

	printf("\tthere are no spaces in the -h and -s options!\n\n");
	return;

}/*end printUsage*/

/* ----------------------------------------------------------------*/

void badHeapSize(char *str)
{
	ERROR1_LVL2("badHeapSize(): bad heap request (%s)",str);
	ERROR0_LVL2("badHeapSize(): require positive integer");
	return;

}/*end badHeapSize*/

void badHeapOption()
{
	ERROR0_LVL2("badHeapOption(): incorrect \'-h\' switch");
	return;

}/*end badHeapOption*/

void badStackSize(char *str)
{
	ERROR1_LVL2("badStackSize(): bad stack request (%s)",str);
	ERROR0_LVL2("badStackSize(): require positive integer");
	return;

}/*end badStackSize*/

void badStackOption()
{
	ERROR0_LVL2("badStackOption(): incorrect \'-s\' switch");
	return;

}/*end badStackOption*/

void badOptionChar(char ch)
{
	ERROR1_LVL2("badOptionChar(): \'-%c\' is invalid",ch);
	return;

}/*end badOptionChar*/

/* ----------------------------------------------------------------*/

/*
	processes the command-line

	command-line looks like:
		
			hecvm [options] file.RUN [arguments]

		file.RUN - name of the bytecode file to execute
		arguments - command line parameters passed to program
	
		Options
		-------

		-h=XXXX
			Sets amount of physical RAM to allocate for heap in KB
			( default is 64 KB )

		-s=XXXX
			Sets amount of physical RAM to allocate for stack in KB
			( default is 64 KB )

		-d
			enables debug moode

		-? 
			Prints usage and version information to the console
			
		Note: use 1-character options so that we can avoid using 
		      strcmp to see if an argument is an option

		Note: argv[0] = name of executable
		      argv[1] = first command line argument
			  argv[2] = second command line argument
			  ... etc. 
*/

struct CmdLine handleArguments(int argc, char *argv[])
{
	struct CmdLine cmdline;
	int i;
	int name_length;
	
	/* set default values */

	cmdline.binaryFile = NULL;
	cmdline.heapRequest = DEF_HEAP;		
	cmdline.stackRequest = DEF_STACK;
	cmdline.debug = FALSE;
	cmdline.ok = TRUE;

	programArgs.nArgs = 0;

	/* handle case when only 1 argument ( name of executable ) */

	if(argc==1)
	{
		ERROR0_LVL2("handleArguments(): no arguments");
		printUsage();
		cmdline.ok = FALSE;
		return(cmdline);
	}

	/* handle virtual machine options */
	/* 
		-h=XXX, -s=XXX, -d, -?
		argv[i][0]= '-'
		argv[i][1]= letter
		argv[i][2]= '='
		argv[i][3]= number
	*/

	i=1;
	while(argv[i][0]=='-')
	{
		switch(argv[i][1])
		{	
			case 'h':
			case 'H':
			{
				if(argv[i][2]=='=')
				{
					char *stop;
					S4 temp;
					temp = strtol( &argv[i][3], &stop, 10 );
					if(temp<=0)
					{
						badHeapSize(argv[i]);
						printUsage();
						cmdline.ok = FALSE;
						return(cmdline);
					}
					cmdline.heapRequest = (U8)temp;
					DBG_CMDLINE0("heap size set\n");
				}
				else
				{
					badHeapOption();
					printUsage();
					cmdline.ok = FALSE;
					return(cmdline);
				}
			}break;
			case 's':
			case 'S':
			{
				if(argv[i][2]=='=')
				{
					char *stop;
					S4 temp;
					temp = strtol( &argv[i][3], &stop, 10 );
					if(temp<=0)
					{
						badStackSize(argv[i]);
						printUsage();
						cmdline.ok = FALSE;
						return(cmdline);
					}
					cmdline.stackRequest = (U8)temp;
					DBG_CMDLINE0("stack size set\n");
				}
				else
				{
					badStackOption();
					printUsage();
					cmdline.ok = FALSE;
					return(cmdline);
				}
			}break;
			case 'd':
			case 'D':
			{
				cmdline.debug = TRUE;
				DBG_CMDLINE0("debug switch toggled\n");

			}break;
			case '?':
			{ 
				printUsage(); 
				cmdline.ok = FALSE;
				return(cmdline);

			}break;
			default:
			{
				badOptionChar(argv[i][1]);
				printUsage();
				cmdline.ok = FALSE;
				return(cmdline);
			}
		}/*end switch*/

		i++;
		if(i==argc)
		{
			ERROR0_LVL2("handleArguments(): executable missing");
			printUsage();
			cmdline.ok = FALSE;
			return(cmdline);
		}

	}/*end while-loop */

	/* set file name, check for .RUN extension */

	name_length = strlen(argv[i]);
			
	if(name_length<4)
	{
		ERROR0_LVL2("handleArguments(): no .RUN suffix");
		printUsage();
		cmdline.ok = FALSE;
		return(cmdline);
	}

	if((argv[i][name_length-4]=='.')&&
	   (argv[i][name_length-3]=='R')&&
	   (argv[i][name_length-2]=='U')&&
	   (argv[i][name_length-1]=='N'))
	{
		cmdline.binaryFile = argv[i];
		appMetaData.fileName = argv[i];
		i++;
	}
	else
	{
		ERROR0_LVL2("handleArguments(): missing .RUN suffix");
		printUsage();
		cmdline.ok = FALSE;
		return(cmdline);
	}

	/* collect program arguments */

	while(i<argc)
	{
		if(programArgs.nArgs<MAX_PROGRAM_ARGS)
		{
			programArgs.args[programArgs.nArgs]=argv[i];
			programArgs.nArgs++;
		}
		i++;
	}

	/* summarize command line */

	DBG_CMDLINE0("Command line summary------------------\n\n");
	
	DBG_CMDLINE1("\theapRequest=%lu KB\n",cmdline.heapRequest);
	DBG_CMDLINE1("\tstackRequest=%lu KB\n",cmdline.stackRequest);
	if(cmdline.debug == TRUE)
	{ 
		DBG_CMDLINE0("\tdebugging enabled\n\n"); 
	}
	else
	{ 
		DBG_CMDLINE0("\tdebugging mode OFF\n\n"); 
	}

	DBG_CMDLINE1("\tfile to load=%s\n\n",cmdline.binaryFile);

	for(i=0;i<programArgs.nArgs;i++)
	{
		DBG_CMDLINE2("arg[%d]=%s\n",i,programArgs.args[i]);
	}
	return(cmdline);

}/*end handleArguments*/