/* ============================================================================
 *  LRandSource.c
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.3 $
*/


#include "LRandSource.h"
#include "LMemory.h"
#include <stdlib.h>


/* MEMBER VARIABLES */
struct LRandSource {
    ui4 mSeed;
};


/* ----------------------------------------------------------------------------
 *  New
 * ----------------------------------------------------------------------------
*/
LRandSource* LRandSource_New(ui4 inSeed) {
    LRandSource theObject;
    theObject.mSeed = inSeed;
    srand(inSeed);
    return LMemory_NewObject(LRandSource, theObject);
}


/* ----------------------------------------------------------------------------
 *  Delete
 * ----------------------------------------------------------------------------
*/
void LRandSource_Delete(LRandSource** ThisA) {
    LMemory_DeleteObject(ThisA);
}


/* ----------------------------------------------------------------------------
 *  GetRand
 * ----------------------------------------------------------------------------
*/
ui4 LRandSource_GetRand(LRandSource* This, ui4 inMin, ui4 inMax) {
    return This->mSeed = inMin+(ui4)((inMax-inMin)*(rand()/(f8)RAND_MAX)+0.5);
}


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
