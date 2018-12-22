/* ============================================================================
 *  LHash.h
 * ============================================================================

 *  Author:         (C) 2002 Stefano Emiliozzi
 *  License:        See the end of this file for license information
 *  Created:        December 28, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/30 11:21:14 $
 *  Changed by:     $Author: stefano $	 
 *  Revision:       $Revision: 1.4 $	
*/


#ifndef __LHash__
#define __LHash__

#include "LType.h"
#include "LDebug.h"

/* COMPONENT ID */
#define LHash_ID   0x8028

typedef struct LHash LHash;

struct LHash 
{
    struct TSlot*	mData;
    ui4				mDataSize;
    ui4				mItemsCount;    /* number of items                            */
	ui4				mEntriesCount;  /* number of entries                          */
	ui1				mHashLength;    /* lenght of the hashing value                */
	ui4				mCollisionKeys; /* number of keys with at least one collision */
	ui4				mRandomOdd;		/* random odd 32bit integer used for hashing  */
};

enum 
{
	LHash_OBJECT_NULL_POINTER = LHash_ID<<16,
};


LHash*     LHash_New					();
void       LHash_Delete					(LHash** ThisA);
void       LHash_InsertItem				(LHash* This, ui4 inItem, ui4 inKey);
void       LHash_RemoveItem				(LHash* This, ui4 inKey);
Bool       LHash_IsInTable				(LHash* This, ui4 inKey);
void       LHash_RemoveAllItems			(LHash* This);
ui4		   LHash_GetItemByKey			(LHash* This, ui4 inKey);
ui4        LHash_GetUsedMem				(LHash* This);
ui4        LHash_GetItemsCount			(LHash* This);
ui4        LHash_GetCollisionKeysCount  (LHash* This);

#ifdef __LSL_DEBUG__
void        LHash_Dump             (LHash* This);
#endif

#endif

/* Copyright (C) 2002 Stefano Emiliozzi

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
