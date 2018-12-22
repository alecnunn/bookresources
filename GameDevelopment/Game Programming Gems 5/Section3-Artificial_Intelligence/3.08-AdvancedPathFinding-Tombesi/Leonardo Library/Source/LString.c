/* ============================================================================
 *  LString.c
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.9 $
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "LString.h"
#include "LMemory.h"
#include "LConfig.h"


/* ----------------------------------------------------------------------------
 *  Clone
 * ----------------------------------------------------------------------------
*/
i1* LString_Clone(const i1* inStr){
    i1* theBuf;
    theBuf = (i1*)LMemory_Malloc(strlen(inStr)+1);
    strcpy(theBuf,inStr);
    return theBuf;
}


/* ----------------------------------------------------------------------------
 *  Copy
 * ----------------------------------------------------------------------------
*/
void LString_Copy(const i1* inStr, i1* outStr) {
    strcpy(outStr,inStr);
}


/* ----------------------------------------------------------------------------
 *  Append
 * ----------------------------------------------------------------------------
*/
void LString_Append(const i1* inStr, i1* thruStr) {
    strcat(thruStr,inStr);
}


/* ----------------------------------------------------------------------------
 *  Len
 * ----------------------------------------------------------------------------
*/
ui4 LString_Len(const i1* inStr) {
    return strlen(inStr);
}


/* ----------------------------------------------------------------------------
 *  Compare
 * ----------------------------------------------------------------------------
*/
i1 LString_Compare(const i1* inStr1, const i1* inStr2){
    return (i1)strcmp(inStr1,inStr2);
}


/* ---------------------------------------------------------------------------------
 *  Format
 * ---------------------------------------------------------------------------------
*/
void LString_Format(i1* outStr, const i1* inMsg, ...) {
    va_list theArgList;
    va_start(theArgList,inMsg);
    vsprintf(outStr, inMsg, theArgList);
    va_end(theArgList);
}


/* ---------------------------------------------------------------------------------
 *  Tokenizer
 * ---------------------------------------------------------------------------------
*/
i1* LString_Tokenizer(i1* inLine, const i1* inSeparators){
    return strtok(inLine,inSeparators);
}


/* ---------------------------------------------------------------------------------
 *  ToI4
 * ---------------------------------------------------------------------------------
*/
i4 LString_ToI4(const i1* inStr){
    i1* theDummy;
    return strtol(inStr,&theDummy,0);
}


/* ---------------------------------------------------------------------------------
 *  ToUI4
 * ---------------------------------------------------------------------------------
*/
ui4 LString_ToUI4(const i1* inStr){
    i1* theDummy;
    return strtoul(inStr,&theDummy,0);
}


/* ---------------------------------------------------------------------------------
 *  ToF8
 * ---------------------------------------------------------------------------------
*/
f8 LString_ToF8(const i1* inStr){
    i1* theDummy;
    return strtod(inStr,&theDummy);
}


/* ---------------------------------------------------------------------------------
 *  PToXP
 * ---------------------------------------------------------------------------------
*/
#if __LSL_NEWLINE__ == __LSL_LF__
void LString_PToXP(i1** thruText, ui4* thruSize){
}
#elif __LSL_NEWLINE__ == __LSL_CRLF__
void LString_PToXP(i1** thruText, ui4* thruSize){
    i1* theT = *thruText;
    ui4 i=0, j=0;
    while (i<*thruSize) {
        if (theT[i]=='\r') i++;
        theT[j++]=theT[i++];
    }
    *thruSize = j;
    *thruText = (i1*)LMemory_Realloc((void*)theT,j);
}
#else
#error "***LSL: Unsupported Newline Format"
#endif

/* ---------------------------------------------------------------------------------
 *  XPToP
 * ---------------------------------------------------------------------------------
*/
#if __LSL_NEWLINE__ == __LSL_LF__
void LString_XPToP(i1** thruText, ui4* thruSize){
}
#elif __LSL_NEWLINE__ == __LSL_CRLF__
void LString_XPToP(i1** thruText, ui4* thruSize){
    ui4 theOutSize = *thruSize + (*thruSize)/10;
    i1* theP  = *thruText;
    i1* theXP = (i1*)LMemory_Malloc(theOutSize);
    ui4 i=0, j=0;
    while(i<*thruSize){
        if (j+2>theOutSize) {
            theOutSize += theOutSize/10;
            theXP = (i1*)LMemory_Realloc(theXP,theOutSize);
        }
        if (theP[i]=='\n') theXP[j++]='\r';
        theXP[j++]=theP[i++];
    }
    theXP = (i1*)LMemory_Realloc(theXP,j);
    LMemory_Free(thruText);
    *thruText = theXP;
    *thruSize = j;
}
#else
#error "***LSL: Unsupported Newline Format"
#endif


/* Copyright (C) 2001 Camil Demetrescu

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
