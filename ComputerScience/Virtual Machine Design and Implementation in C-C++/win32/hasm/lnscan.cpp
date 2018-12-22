/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+	lnscan.cpp                                                      +
+                                                                   +
+	This class takes an input file and breaks it into a stream of   + 
+	null-char terminated lines ( represented by Line struct )       +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Macros                                                            +                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*#define INPUT_SCANNER_DEBUG		1*/

#ifdef INPUT_SCANNER_DEBUG
#define	INPUT_SCAN_DEBUG0(arg);				printf("LineScanner::");printf(arg);
#define INPUT_SCAN_DEBUG1(arg1,arg2);		printf("LineScanner::");printf(arg1,arg2);
#define INPUT_SCAN_DEBUG2(arg1,arg2,arg3);	printf("LineScanner::");printf(arg1,arg2,arg3);
#else
#define	INPUT_SCAN_DEBUG0(arg);
#define INPUT_SCAN_DEBUG1(arg1,arg2);
#define INPUT_SCAN_DEBUG2(arg1,arg2);
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define LINE_SIZE   512		/*max length of line of assembly code*/

/*flag from getInputChar(), indicates have hit EOF*/

#define IN_END		-1		

struct Line
{
	char src[LINE_SIZE];	/*single line of assembler code*/
	unsigned long line;		/*line number*/
	char *fName;			/*file reading assembly code from*/
	unsigned char end;		/*(TRUE|FALSE), indicates we've hit EOF*/
};

class LineScanner
{
	FILE *iFptr;					/*pointer to assembly code file*/
	char iFname[FNAME_SIZE];		/*name of assembly code file*/
	char inBuffer[BUFFER_SIZE];		/*buffer for input from assembly file*/
	int	nInBuffer;					/*number of bytes in inBuffer*/
	int	inBufferPos;				/*index into inBuffer: 0 -> nInBuffer-1 */
	unsigned long inLine;			/*current line in file*/

	void fillInputBuffer();
	char getInputChar();
	void prep(char ch, char *str);

	public:
	LineScanner(CommandLine *cl_obj);
	~LineScanner();

	struct Line getInputLine();
	void printScannedLine(struct Line *lptr);

