// GAClippedSineOscillator - Interface
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

#ifndef _atomosGAClippedSineOscillator_
#define _atomosGAClippedSineOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAClippedSineOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGAClippedSineOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GAClippedSineOscillator };
	static const CClass classCGAClippedSineOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAClippedSineOscillator


	// CTOR / DTOR
	CGAClippedSineOscillator();
	~CGAClippedSineOscillator();

protected:

	FLOAT		m_fClip;
	FLOAT		m_fDamping;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAClippedSineOscillator_
// --------------------------------------------------------------------------------------------------------
