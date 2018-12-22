/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================*/
/* - CPU information						*/
/* - Memory allocation						*/
/* - String extension functions					*/
/*								*/
/* AUTHOR:	Gabor Nagy					*/
/* DATE:	2000-May-02 14:48:57				*/
/*								*/
/* For Game Programming Graphics Gems				*/
/*==============================================================*/
#define _EStrings_c

#ifdef _WIN32
#include <windows.h>
#else
#include <pwd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Define boolean flags
//
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#include "str.h"

/*------------------------------*/
/* Function prototypes		*/
/*------------------------------*/
char* EStr_GetPathName(char* LFullStr,char* LRet,int LMaxLen);

/*--------------------------------------*/
/* Memory management			*/
/*--------------------------------------*/

/*==============================================================*/
/* Allocate memory						*/
/*==============================================================*/
void* EMalloc(size_t LSize)
{
 void*	LM;

 LM=malloc(LSize);
#ifdef E_MEMDEBUG
 printf("Alloc(%d): %08x\n",LSize,LM);fflush(stdout);
#endif // E_MEMDEBUG
 return(LM);
}


/*==============================================================*/
/* Reallocate memory						*/
/*==============================================================*/
void* ERealloc(void* LBuffer,size_t LNewSize)
{
 void*	LM;

 LM=realloc(LBuffer,LNewSize);
#ifdef E_MEMDEBUG
 if(LM!=LBuffer)
 {
  printf("Free(R): %08x\n",LBuffer);fflush(stdout);
  printf("Alloc(R) (%d): %08x\n",LNewSize,LM);fflush(stdout);
 }
#endif // E_MEMDEBUG
 return(LM);
}


/*==============================================================*/
/* Free allocated memory					*/
/*==============================================================*/
void EFree(void* LMem)
{
 if(LMem==NULL) assert(0);
#ifdef E_MEMDEBUG
 printf("Free: %08x\n",LMem);fflush(stdout);
#endif // E_MEMDEBUG
 free(LMem);
}




/*======================================*/
/* Check two strings if they are equal	*/
/*======================================*/
int EStr_StringsEqual(register char* LString0,register char* LString1)
{
 register unsigned int	LC;
 register char		LCh;

 LC=0;
 do
 {
  if((LCh=LString0[LC])!=LString1[LC]) return(FALSE);
  else if(LCh=='\0') return(TRUE);
  LC++;
 } while(1);
}


/*======================================*/
/* Get path name from a full path	*/
/*======================================*/
char* EStr_GetPathName(char* LFullStr,char* LRet,int LMaxLen)
{
 register unsigned int	LC;
 register int		LLen,LLastSlashPos;

 LLastSlashPos=-1;
 for(LC=0;LFullStr[LC]!='\0';LC++)
 {
  if(LFullStr[LC]=='/') LLastSlashPos=LC;
 }
 LLen=LLastSlashPos+1;
 if(LRet==NULL) return(NULL);

 LC=0;
 if((LRet!=NULL)&&((LLen+1)<=LMaxLen))
 {
  for(;LC<LLen;LC++) LRet[LC]=LFullStr[LC];
 }
 LRet[LC]='\0';
 return(LRet);
}
