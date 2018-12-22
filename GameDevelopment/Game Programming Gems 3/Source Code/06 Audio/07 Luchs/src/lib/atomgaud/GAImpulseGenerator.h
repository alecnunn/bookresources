// GAImpulseGenerator - Interface
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

#ifndef _atomosGAImpulseGenerator_
#define _atomosGAImpulseGenerator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAImpulseGenerator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGAImpulseGenerator : public CGAOscillator
{
public:

	// IObject
	enum { CID = CID_GAImpulseGenerator };
	static const CClass classCGAImpulseGenerator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAImpulseGenerator


	// CTOR / DTOR
	CGAImpulseGenerator();
	~CGAImpulseGenerator();

protected:


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAImpulseGenerator_
// --------------------------------------------------------------------------------------------------------
 