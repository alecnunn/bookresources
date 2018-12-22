/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerCommand_t.h: interface for the AVLayerCommand_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCOMMAND_T_H__2C2DDBE5_31EA_4C7C_9E39_4CD6148C3932__INCLUDED_)
#define AFX_AVLAYERCOMMAND_T_H__2C2DDBE5_31EA_4C7C_9E39_4CD6148C3932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AVLayer_t;

class AVLayerCommand_t  
{
public:
	enum AVLayerCommandID_t {
		INVALID,
		PUSH_BACK,
		PUSH_FRONT,
		REMOVE,
		REMOVE_ALL,
	};

	AVLayerCommand_t( AVLayerCommandID_t nID, AVLayer_t *pAVLayer )
	{
		m_nID = nID;
		m_pAVLayer = pAVLayer;
	}
	virtual ~AVLayerCommand_t() {}

	AVLayerCommandID_t m_nID;
	AVLayer_t *m_pAVLayer;
};

typedef std::vector<AVLayerCommand_t> AVLayerCommandVector_t;

#endif // !defined(AFX_AVLAYERCOMMAND_T_H__2C2DDBE5_31EA_4C7C_9E39_4CD6148C3932__INCLUDED_)
