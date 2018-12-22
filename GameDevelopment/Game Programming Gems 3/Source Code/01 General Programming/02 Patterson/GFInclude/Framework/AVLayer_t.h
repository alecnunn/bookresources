/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayer_t.h: interface for the AVLayer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYER_T_H__6E103465_6DAF_4302_9364_D5971BF85AB6__INCLUDED_)
#define AFX_AVLAYER_T_H__6E103465_6DAF_4302_9364_D5971BF85AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AVLayer_t  
{
public:
	AVLayer_t() {};
	virtual ~AVLayer_t() {};

	virtual bool Connect( Systems_t *pSystems ) = 0;
	virtual void Disconnect() = 0;

	virtual void Update() = 0;
	virtual void RenderVisual() = 0;
	virtual void RenderAudio() = 0;
};

typedef std::list<AVLayer_t *> AVLayerPtrList_t;

#endif // !defined(AFX_AVLAYER_T_H__6E103465_6DAF_4302_9364_D5971BF85AB6__INCLUDED_)
