// GASquareOscillator - Interface
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

#ifndef _atomosGASquareOscillator_
#define _atomosGASquareOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASquareOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASquareOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASquareOscillator };
	static const CClass classCGASquareOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);
	

	// IGASquareOscillator


	// CTOR / DTOR
	CGASquareOscillator();
	~CGASquareOscillator();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASquareOscillator_
// --------------------------------------------------------------------------------------------------------
