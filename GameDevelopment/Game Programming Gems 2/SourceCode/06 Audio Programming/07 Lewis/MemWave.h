/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// MemWave.h: interface for the CMemWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMWAVE_H__6F703A4D_659E_4686_A688_31F48385E260__INCLUDED_)
#define AFX_MEMWAVE_H__6F703A4D_659E_4686_A688_31F48385E260__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wave.h"

class CMemWave : public CWave  
{
public:
	CMemWave(char* filename);
	virtual ~CMemWave();

};

#endif // !defined(AFX_MEMWAVE_H__6F703A4D_659E_4686_A688_31F48385E260__INCLUDED_)
