/* ============================================================================
 *  LType.h
 * ============================================================================

 *  Author:         (c) 2001-2003 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 28, 2001
 *  Module:         LSL

 *  Last changed:   $Date: 2003/02/06 18:53:03 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.11 $
*/


#ifndef __LType__
#define __LType__

/* COMPONENT ID */
#define LType_ID    0x800E


/* NUMERIC INTRINSIC TYPES */
typedef char                    i1;
typedef short int               i2;
typedef long int                i4;

typedef unsigned char           ui1;
typedef unsigned short int      ui2;
typedef unsigned long int       ui4;

typedef float                   f4;
typedef double                  f8;

typedef char                    Bool;


/* IDs OF INTRINSIC TYPES */
typedef enum {
    LType_UI1_ID,
    LType_UI2_ID,
    LType_UI4_ID,
    LType_I1_ID,
    LType_I2_ID,
    LType_I4_ID,
    LType_F4_ID,
    LType_F8_ID,
    LType_Bool_ID,
    LType_Ptr_ID,
    LType_Custom_ID
} LType_TTypeID;


/* GENERIC COMPARATOR TYPE */
typedef i4 (*LType_TCompar)(const void* inA, const void* inB);


/* DEFAULT COMPARATORS */
i4 LType_UI2Compar  (const void* inA, const void* inB);
i4 LType_UI4Compar  (const void* inA, const void* inB);
i4 LType_I1Compar   (const void* inA, const void* inB);
i4 LType_I2Compar   (const void* inA, const void* inB);
i4 LType_I4Compar   (const void* inA, const void* inB);
i4 LType_F4Compar   (const void* inA, const void* inB);
i4 LType_F8Compar   (const void* inA, const void* inB);
i4 LType_BoolCompar (const void* inA, const void* inB);
i4 LType_PtrCompar  (const void* inA, const void* inB);


/* TYPE RECORD */
typedef struct {
    LType_TTypeID mID;
    ui4           mSize;
} LType_TType;


/* TYPE COMPARATOR */
#define LType_EqualTypes(a,b) ((a).mID == (b).mID)


/* INTRINSIC TYPE RECORDS */
const extern LType_TType LType_UI1;
const extern LType_TType LType_UI2;
const extern LType_TType LType_UI4;
const extern LType_TType LType_I1;
const extern LType_TType LType_I2;
const extern LType_TType LType_I4;
const extern LType_TType LType_F4;
const extern LType_TType LType_F8;
const extern LType_TType LType_Bool;
const extern LType_TType LType_Ptr;


/* LIMITS */
#define LType_MAX_UI1           ((ui1)0xFF)
#define LType_MAX_UI2           ((ui2)0xFFFF)
#define LType_MAX_UI4           ((ui4)0xFFFFFFFF)
#define LType_MIN_I1            ((i1)0x80)
#define LType_MAX_I1            ((i1)0x7F)
#define LType_MIN_I2            ((i2)0x8000)
#define LType_MAX_I2            ((i2)0x7FFF)
#define LType_MIN_I4            ((i4)0x80000000)
#define LType_MAX_I4            ((i4)0x7FFFFFFF)

#define LType_MIN_F4            ((f4)1.175494e-38f)
#define LType_MAX_F4            ((f4)3.402823e+38f)
#define LType_MIN_F8            ((f8)2.225074e-308)
#define LType_MAX_F8            ((f8)1.797693e+308)


/* MACROS FOR TYPED MEMORY ACCESS */
#define  _i1_(a)                (*( i1*)(a))
#define _ui1_(a)                (*(ui1*)(a))
#define  _i2_(a)                (*( i2*)(a))
#define _ui2_(a)                (*(ui2*)(a))
#define  _i4_(a)                (*( i4*)(a))
#define _ui4_(a)                (*(ui4*)(a))
#define  _f4_(a)                (*( f4*)(a))
#define  _f8_(a)                (*( f8*)(a))

#define Mem_(type,addr)         ( (type*)(addr))
#define AtMem_(type,addr)       (*(type*)(addr))


/* NULL POINTER DEFINITION */

#ifndef NULL
#define NULL                    (0L)
#endif


/* Bool CONSTANTS */
#ifndef FALSE
#define FALSE                   (0)
#endif

#ifndef TRUE
#define TRUE                    (1)
#endif


/* MACROS FOR NUMERICAL ORDER SWAPPING */

#define SwapN2_(n) ((((n) & 0x00FF) << 8) | (((n) & 0xFF00) >> 8))
#define SwapN4_(n) ((((n) & 0x000000FF) << 24) |                    \
                    (((n) & 0x0000FF00) << 8 ) |                    \
                    (((n) & 0x00FF0000) >> 8 ) |                    \
                    (((n) & 0xFF000000) >> 24) )

/* UTILITY FUNCTIONS */
Bool LType_CheckConfig();

#endif


/* Copyright (C) 2001-2003 Camil Demetrescu

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
