// GAPulseOscillator - Interface
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

#ifndef _atomosGAPulseOscillator_
#define _atomosGAPulseOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAPulseOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGAPulseOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GAPulseOscillator };
	static const CClass classCGAPulseOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAPulseOscillator


	// CTOR / DTOR
	CGAPulseOscillator();
	~CGAPulseOscillator();

protected:


	FLOAT32	m_fPulseWidth;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPulseOscillator_
// --------------------------------------------------------------------------------------------------------
