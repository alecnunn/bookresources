/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// Filter.h: interface for the CFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTER_H__3720045B_6EA9_47D0_BE1D_9FDEDEC7A626__INCLUDED_)
#define AFX_FILTER_H__3720045B_6EA9_47D0_BE1D_9FDEDEC7A626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAudioBuffer;

/*
CFilter
This class simply takes an input buffer
and transforms it in some way. The output 
and input consist of the same number of bytes. 

Some transformations produce more output data
than input; in these cases it is up to the
filter to buffer these extra samples and
mix them into the next buffer. To do otherwise
would be to unnecessarily complicate the mixer
algorithms.
*/
class CFilter  
{
public:
	CFilter();
	virtual ~CFilter();

	virtual void process(CAudioBuffer& buf)=0;
};

#endif // !defined(AFX_FILTER_H__3720045B_6EA9_47D0_BE1D_9FDEDEC7A626__INCLUDED_)
