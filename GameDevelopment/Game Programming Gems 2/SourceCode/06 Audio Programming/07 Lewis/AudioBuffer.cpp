/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// AudioBuffer.cpp: implementation of the CAudioBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "AudioBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioBuffer::CAudioBuffer(BYTE* dat, int len):data(data),length(len)
{

}
