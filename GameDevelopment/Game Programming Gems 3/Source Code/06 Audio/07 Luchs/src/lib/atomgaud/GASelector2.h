// GASelector2 - Interface
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

#ifndef _atomosGASelector2_
#define _atomosGASelector2_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASelector2 : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGASelector2 : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GASelector2 };
	static const CClass classCGASelector2;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGASelector2


	// CTOR / DTOR
	CGASelector2();
	~CGASelector2();

protected:

	UINT32	m_nChannels;
	UINT32	m_nSelection;
	FLOAT32	m_fLastImpulse;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASelector2_
// --------------------------------------------------------------------------------------------------------
