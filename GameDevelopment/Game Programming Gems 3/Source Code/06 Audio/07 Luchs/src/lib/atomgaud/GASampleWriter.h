// GASampleWriter - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGASampleWriter_
#define _atomosGASampleWriter_

#include "GAFile.h"

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASampleWriter : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGASampleWriter : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GASampleWriter };
	static const CClass classCGASampleWriter;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAProcessor
	virtual void Start();
	virtual void Stop();

	// IGASampleWriter
void Append(FLOAT32* pSignal, UINT32 nCount);

	// CTOR / DTOR
	CGASampleWriter();
	~CGASampleWriter();

		bool		m_bWorking;

protected:

	CGAFile	m_File;
	FLOAT32		m_Samples[44100*2];
	UINT32		m_nWritePos;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASampleWriter_
// --------------------------------------------------------------------------------------------------------
