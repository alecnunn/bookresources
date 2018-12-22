// GASineOscillator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
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

#ifndef _atomosGASineOscillator_
#define _atomosGASineOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASineOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASineOscillator : public CGAOscillator
{
public:

	// IObject
	enum { CID = CID_GASineOscillator };
	static const CClass classCGASineOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);


	// IGASineOscillator


	// CTOR / DTOR
	CGASineOscillator();
	~CGASineOscillator();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASineOscillator_
// --------------------------------------------------------------------------------------------------------
