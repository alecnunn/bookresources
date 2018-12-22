/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfig_t.h: interface for the AVLayerConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIG_T_H__4014E8AE_C3FC_49A5_B863_56CEEA34E639__INCLUDED_)
#define AFX_AVLAYERCONFIG_T_H__4014E8AE_C3FC_49A5_B863_56CEEA34E639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AVLayer_t;

class AVLayerConfig_t  
{
public:
	AVLayerConfig_t() {};
	virtual ~AVLayerConfig_t() {};

	virtual void Configure( AVLayer_t *pAVLayer ) = 0;
};

#endif // !defined(AFX_AVLAYERCONFIG_T_H__4014E8AE_C3FC_49A5_B863_56CEEA34E639__INCLUDED_)
