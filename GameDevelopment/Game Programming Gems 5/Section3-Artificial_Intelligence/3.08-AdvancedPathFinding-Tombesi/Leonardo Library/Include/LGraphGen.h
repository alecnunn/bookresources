/* ============================================================================
 *  LGraph_Gen.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/31 15:54:44 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.5 $    
*/


#ifndef __LGraphGen__
#define __LGraphGen__

#include "LType.h"
#include "LGraph.h"
#include "LEdgeInfo.h"

/* COMPONENT ID */
#define LGraphGen_ID   0x802F

LGraph*		LGraphGen_RandomNM		(ui4 n, ui4 m, ui4 inSeed);
LGraph*		LGraphGen_RandomUNM		(ui4 n, ui4 m, ui4 inSeed);
LGraph*		LGraphGen_RandomNP		(ui4 n, f4 p);
LGraph*		LGraphGen_RandomUNP		(ui4 n, f4 p);
LGraph*		LGraphGen_RandomGrid	(ui4 n, Bool inDirected);
LEdgeInfo*	LGraphGen_RndEdgeInfoUI4(LGraph* inGraph, ui4 inA, ui4 inB);
void        LGraphGen_AddCycle      (LGraph* inGraph);

#endif

/* Copyright (C) 2003 Stefano Emiliozzi, Camil Demetrescu

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
