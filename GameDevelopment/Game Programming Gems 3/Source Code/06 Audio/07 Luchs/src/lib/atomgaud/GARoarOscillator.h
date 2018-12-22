// GARoarOscillator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/atomos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of the Atomos Application Base
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGARoarOscillator_
#define _atomosGARoarOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGARoarOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGARoarOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GARoarOscillator };
	static const CClass classCGARoarOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGARoarOscillator


	// CTOR / DTOR
	CGARoarOscillator();
	~CGARoarOscillator();

protected:

	CGARandom m_Random;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGARoarOscillator_
// --------------------------------------------------------------------------------------------------------
