
/******************************************************************************

Coral Tree osd/xosd.c
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include "osd.h"

#define XOSD_TEST_ACCOUNT "xosd"

#if OSD_COMPILER==OSD_KR
void child_function(data)
	void *data;
#else
void child_function(void *data)
#endif
{
	OSD_Mutex *mutex;
	mutex = (OSD_Mutex *)data;


	osdPrint(OSD_SOFTWARE,OSD_LOG,"child here");

	/* LOCK MUTEX */
	if(!osdMutexLock(mutex))
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not lock mutex");
	else
		osdPrint(OSD_SOFTWARE,OSD_LOG,"child obtained mutex");

	/* UNLOCK MUTEX */
	if(!osdMutexUnlock(mutex))
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not unlock mutex");
	else
		osdPrint(OSD_SOFTWARE,OSD_LOG,"child released mutex");

	/* DIE */
	osdThreadExit(0);
}

typedef struct _a_struct
{
	int a;
	char b;
	short c;
} a_struct;


#define UNAMELEN 1024

#if OSD_COMPILER==OSD_KR
main(argc, argv)
	int argc; 
	char *argv[];
#else
main(int argc, char *argv[])
#endif
{
	OSD_File	file;
	char 		buffer[100];
	char 		inbuffer[100];
	OSD_Output 	output;
	OSD_Mutex 	mutex;
	char 		string[100];
	char 		pathname[100];
	char		*arg[3];
	long		time;
	a_struct	*sptr;
	char		username[UNAMELEN];
	char 		*filemem;
	char 		*base_name;
	OSD_Time	osdtime;
	char 		*pt;

	pt = osdSPrintf("%f", (float)(1.234));
	fprintf(stderr,"%s\n", pt);
	osdFree(pt);

	fprintf(stderr,"This is a [%s]\n", osdPlatform(NULL));
	fprintf(stderr,"This is a [%s]\n", osdPlatform("h"));
	fprintf(stderr,"This is a [%s]\n", osdPlatform("o"));
	fprintf(stderr,"This is a [%s]\n", osdPlatform("r"));
	fprintf(stderr,"This is a [%s]\n", osdPlatform("hr"));
	fprintf(stderr,"This is a [%s]\n", osdPlatform("ro"));

	osdThreadInit();
#if 0
	if(!osdThreadInit())
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"thread initialization failed");
#endif

	/* COMPILED AS WHICH ENDIAN */
#if OSD_ENDIAN==OSD_LITTLE
	osdPrint(OSD_SOFTWARE,OSD_LOG,"OSD_LITTLE");
#else
	osdPrint(OSD_SOFTWARE,OSD_LOG,"OSD_BIG");
#endif

	/* SYSTEM DEP TEST */
	if(argc > 1)
	{
		osdPrint(OSD_SOFTWARE,OSD_LOG,"child process started");
		osdPrint(OSD_SOFTWARE,OSD_LOG,argv[1]);
		exit(0);
	}
#if 0

	/* TIME TEST */
	if(!osdTimeGet(&osdtime))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not get time");
	}
	else
	{
		sprintf(string,"Y%d M%d D%d H%d M%d S%d m%d",
			osdtime.year,
			osdtime.month,
			osdtime.day,
			osdtime.hour,
			osdtime.minute,
			osdtime.second,
			osdtime.millisecond);
		osdPrint(OSD_SOFTWARE, OSD_LOG, string);
	}

	time = osdTimer();

#endif
	arg[0] = argv[0];
	arg[1] = string;
	sprintf(arg[1],"child_arg");
	arg[2] = NULL;

	if(osdSpawnve(argv[0],arg,NULL) == -1)
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not spawn process");
	else
		osdPrint(OSD_SOFTWARE,OSD_LOG,"spawned process");

	if(osdSpawnveAsUser("xosd", NULL,argv[0],arg,NULL) == -1)
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not spawn process as user");
	else
		osdPrint(OSD_SOFTWARE,OSD_LOG,"spawned process as user");

	if(osdSpawnveAsUser("xosd", "dino6",argv[0],arg,NULL) == -1)
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not spawn process as user");
	else
		osdPrint(OSD_SOFTWARE,OSD_LOG,"spawned process as user");

