/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Portable.cpp

#include "StdAfx.h"
#include "Portable.h"

s32 Portable_ConvertStringToS32(u8 *s)
{
	s32 nReturn;
	
	nReturn =((s32)s[0] << 24) + ((s32)s[1] << 16) + ((s32)s[2] << 8) + s[3];
	return nReturn;
}

u16 Portable_ConvertStringToU16(u8 *s)
{
	u16 nReturn;

	nReturn = (s[0] << 8) + s[1];
	return nReturn;
}

