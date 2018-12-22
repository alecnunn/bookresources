/* ============================================================================
 *  LType.c
 * ============================================================================

 *  Author:         (c) 2001-2003 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 9, 2003
 *  Module:         LSL

 *  Last changed:   $Date: 2003/02/06 18:53:03 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.3 $
*/

#include "LType.h"

/* DEFAULT COMPARATORS */
i4 LType_UI1Compar  (const void* inA, const void* inB) { return _ui1_(inA) <= _ui1_(inB); }
i4 LType_UI2Compar  (const void* inA, const void* inB) { return _ui2_(inA) <= _ui2_(inB); }
i4 LType_UI4Compar  (const void* inA, const void* inB) { return _ui4_(inA) <= _ui4_(inB); }
i4 LType_I1Compar   (const void* inA, const void* inB) { return  _i1_(inA) <=  _i1_(inB); }
i4 LType_I2Compar   (const void* inA, const void* inB) { return  _i2_(inA) <=  _i2_(inB); }
i4 LType_I4Compar   (const void* inA, const void* inB) { return  _i4_(inA) <=  _i4_(inB); }
i4 LType_F4Compar   (const void* inA, const void* inB) { return  _f4_(inA) <=  _f4_(inB); }
i4 LType_F8Compar   (const void* inA, const void* inB) { return  _f8_(inA) <=  _f8_(inB); }
i4 LType_BoolCompar (const void* inA, const void* inB) { return  _i1_(inA) <=  _i1_(inB); }
i4 LType_PtrCompar  (const void* inA, const void* inB) { return _ui4_(inA) <= _ui4_(inB); }

/* INTRINSIC TYPES RECORDS */
const LType_TType LType_UI1  = { LType_UI1_ID,  1  };
const LType_TType LType_UI2  = { LType_UI2_ID,  2  }; 
const LType_TType LType_UI4  = { LType_UI4_ID,  4  };
const LType_TType LType_I1   = { LType_I1_ID,   1  }; 
const LType_TType LType_I2   = { LType_I2_ID,   2  }; 
const LType_TType LType_I4   = { LType_I4_ID,   4  }; 
const LType_TType LType_F4   = { LType_F4_ID,   4  };
const LType_TType LType_F8   = { LType_F8_ID,   8  };
const LType_TType LType_Bool = { LType_Bool_ID, 1  };
const LType_TType LType_Ptr  = { LType_Ptr_ID,  4  };


/* ---------------------------------------------------------------------------------
 *  CheckConfig
 *  ---------------------------------------------------------------------------------
 *  function that checks if the compiler is compatible with LL assumptions  */
Bool LType_CheckConfig(){
    return sizeof(i1 )==1 &&
           sizeof(ui1)==1 &&
           sizeof(i2 )==2 &&
           sizeof(ui2)==2 &&
           sizeof(i4 )==4 &&
           sizeof(ui4)==4 &&
           sizeof(f4 )==4 &&
           sizeof(f8 )==8 &&
           sizeof(void*)==4;
}


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