#if 0
	osdPathnameConvert(pathname,"X:\\fat_path\\one\\two\\three\\");
	osdPrint(OSD_SOFTWARE,OSD_LOG,"original pathname");
	osdPrint(OSD_SOFTWARE,OSD_LOG,pathname);
	osdPathnameConvert(pathname,"/unix_path/one/two/three/");
	osdPrint(OSD_SOFTWARE,OSD_LOG,"converted pathname");
	osdPrint(OSD_SOFTWARE,OSD_LOG,pathname);

	base_name = (char *)osdBasename(pathname);
	osdPrint(OSD_SOFTWARE,OSD_LOG,"basename");
	osdPrint(OSD_SOFTWARE,OSD_LOG,base_name);
	osdPrint(OSD_SOFTWARE,OSD_LOG,"pathname after basename call");
	osdPrint(OSD_SOFTWARE,OSD_LOG,pathname);

	time = osdTimer() - time;
	sprintf(pathname,"time %d usecs", time);
	osdPrint(OSD_SOFTWARE,OSD_LOG,pathname);

	/* FILE TEST */

#if 1
		/* OPEN FILE */
		if(!osdFileOpen(&file, "XOSDTEST", OSD_FILE_ERASE | OSD_FILE_READ | OSD_FILE_WRITE))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file open failed");

		/* WRITE TO FILE */
		strcpy(inbuffer,"if you can read this, something is wrong");
		strcpy(buffer,"after this text is garbage");
		if(osdFileWrite(&file,buffer,strlen(buffer)+1) == -1)
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file write failed");

		/* READ FROM FILE */
		if(!osdFileSetPointer(&file,0,OSD_FILE_BEGIN))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not set file pointer");
		if(osdFileRead(&file,inbuffer,100) == -1)
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file read failed");
		inbuffer[26] = 0;
		osdPrint(OSD_SOFTWARE,OSD_LOG,inbuffer);

		/* CLOSE FILE */
		if(!osdFileClose(&file))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file close failed");
