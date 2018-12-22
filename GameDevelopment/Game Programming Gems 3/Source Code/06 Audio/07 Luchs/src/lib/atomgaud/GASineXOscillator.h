// GASineXOscillator - Interface
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

#ifndef _atomosGASineXOscillator_
#define _atomosGASineXOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASineXOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASineXOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASineXOscillator };
	static const CClass classCGASineXOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGASineXOscillator


	// CTOR / DTOR
	CGASineXOscillator();
	~CGASineXOscillator();

protected:


	CGARandom m_Random;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASineXOscillator_
// --------------------------------------------------------------------------------------------------------
