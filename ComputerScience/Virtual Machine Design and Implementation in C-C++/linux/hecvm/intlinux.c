/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  intlinux.c - the file has all the linux interrupt code           +
+            ( native system calls are needed to implement          +
+              basic I/O functionality )                            +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handlefileIO                                                     +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handlefileIO()
{
	int retval;

	switch((U1)R[$R1])
	{
		case 0:	
		{
			char *modes[]={"rb","wb","ab","wb+","ab+"};
			FILE *fptr;
			if(R[$R3]>4){ R[$R4]=0; return; }
			fptr = fopen(&RAM[R[$R2]],modes[R[$R3]]);
			if(fptr==NULL){ R[$R4]=0; }else{ R[$R4] = (U8)fptr; }

		}break;
		case 1: 
		{
			R[$R3]=fclose((FILE *)R[$R2]);

		}break;
		case 2: 
		{
			R[$R3]=fflush((FILE *)R[$R2]);
			
		}break;
		case 3: 
		{
			R[$R3]=fflush(stdin);
			
		}break;
		case 4: 
		{
			R[$R3]=fflush(stdout);
			
		}break;
		case 5: 
		{
			R[$R3]=fflush(stderr);
			
		}break;
		case 6: 
		{
			rewind((FILE *)R[$R2]);

		}break;
		case 7:
		{
			long pos;
			pos = ftell((FILE *)R[$R2]);
			if(pos==-1L){ R[$R3]=0; R[$R4]=1; }
			else{ R[$R3]=pos; R[$R4]=0; }

		}break;
		case 8:
		{
			switch(R[$R4])
			{
				case 0:{ R[$R5] = fseek((FILE *)R[$R2],(long)R[$R3],SEEK_SET); }break;
				case 1:{ R[$R5] = fseek((FILE *)R[$R2],(long)R[$R3],SEEK_END); }break;
				case 2:{ R[$R5] = fseek((FILE *)R[$R2],(long)R[$R3],SEEK_CUR); }break;
				default:{ R[$R5] = fseek((FILE *)R[$R2],(long)R[$R3],SEEK_SET); }
			}

		}break;
		case 9: 
		{
			R[$R3]=feof((FILE *)R[$R2]);
			
		}break;
		case 10: 
		{
			R[$R3]=ferror((FILE *)R[$R2]);
			
		}break;
		case 11:
		{
			size_t nbytes;
			nbytes = fread(&RAM[R[$R4]],1,(size_t)R[$R3],(FILE *)R[$R2]);
			if(nbytes<(size_t)R[$R3])
			{
				if(feof((FILE *)R[$R2])){ R[$R5]=nbytes; R[$R6]=0; }
				else if(ferror((FILE *)R[$R2])){ R[$R5]=nbytes; R[$R6]=1;}
				else{ R[$R5]=nbytes; R[$R6]=0; }
			}
			else{ R[$R5]=nbytes; R[$R6]=0; }

		}break;
		case 12:
		{
			size_t nbytes;
			nbytes = fwrite(&RAM[R[$R4]],1,(size_t)R[$R3],(FILE *)R[$R2]); 
			if(nbytes<(size_t)R[$R3])
			{
				if(feof((FILE *)R[$R2])){ R[$R5]=nbytes; R[$R6]=0; }
				else if(ferror((FILE *)R[$R2])){ R[$R5]=nbytes; R[$R6]=1;}
				else{ R[$R5]=nbytes; R[$R6]=0; }
			}
			else{ R[$R5]=nbytes; R[$R6]=0; }

		}break;
		case 13:
		{
			size_t nbytes;
			nbytes = fread(&RAM[R[$R4]],1,(size_t)R[$R3],stdin);
			if(nbytes<(size_t)R[$R3])
			{
				if(feof(stdin)){ R[$R5]=nbytes; R[$R6]=0; }
				else if(ferror(stdin)){ R[$R5]=nbytes; R[$R6]=1;}
				else{ R[$R5]=nbytes; R[$R6]=0; }
			}
			else{ R[$R5]=nbytes; R[$R6]=0; }

		}break;
		case 14:
		{
			size_t nbytes;
			nbytes = fwrite(&RAM[R[$R4]],1,(size_t)R[$R3],stdout);
			if(nbytes<(size_t)R[$R3])
			{
				if(feof(stdout)){ R[$R5]=nbytes; R[$R6]=0; }
				else if(ferror(stdout)){ R[$R5]=nbytes; R[$R6]=1;}
				else{ R[$R5]=nbytes; R[$R6]=0; }
			}
			else{ R[$R5]=nbytes; R[$R6]=0; }

		}break;
		case 15:
		{
			size_t nbytes;
			nbytes = fwrite(&RAM[R[$R4]],1,(size_t)R[$R3],stderr);
			if(nbytes<(size_t)R[$R3])
			{
				if(feof(stderr)){ R[$R5]=nbytes; R[$R6]=0; }
				else if(ferror(stderr)){ R[$R5]=nbytes; R[$R6]=1;}
				else{ R[$R5]=nbytes; R[$R6]=0; }
			}
			else{ R[$R5]=nbytes; R[$R6]=0; }

		}break;
		case 16: 
		{
			retval = printf("%c",(char)R[$R2]);
			if(retval < 0){ R[$R3]=0; R[$R4]=1; }
			else{ R[$R3]=retval; R[$R4]=0; }

		}break;
		case 17: 
		{
			printf("Warning (INT 0, $R1=17) no wide char support!\n");
			R[$R3]=0; R[$R4]=1;

		}break;
		case 18:
		{
			retval = printf("%s",(char *)&RAM[R[$R2]]);
			if(retval < 0){ R[$R3]=0; R[$R4]=1; }
			else{ R[$R3]=retval; R[$R4]=0; }

		}break;
		case 19:
		{
			printf("Warning (INT 0, $R1=19) no wide character support!\n");
			R[$R3]=0; R[$R4]=0;

		}break;
		case 20: /* note signed vs. unsigned */
		{
			if(R[$R3]==1){ retval = printf("%llu",R[$R2]); }
			else{ retval = printf("%lld",R[$R2]); }
			if(retval < 0){ R[$R3]=0; R[$R4]=1; }
			else{ R[$R3]=retval; R[$R4]=0; }

		}break;
		case 21:
		{
			retval = printf("%le",Rd[$D1]);
			if(retval < 0){ R[$R3]=0; R[$R4]=1; }
			else{ R[$R3]=retval; R[$R4]=0; }

		}break;
		case 22:
		{
			retval = scanf("%s",(char *)&RAM[R[$R2]]);
			if((retval==EOF)||(retval==0)){ R[$R3]=0; }else{ R[$R3]=retval; }

		}break;
		case 23:
		{
			printf("Warning (INT 0, $R1=23) no wide character support!\n");
			R[$R3]=0;

		}break;
		case 24:
		{
			if(R[$R3]==1){ retval = scanf("%llu",&R[$R2]); }
			else{ retval = scanf("%lld",&R[$R2]); }
			if((retval==EOF)||(retval==0)){ R[$R3]=0; }else{ R[$R3]=retval; }

		}break;
		case 25:
		{
			retval = scanf("%le",&Rd[$D1]);
			if((retval==EOF)||(retval==0)){ R[$R3]=0; }else{ R[$R3]=retval; }

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 0 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handlefileIO*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleFileManagement                                             +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<bits/posix1_lim.h>

void handleFileManagement()
{
	int retval;

	switch((U1)R[$R1])
	{
		case 0:
		{
			retval = access(&RAM[R[$R2]],F_OK); /*exist*/
			if(retval==0){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 1:
		{
			int result;
			struct stat buff;
			result = stat(&RAM[R[$R2]],&buff);
			if(result!=0)
			{
				R[$R3]=2;
			}
			else
			{
				if(S_ISDIR(buff.st_mode))
				{
					R[$R3]=1;
				}
				else
				{
					R[$R3]=0;
				}
			}

		}break;
		case 2:
		{
			retval = access(&RAM[R[$R2]],R_OK); /*read*/
			if(retval==0){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 3:
		{
			retval = access(&RAM[R[$R2]],W_OK); /*write*/
			if(retval==0){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 4:
		{
			int result;
			struct stat buff;
			result = stat(&RAM[R[$R2]],&buff);
			if(result!=0)
			{
				R[$R3]=0;
				R[$R4]=1;
			}
			else
			{
				R[$R3]=buff.st_size;
				R[$R4]=0;
			}	

		}break;
		case 5:
		{
			int result;
			struct stat buff;
			result = stat(&RAM[R[$R2]],&buff);
			if(result!=0)
			{
				R[$R3]=0;
				R[$R4]=1;
			}
			else
			{
				R[$R3]=buff.st_mtime;
				R[$R4]=0;
			}	

		}break;
		case 6:
		{
			if(remove(&RAM[R[$R2]])==-1){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 7:
		{
			if(mkdir(&RAM[R[$R2]],0777)==-1){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 8:
		{
			if(rmdir(&RAM[R[$R2]])==-1){ R[$R3]=1; }
			else{ R[$R3]=0; }

		}break;
		case 9:
		{
			/*
			have to assume file is in cwd
				such that &RAM[R[$R2]] points to file name
				          &RAM[R[$R3]] has cwd
			*/

			char full[_POSIX_PATH_MAX];
			if(getcwd(full,_POSIX_PATH_MAX)==full)
			{
				int len;
				strcpy(&RAM[R[$R3]],full);
				strcpy(&RAM[R[$R3]],"/");
				strcpy(&RAM[R[$R3]],&RAM[R[$R2]]);
				len = strlen(&RAM[R[$R3]]);
				R[$R4]=0;
				R[$R5]=len+1; /*include null*/
			}
			else
			{
				R[$R4]=1;
				R[$R5]=0;
			}

		}break;
		case 10:
		{
			R[$R2]=_POSIX_PATH_MAX;

		}break;
		case 11:
		{
			/* execute ls -1 */
			FILE *pipe;
			char buffer[_POSIX_NAME_MAX*2*1024];
			
			strcpy(buffer,"ls -1 ");
			
			/*max posix filename is _POSIX_NAME_MAX chars in length*/

			if(strlen(&RAM[R[$R2]])>_POSIX_NAME_MAX)
			{
				R[$R5]=0;
				R[$R6]=1;
			}
			else
			{
				strcat(buffer,&RAM[R[$R2]]);
				pipe = popen(buffer,"r");
				if(pipe==NULL)
				{
					R[$R5]=0;
					R[$R6]=1;
				}
				else
				{
					R[$R5]=fread(buffer,1,_POSIX_NAME_MAX*2*1024,pipe);
					if(R[$R3]!=1)
					{
						strcpy(&RAM[R[$R4]],buffer);
					}
					R[$R6]=0;
					pclose(pipe);
				}
			}

		}break;
		case 12:
		{
			if(rename(&RAM[R[$R2]],&RAM[R[$R3]]))
			{
				R[$R4]=1;
			}
			else
			{
				R[$R4]=0;
			}

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 1 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleFileManagement*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleFileManagement                                             +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include<sys/wait.h>

void handleProcessManagement()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			pid_t child;
			child = fork();

			if(child==0)
			{
				exit(system(&RAM[R[$R2]]));			
			}
			else
			{
				R[$R4]=(U8)child;
				R[$R5]=0;
			}

		}break;
		case 1:
		{
			pid_t retval;
			int code;
			retval = waitpid((pid_t)R[$R2],&code,0);
			if(retval!=((pid_t)R[$R2]))
			{
				R[$R3]=0;
				R[$R4]=1;
			}
			else
			{
				if(WIFEXITED(code))
				{
					R[$R3]=WEXITSTATUS(code);
					R[$R4]=0;
				}
				else
				{
					R[$R3]=0;
					R[$R4]=1;	
				}
			}
		
		}break;
		case 2:
		{
			int retval;
			retval = system(&RAM[R[$R2]]);
			if(retval==-1)
			{ 
				R[$R3]=0;
				R[$R4]=1;
			}
			else
			{ 
				R[$R3]=retval;
				R[$R4]=0;
			}

		}break;
		case 3:
		{
			FILE *pipe;
			
			pipe = popen(&RAM[R[$R2]],"r");
			if(pipe==NULL)
			{
				R[$R5]=0;
				R[$R6]=1;
			}
			else
			{
				R[$R5]=fread(&RAM[R[$R3]],1,(size_t)R[$R4],pipe);
				RAM[R[$R3]+R[$R5]]='\0';
				R[$R6]=0;
				pclose(pipe);
			}

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 2 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleProcessManagement*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleTimeDateCall                                               +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handleTimeDateCall()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			R[$R2]= time(NULL);
			
		}break;
		case 1:
		{
			struct tm *tptr;
			time_t t;
			t=(time_t)R[$R2];
			tptr = gmtime(&t);
			R[$R3]=(*tptr).tm_sec;
			R[$R4]=(*tptr).tm_min;
			R[$R5]=(*tptr).tm_hour;
			R[$R6]=(*tptr).tm_mday;
			R[$R7]=(*tptr).tm_mon;
			R[$R8]=(*tptr).tm_year; 

		}break;
		case 2:
		{
			struct tm *tptr;
			time_t t;
			t=(time_t)R[$R2];
			tptr = localtime(&t);
			R[$R3]=(*tptr).tm_sec;
			R[$R4]=(*tptr).tm_min;
			R[$R5]=(*tptr).tm_hour;
			R[$R6]=(*tptr).tm_mday;
			R[$R7]=(*tptr).tm_mon;
			R[$R8]=(*tptr).tm_year;

		}break;
		case 3:
		{
			struct tm ts;
			time_t t;
			(ts).tm_sec= (int)R[$R2];
			(ts).tm_min= (int)R[$R3];
			(ts).tm_hour= (int)R[$R4];
			(ts).tm_mday= (int)R[$R5];
			(ts).tm_mon= (int)R[$R6];
			(ts).tm_year= (int)R[$R7];

			t = mktime(&ts);
			R[$R8]=t;

		}break;
		case 4:
		{
			char datestr[26];
			time_t t;
			t = (time_t)R[$R2];
			strcpy(datestr,ctime(&t));
			strcpy(&RAM[R[$R3]],datestr);
			RAM[R[$R3]+24]=0;

		}break;
		case 5:
		{
			R[$R4] = (long)difftime((time_t)R[$R3],(time_t)R[$R2]);

		}break;
		case 6:
		{
			R[$R2] = (U8)clock();

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 4 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleTimeDateCall*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleCommandLine                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handleCommandLine()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			R[$R2]= programArgs.nArgs;
			
		}break;
		case 1:
		{
			if(R[$R2]<programArgs.nArgs)
			{
				strncpy(&RAM[R[$R3]],programArgs.args[R[$R2]],256);
				RAM[R[$R3]+256]=0;
				R[$R4]=0;
				R[$R5]= strlen(&RAM[R[$R3]]);
				R[$R5]= R[$R5] + 1;
			}
			else
			{
				R[$R4]=1;
				R[$R5]=0;
			}

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 5 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleCommandLine*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleCommandLine                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handleMemoryStatus()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			R[$R2]=R[$BE]+1;
			R[$R3]=R[$HE]-R[$HS]+1;
			R[$R4]=R[$TOP]-R[$SS]+1;
			R[$R5]=R[$TOP]+1;

		}break;
		case 1:
		{
			/*
			parsing /proc/meminfo not a good approach
			in fact, I couldn't find a good approach
			*/
			printf("this call (INT 6, $R1=1) is not implemented on Linux\n");
			R[$R2]=0;
			R[$R3]=0;
			R[$R4]=0;
			R[$R5]=0;

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 6 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleMemoryStatus*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleAllocCall                                                  +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define FREE		0
#define RESERVED	1
char *statStr[] = {"FREE","RESERVED"};

/* Memory block header = 17 bytes = 8 previous + 8 next + status */

#define MEM_HEADER	17

#define prev(i)		*((U8 *)&memory[i])
#define next(i)		*((U8 *)&memory[i+8])
#define status(i)	memory[i+16]
#define size(i)		next(i)-i-MEM_HEADER

/* if going to split free block, need at least 8 bytes in new free part */

#define MIN_FREE_BYTES   8

U1 *memory;	/*alias for RAM */
U8 first;	/*stores address of first byte of heap*/
U8 last;	/*store address of last byte of heap + 1*/

/* prototypes------------------------------------------------------*/

void heapInit();
U8 alloc(U8 nbytes);
int currentNodeAlloc(U8 i,U8 nbytes);
void deAlloc(U8 address);
void printMemory();

/* definitions-----------------------------------------------------*/

void handleAllocCall()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			R[$R3] = alloc(R[$R2]);

		}break;
		case 1:
		{
			deAlloc(R[$R2]);

		}break;
		case 2:
		{
			printMemory();

		}break;
		case 3:
		{
			heapInit();

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 7 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleAllocCall*/

void heapInit()
{
	/* set up scale parameters*/
	memory=RAM;
	first=R[$HS];
	last=R[$HE]+1;
	
	/*initialize the entire heap*/
	prev(first)=0;
	next(first)=0;
	status(first)=FREE;

	return;

}/*end heapInit*/

U8 alloc(U8 nbytes)
{
	int ret;
	U8 i;

	i=first;

	if(status(i)==FREE)
	{
		ret = currentNodeAlloc(i,nbytes);
		if(ret==TRUE)
		{
			return(i+MEM_HEADER);
		}
	}

	while(next(i)!=0)
	{
		i=next(i);
		if(status(i)==FREE)
		{
			ret = currentNodeAlloc(i,nbytes);
			if(ret==TRUE)
			{
				return(i+MEM_HEADER);
			}
		}
	}

	return(0);

}/*end alloc*/

int currentNodeAlloc(U8 i,U8 nbytes)
{
	U8 size;

	/*handle case of current block being the last*/

	if(next(i)==0){ size = last-i-MEM_HEADER; }
	else{ size = size(i); }

	/*either split current block, use entire current block, or fail*/

	if(size >= nbytes + MEM_HEADER + MIN_FREE_BYTES)
	{
		U8 old_next;
		U8 old_block;
		U8 new_block;

		old_next = next(i);
		old_block = i;

		/*fix up original block*/

		next(i)=i+MEM_HEADER+nbytes;	
		new_block = next(i);
		status(i)=RESERVED;

		/*set up new free block*/

		i = next(i);					
		next(i)=old_next;
		prev(i)=old_block;
		status(i)=FREE;

		/*right nieghbor must point to new free block*/

		if(next(i)!=0)			
		{
			i = next(i);
			prev(i)=new_block;
		}
		
		return(TRUE);
	}
	else if(size >= nbytes)
	{
		status(i)=RESERVED;
		return(TRUE);
	}

	return(FALSE);

}/*end currentNodeAlloc*/

/*Note: disaster will strike if fed wrong address*/

void deAlloc(U8 address)
{
	U8 block;
	U8 lblock;
	U8 rblock;

	block = address-MEM_HEADER;
	lblock = prev(block);
	rblock = next(block);

	if((address>R[$HE])||(address<R[$HS]))
	{ 
		ERROR1_LVL2("deAlloc(): %I64u address out of bounds!\n",address); 
		return; 
	}

	/*
	4 cases: FFF->F, FFR->FR, RFF->RF, RFR 
	always want to merge free blocks 
	*/

	if((lblock!=0)&&(rblock!=0)&&(status(lblock)==FREE)&&(status(rblock)==FREE))
	{
		next(lblock)=next(rblock);
		status(lblock)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=lblock; }
	}
	else if((lblock!=0)&&(status(lblock)==FREE))
	{
		next(lblock)=next(block);
		status(lblock)=FREE;
		if(next(block)!=0){ prev(next(block))=lblock; }
	}
	else if((rblock!=0)&&(status(rblock)==FREE))
	{
		next(block)=next(rblock);
		status(block)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=block; }
	}
	else{ status(block)=FREE; }

	return;

}/*end deAlloc*/

void printMemory()
{
	U8 i;
	i=first;
	printf("--HEAP--\n");
	printf("[%llu,%llu,%llu,%s]\n",prev(i),i,next(i),statStr[status(i)]);
	while(next(i)!=0)
	{
		i=next(i);
		printf("[%llu,%llu,%llu,%s]\n",prev(i),i,next(i),statStr[status(i)]);
	}
	return;

}/*end printMemory*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleMathCall                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include<math.h>

void handleMathCall()
{
	switch((U1)R[$R1])
	{
		case 0:
		{
			/*
			no call to handle 64-bit values
			so we handle what we can ( 32-bit )
			*/
			printf("Warning: (INT 8, $R1=0) only handles 32-bit values on Linux\n");
			R[$R3]=atol(&RAM[R[$R2]]);

		}break;
		case 1:
		{
			Rd[$D1]=atof(&RAM[R[$R2]]);

		}break;
		case 2:
		{
			sprintf(&RAM[R[$R3]],"%lld",R[$R2]);

		}break;
		case 3:
		{
			sprintf(&RAM[R[$R3]],"%e",Rd[$D1]);

		}break;
		case 4:
		{
			printf("Warning: (INT 8, $R1=4) not handled on Linux\n");

		}break;
		case 5:
		{
			R[$R3] = labs((long)R[$R2]);

		}break;
		case 6:
		{
			Rd[$D2] = fabs(Rd[$D1]);

		}break;
		case 7:
		{
			Rd[$D2] = ceil(Rd[$D1]);

		}break;
		case 8:
		{
			Rd[$D2] = floor(Rd[$D1]);

		}break;
		case 9:
		{
			Rd[$D2] = exp(Rd[$D1]);

		}break;
		case 10:
		{
			Rd[$D2] = log(Rd[$D1]);

		}break;
		case 11:
		{
			Rd[$D2] = log10(Rd[$D1]);

		}break;
		case 12:
		{
			Rd[$D3] = pow(Rd[$D1],Rd[$D2]);

		}break;
		case 13:
		{
			Rd[$D2] = sqrt(Rd[$D1]);

		}break;
		case 14:
		{
			Rd[$D2] = cos(Rd[$D1]);

		}break;
		case 15:
		{
			Rd[$D2] = sin(Rd[$D1]);

		}break;
		case 16:
		{
			Rd[$D2] = tan(Rd[$D1]);

		}break;
		case 17:
		{
			Rd[$D2] = acos(Rd[$D1]);

		}break;
		case 18:
		{
			Rd[$D2] = asin(Rd[$D1]);

		}break;
		case 19:
		{
			Rd[$D2] = atan(Rd[$D1]);

		}break;
		case 20:
		{
			Rd[$D2] = cosh(Rd[$D1]);

		}break;
		case 21:
		{
			Rd[$D2] = sinh(Rd[$D1]);

		}break;
		case 22:
		{
			Rd[$D2] = tanh(Rd[$D1]);

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 8 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleMathCall*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleNativeCall                                                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handleNativeCall()
{
	/*
		Linux shared libraries must be specified on the
		command line when an application is compiled and linked.
		You can't just specify a file name dynamically like you
		do on windows
	*/

	switch((U1)R[$R1])
	{
		case 0:
		{
			printf("Warning: (INT 9, $R1=0) not handled on Linux\n");

		}break;
		case 1:
		{
			printf("Warning: (INT 9, $R1=1) not handled on Linux\n");

		}break;
		case 2:
		{
			printf("Warning: (INT 9, $R1=2) not handled on Linux\n");

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 9 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleNativeCall*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  handleIPC                                                        +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include<sys/socket.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<errno.h>
#include<netinet/in.h>

char fname[_POSIX_PATH_MAX];

void handleIPC()
{
	switch((U1)R[$R1])
	{
		/*use file locking instead of semaphores (RS p.463)*/

		case 0:
		{
			int fd;
			fd = open(&RAM[R[$R2]],O_WRONLY|O_CREAT|O_EXCL,0644);
			while(fd<0 && errno==EEXIST)
			{
				fd = open(&RAM[R[$R2]],O_WRONLY|O_CREAT|O_EXCL,0644);	
			}
			if(fd<0){ R[$R3]=0; R[$R4]=1; }
			else
			{
				R[$R3]=fd; R[$R4]=1;
				strcpy(fname,&RAM[R[$R2]]);
			}
		
		}break;
		case 1:
		{
			int retval;
			retval = close((int)R[$R2]);
			if(retval==-1)
			{
				R[$R3]=1;
			}
			else
			{
				retval = unlink(fname);
				if(retval==-1){ R[$R3]=1; }
				else{ R[$R3]=0; }
			}	

		}break;
		case 2:
		{
			struct sockaddr_in address;
			int client;
			int err;

			address.sin_family=AF_INET;
			address.sin_port = htons((unsigned short)R[$R3]);
			address.sin_addr.s_addr = inet_addr(&RAM[R[$R2]]);

			client = socket(AF_INET,SOCK_STREAM,0);
			if(client<0)
			{
				R[$R5]=1;
			}
			else
			{
				err = connect(client,
					       &address,
						   sizeof(struct sockaddr_in));
				if(err==-1)
				{
					/*printf("connection to %s failed\n",&RAM[R[$R2]]);*/
					R[$R5]=1;
				}
				else
				{
					/*printf("connection to %s suceeded\n",&RAM[R[$R2]]);*/
					R[$R5]=0;
					R[$R4]=(U8)client;
				}
			}

		}break;
		case 3:
		{
			if(close((int)R[$R2])==-1)
			{
				R[$R3]=1;
			}
			else
			{
				R[$R3]=0;
			}

		}break;
		case 4:
		{
			int nLeft;
			int index;
			char *buffer;

			nLeft=(int)R[$R4];
			index=0;
			R[$R5]=0;
			buffer = &RAM[R[$R3]];
			while(nLeft>0)
			{
				int ret;
				ret = send((int)R[$R2],
					       &(buffer[index]),
						   nLeft,
						   0);
				if(ret==-1)
				{ 
					R[$R5]=1;
					break;
				}
				nLeft = nLeft - ret;
				index = index + ret;
			}

		}break;
		case 5:
		{
			int nLeft;
			int index;
			char *buffer;

			nLeft=(int)R[$R4];
			index=0;
			R[$R5]=0;
			buffer = &RAM[R[$R3]];
			while(nLeft>0)
			{
				int ret;
				ret = recv((int)R[$R2],
					       &(buffer[index]),
						   nLeft,
						   0);
				if(ret==-1)
				{ 
					R[$R5]=1;
					break;
				}
				nLeft = nLeft - ret;
				index = index + ret;
			}

		}break;
		case 6:
		{
			if(gethostname(&RAM[R[$R2]],(int)R[$R3]))
			{
				R[$R4]=1;
			}
			else
			{
				R[$R4]=0;
			}

		}break;
		case 7:
		{
			struct hostent *hp;
			int i;

			hp = gethostbyname(&RAM[R[$R2]]);
			if(hp==NULL)
			{
				R[$R4]=1;
			}
			else
			{
				if((*hp).h_addr_list[0]!=0)
				{
					struct in_addr addr;
					memcpy(&addr,
						   (*hp).h_addr_list[0],
						   sizeof(struct in_addr));
					strcpy(&RAM[R[$R3]],inet_ntoa(addr));
				}
				for (i =1;(*hp).h_addr_list[i]!=0;++i)
				{
					struct in_addr addr;
					memcpy(&addr,
						   (*hp).h_addr_list[i],
						   sizeof(struct in_addr));
					strcat(&RAM[R[$R3]],":");
					strcat(&RAM[R[$R3]],inet_ntoa(addr));
				}

				R[$R4]=0;
			}

		}break;
		case 8:
		{
			struct hostent *hp;
			struct in_addr hostaddr;

			hostaddr.s_addr = inet_addr(&RAM[R[$R2]]);

			hp = gethostbyaddr((char *)&hostaddr,
					           sizeof(struct in_addr),
							   AF_INET);
			if(hp==NULL)
			{
				R[$R4]=1;
			}
			else
			{
				strcpy(&RAM[R[$R3]],(*hp).h_name);
				R[$R4]=0;
			}

		}break;
		case 9:
		{
			struct sockaddr_in address;
			int server;
			int err;

			address.sin_family=AF_INET;
			address.sin_port = htons((unsigned short)R[$R3]);
			address.sin_addr.s_addr = inet_addr(&RAM[R[$R2]]);

			server = socket(AF_INET,SOCK_STREAM,0);
			if(server==-1)
			{
				/*printf("socket for %s failed\n",&RAM[R[$R2]]);*/
				R[$R5]=1;
			}
			else
			{
				err = bind(server,
						   &address,
						   sizeof(address));
				if(err==-1)
				{
					/*printf("bind to %s failed\n",&RAM[R[$R2]]);*/
					R[$R5]=1;
				}
				else
				{
					/*printf("bound to %s \n",&RAM[R[$R2]]);*/
					err = listen(server,5);
					if(err==-1)
					{
						/*printf("listen to %s failed\n",&RAM[R[$R2]]);*/
						R[$R5]=1;
					}
					else
					{
						/*printf("listen to %s succeeded\n",&RAM[R[$R2]]);*/
						R[$R5]=0;
						R[$R4]=(U8)server;
					}
				}
			}

		}break;
		case 10:
		{
			int connection;
			struct sockaddr_in client;
			int length;
			connection = accept((int)R[$R2],
				                &client,
								&length);
			if(connection==-1)
			{
				/*printf("accept failed\n");*/
				R[$R5]=1;
			}
			else
			{
				R[$R5]=0;
				strcpy(&RAM[R[$R3]],inet_ntoa(client.sin_addr));
				R[$R4]=(U8)connection;
				/*printf("accept of %s was a sucess\n",&RAM[R[$R3]]);*/
			}

		}break;
		case 11:
		{
			if(close((int)R[$R2]))
			{
				R[$R3]=1;
			}
			else
			{
				R[$R3]=0;
			}

		}break;
		case 12:
		{
			if(close((int)R[$R2]))
			{
				R[$R3]=1;
			}
			else
			{
				R[$R3]=0;
			}

		}break;
		default:
		{ 
			ERROR1_LVL2("INT 10 %lu function not handled",(U1)R[$R1]);
		}
	}
	return;

}/*end handleIPC*/