#endif

		/* OPEN FILE */
		if(!osdFileOpen(&file, "XOSDTEST", OSD_FILE_READ))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file open failed");

		strcpy(inbuffer,"if you can read this, something is wrong");
		if(osdFileRead(&file,inbuffer,100) == -1)
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file read failed");
		inbuffer[26] = 0;
		osdPrint(OSD_SOFTWARE,OSD_LOG,inbuffer);

		/* CLOSE FILE */
		if(!osdFileClose(&file))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file close failed");

		/* OPEN FILE */
		if(!osdFileOpen(&file, "XOSDTEST", OSD_FILE_READ | OSD_FILE_WRITE))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"file open failed");

		/* MAP FILE TO MEMORY */
		filemem = osdFileMap(&file, 100);
		if(!filemem)
		{
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"failed to map memory");
			exit(-1);
		}
		else
		{
			filemem[26] = 0;
			osdMemcpy((void *)&filemem[26],"added through mmap",18);
			osdPrint(OSD_SOFTWARE,OSD_LOG, filemem);
			osdFileUnmap(&file);
		}

		osdPrint(OSD_SOFTWARE,OSD_LOG,"file tests complete");
	
	/* OUTPUT TEST */

		/* OPEN OUTPUT */
		if(!osdOutputOpen(&output))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"output open failed");
		if(!osdOutputSetFilename(&output,"XOSDOUTT"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not set filename");

		/* OUTPUT TO STDOUT */
		if(!osdOutputSetFlags(&output, OSD_OUT_STDOUT))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"set flags failed");
		if(!osdOutputPrintLine(&output, "TEST OUTPUT", "stdout"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not print line");
		
	
		/* OUTPUT TO STDERR */
		if(!osdOutputSetFlags(&output, OSD_OUT_STDERR))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"set flags failed");
		if(!osdOutputPrintLine(&output, "TEST OUTPUT", "stderr"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not print line");

		/* OUTPUT TO FILE */
		if(!osdOutputSetFlags(&output, OSD_OUT_FILE))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"set flags failed");
		if(!osdOutputPrintLine(&output, "TEST OUTPUT", "file"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not print line");

		/* OUTPUT TO MESSAGEBOX */
		if(!osdOutputSetFlags(&output, OSD_OUT_MESSAGEBOX))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"set flags failed");
		if(!osdOutputPrintLine(&output, "TEST OUTPUT", "messagebox"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not print line");

		/* OUTPUT TO ALL */
		if(!osdOutputSetFlags(&output, 	
								OSD_OUT_MESSAGEBOX	|
								OSD_OUT_STDOUT		|
								OSD_OUT_STDERR		|
								OSD_OUT_FILE))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"set flags failed");
		if(!osdOutputPrintLine(&output, "TEST OUTPUT", "all"))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not print line");

		osdPrint(OSD_SOFTWARE,OSD_LOG,"output tests complete");

#endif
	/* MUTEX/THREAD TEST */
fprintf(stderr,"==================================\n");
		/* CREATE MUTEX */
		if(!osdMutexCreate(&mutex))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not create mutex");
		/* LOCK */
		if(!osdMutexLock(&mutex))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not lock mutex");
		else
			osdPrint(OSD_SOFTWARE,OSD_LOG,"parent obtained mutex");

		/* SPAWN THREAD */
		if(!osdThreadSpawn(child_function, (void *)&mutex))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not spawn thread");
		else
			osdPrint(OSD_SOFTWARE,OSD_LOG,"child spawned");

		/* PARENT SLEEP */
		osdSleep(3);

		osdPrint(OSD_SOFTWARE,OSD_LOG,"pre parent mutex release");
		/* PARENT UNLOCK MUTEX */
		if(!osdMutexUnlock(&mutex))
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not unlock mutex");
		else
			osdPrint(OSD_SOFTWARE,OSD_LOG,"parent released mutex");

		/* PARENT SLEEP */
		osdSleep(3);

fprintf(stderr,"==================================\n");
#if 0
	/* MEMORY ABSTRACTION */

		sptr = (a_struct *)osdMalloc(10 * sizeof(a_struct));
		if(!sptr)
		{
			osdPrint(OSD_SOFTWARE,OSD_ERROR,"could not malloc");
		}
		else
		{
			osdPrint(OSD_SOFTWARE,OSD_LOG,"malloc succeeded");
		}

		osdFree(sptr);
		osdPrint(OSD_SOFTWARE,OSD_LOG,"post osdFree");
#endif
	/* USER STUFF */
		osdUserGet(username,UNAMELEN);
		osdPrint(OSD_SOFTWARE,OSD_LOG,"username:");
		osdPrint(OSD_SOFTWARE,OSD_LOG,username);

		/* osdUserSet(XOSD_TEST_ACCOUNT,"dino6"); */
		osdUserSet("xosd","dino6");

		osdUserGet(username,UNAMELEN);
		osdPrint(OSD_SOFTWARE,OSD_LOG,"username:");
		osdPrint(OSD_SOFTWARE,OSD_LOG,username);

#if 0
	/* MILLISLEEP */

		osdPrint(OSD_SOFTWARE,OSD_LOG,"millisleep 10 secs");
		osdMilliSleep(10000);

	/* ERROR TEST */

		/* LOG */
		osdPrint(OSD_SYSTEM,OSD_LOG,"test log message");

		/* WARNING */
		osdPrint(OSD_SYSTEM,OSD_WARN,"test warning message");

		/* ERROR */
		osdPrint(OSD_SYSTEM,OSD_ERROR,"test error message");

		/* FATAL */
		osdPrint(OSD_SYSTEM,OSD_FATAL,"test fatal message");
#endif

}

