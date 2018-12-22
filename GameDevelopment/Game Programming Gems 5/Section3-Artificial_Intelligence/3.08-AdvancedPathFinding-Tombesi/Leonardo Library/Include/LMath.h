/* ============================================================================
 *  LMath.h
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 28, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.5 $
*/


#ifndef __LMath__
#define __LMath__

#include "LConfig.h"
#include "LType.h"

/* COMPONENT ID */
#define LMath_ID    0x800D

#include <math.h>

#define LMath_Cos(c)            cos(c)
#define LMath_Sin(c)            sin(c)
#define LMath_Tan(c)            tan(c)
#define LMath_ASin(c)           asin(c)
#define LMath_ACos(c)           acos(c)
#define LMath_ATan(c)           atan(c)
#define LMath_ATan2(c,d)        atan2(c,d)
#define LMath_Cosh(c)           cosh(c)
#define LMath_Sinh(c)           sinh(c)
#define LMath_Tanh(c)           tanh(c)
#define LMath_Exp(c)            exp(c)
#define LMath_Log(c)            log(c)
#define LMath_Log10(c)          log10(c)
#define LMath_FAbs(c)           fabs(c)
#define LMath_Sqrt(c)           sqrt(c)
#define LMath_Pow(c,d)          pow(c,d)
#define LMath_Ceil(c)           ceil(c)
#define LMath_Floor(c)          floor(c)
#define LMath_FRExp(c,d)        _LMath_FRExp(c,d)
#define LMath_LDExp(c,d)        ldexp(c,d)
#define LMath_FMod(c,d)         fmod(c,d)
#define LMath_Modf(c,d)         _LMath_Modf(c,d)
#define LMath_HyperCeil(c)      (1<<((ui4)LMath_Ceil(LMath_Log((f8)(c))/LMath_Log(2))))

f8      _LMath_FRExp(f8 c, i4* d);
f8      _LMath_Modf (f8 c, f8* d);

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
