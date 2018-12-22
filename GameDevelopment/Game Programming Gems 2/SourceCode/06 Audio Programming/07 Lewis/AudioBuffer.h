/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// AudioBuffer.h: interface for the CAudioBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOBUFFER_H__DD3B1EDC_566C_44A9_8F2A_23F510BAEB7E__INCLUDED_)
#define AFX_AUDIOBUFFER_H__DD3B1EDC_566C_44A9_8F2A_23F510BAEB7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
CAudioBuffer
Encapsulates a memory area of arbitrary length.
This is used as the primary data-transfer object
between mixers, channels, waves, and wave pointers.

Note that CAudioBuffer does NOT own the memory that
it encapsulates. This is because the memory is
likely to be some portion of a hardware-accelerated
system buffer.
*/
class CAudioBuffer  
{
private:
	BYTE* data;
	unsigned int length;//length is in bytes. It's up to the user
				//to determine what format the data is in.
public:
	CAudioBuffer(BYTE* data, int len);

	//access functions
	BYTE* getData(){return data;}
	int getLength(){return length;}
};

#endif // !defined(AFX_AUDIOBUFFER_H__DD3B1EDC_566C_44A9_8F2A_23F510BAEB7E__INCLUDED_)
