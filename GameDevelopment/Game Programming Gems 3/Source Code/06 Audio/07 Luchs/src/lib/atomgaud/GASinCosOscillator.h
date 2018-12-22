// GASinCosOscillator - Interface
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

#ifndef _atomosGASinCosOscillator_
#define _atomosGASinCosOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASinCosOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASinCosOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASinCosOscillator };
	static const CClass classCGASinCosOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGASinCosOscillator


	// CTOR / DTOR
	CGASinCosOscillator();
	~CGASinCosOscillator();

protected:


	UINT32	m_nMode;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASinCosOscillator_
// --------------------------------------------------------------------------------------------------------
