// GASampleAndHold - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The S&H reads a value at the input and holds this for a period defined by
//				the holdtime
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGASampleAndHold_
#define _atomosGASampleAndHold_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASampleAndHold : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGASampleAndHold : public CGAProcessor
{
public:

	// IObject
	enum { CID = CID_GASampleAndHold };
	static const CClass classCGASampleAndHold;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGASampleAndHold

	// CTOR / DTOR
	CGASampleAndHold();
	~CGASampleAndHold();

protected:

//	FLOAT32*	m_pfInOut;
	FLOAT32		m_fCurrSample;
	UINT32		m_nHoldTime;
	UINT32		m_nCounter;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASampleAndHold_
// --------------------------------------------------------------------------------------------------------
