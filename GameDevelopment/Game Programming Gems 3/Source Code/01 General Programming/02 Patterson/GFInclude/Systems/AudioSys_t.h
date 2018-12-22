/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AudioSys_t.h: interface for the AudioSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOSYS_T_H__5E246F98_32C1_47B3_95CA_900BC91D5E03__INCLUDED_)
#define AFX_AUDIOSYS_T_H__5E246F98_32C1_47B3_95CA_900BC91D5E03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class AudioSys_t  
{
public:
	AudioSys_t() {};
	virtual ~AudioSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual bool BeginRender() = 0;
	virtual void EndRender() = 0;
};

#endif // !defined(AFX_AUDIOSYS_T_H__5E246F98_32C1_47B3_95CA_900BC91D5E03__INCLUDED_)