	void test();
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LineScanner::LineScanner(CommandLine *cl_obj)
{
	if(((*cl_obj).inputFile==NULL)||(strlen((*cl_obj).inputFile)==0))
	{ 
		INPUT_SCAN_DEBUG0("LineScanner(): file name empty\n");
		strcpy(iFname,(*cl_obj).nullName);
	}
	else
	{  
		strncpy(iFname,(*cl_obj).inputFile,FNAME_SIZE);
		iFname[FNAME_SIZE-1]='\0';
		INPUT_SCAN_DEBUG1("LineScanner(): opened %s\n",iFname);
	}

	iFptr = fopen(iFname,"rb");
	if(iFptr==NULL)
	{ 
		printf("LineScanner::LineScanner(): could not open %s\n",iFname); 
		exit(SHUTDOWN_ERROR); 
	}
	inLine = 1;
	fillInputBuffer();
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

LineScanner::~LineScanner()
{
	if(iFptr!=NULL)
	{
		if(fclose(iFptr))
		{ 
			printf("LineScanner::~LineScanner(): ");
			printf("could not close %s\n",iFname);  
		}
	}
	else
	{
		INPUT_SCAN_DEBUG0("~LineScanner(): null file pointer\n"); 
	}

	INPUT_SCAN_DEBUG0("~LineScanner(): closed file\n"); 

	return;

}/*end destructor*/

/*-----------------------------------------------------------------*/

void LineScanner::fillInputBuffer()
{
	int nbytes;

	nbytes=0;
	nbytes = fread(inBuffer,sizeof(char),BUFFER_SIZE,iFptr);

	/*if nbytes is less than BUFFER_SIZE, have hit EOF or have an error*/

	if(nbytes<BUFFER_SIZE)
	{
		if(feof(iFptr)==0)
		{ 
			printf("LineScanner::fillInputBuffer(): ");
			printf("error reading from %s\n",iFname); 
			exit(SHUTDOWN_ERROR); 
		}
		else
		{
			/*add a byte to buffer size and set it to IN_END*/

			nInBuffer = nbytes+1;
			inBuffer[nInBuffer-1] = IN_END;
			INPUT_SCAN_DEBUG0("fillInputBuffer(): hit EOF filling buffer\n");
		}
	}
	else
	{
		nInBuffer = BUFFER_SIZE;
	}

	INPUT_SCAN_DEBUG1("fillInputBuffer(): read %d bytes\n",nInBuffer);

	inBufferPos = -1;  /*init for first call to getInputChar()*/
	return;

}/*end fillInputBuffer*/

/*-----------------------------------------------------------------*/

char LineScanner::getInputChar()
{ 
	//char str[4];

	inBufferPos++;

	/*
	if read entire buffer (0-(nInBuffer-1)) 
	and are at the char following the end (nInBuffer) 
	then fill in a new one
	*/

	if(inBufferPos == nInBuffer)
	{
		INPUT_SCAN_DEBUG0("getInputBufferChar(): hit end, filling buffer\n");
		fillInputBuffer();
		inBufferPos++;	/*fillInputBuffer set to -1*/
	}

	/*if at very last char of buffer, look at last char for EOF flag*/

	if(inBufferPos == nInBuffer-1)
	{
		if(inBuffer[inBufferPos]==IN_END)
		{
			INPUT_SCAN_DEBUG0("getInputBufferChar():hit EOF\n");
			/*make sure always stay at end, if call again*/
			inBufferPos--; 
			return(IN_END);
		}
	}

	//prep(inBuffer[inBufferPos],str)
	//INPUT_SCAN_DEBUG1("getInputBufferChar():read %s\n",str);

	return(inBuffer[inBufferPos]);
	
}/*end getInputChar*/

/*-----------------------------------------------------------------*/

/* prepares a charater to be printed to screen */

void LineScanner::prep(char ch,char* str)
{
	switch(ch)
	{
		case '\n':
		{
			strcpy(str,"NL");
			return;
		}
		case '\r':
		{
			strcpy(str,"CR");
			return;
		}
		case '\t':
		{
			strcpy(str,"TAB");
			return;
		}
		case ' ':
		{
			strcpy(str,"SP");
			return;
		}
	}
	str[0]=ch;
	str[1]='\0';
	return;

}/*end prep*/

/*-----------------------------------------------------------------*/

struct Line LineScanner::getInputLine()
{
	struct Line line;
	char ch;
	int i;
	
	i=0;
	line.end = FALSE;
	line.fName = iFname; 

	/*get first non-whitspace character*/

	ch = getInputChar();

	while((ch=='\n')||(ch=='\r'))
	{ 
		if(ch=='\n'){ inLine++; }
		ch = getInputChar(); 
	}

	/*keep reading until hit end of line, file, or buffer*/

	while((ch!='\n')&&(ch!=IN_END)&&(i<LINE_SIZE-1))
	{
		if(ch=='\r'){ ch = getInputChar(); }
		else
		{
			line.src[i]=ch;
			ch = getInputChar();
			i++;
		}
	}

	if(ch==IN_END){ line.end = TRUE; }

	line.src[i]='\0';
	line.line = inLine;
	inLine++; 
	INPUT_SCAN_DEBUG1("getInputLine(): scanned %s\n",line.src); 
	return(line);

}/*end getInputLine*/

/*-----------------------------------------------------------------*/

void LineScanner::printScannedLine(struct Line *lptr)
{
	printf("file=%s ",(*lptr).fName);
	printf("line=%lu ",(*lptr).line);
	printf("text=%s\n",(*lptr).src);
	return;

}/*end printScannedLine*/

/*-----------------------------------------------------------------*/

/*
 this test driver demonstrates usage
*/

void LineScanner::test()
{
	struct Line ln;
	
	ln = getInputLine();
	while(ln.end!=TRUE)
	{
		printScannedLine(&ln);
		ln = getInputLine();
	}

	return;

}/*end test*/