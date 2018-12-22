// GANoiseParticle - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	A noisegenerator with built-in envelope, mainly for short impulses
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

#ifndef _atomosGANoiseParticle_
#define _atomosGANoiseParticle_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGANoiseParticle : public IGANoiseGenerator
{



}; // interface


class ATOMGAUD_DLLCLASS CGANoiseParticle : public CGANoiseGenerator  
{
public:

	// IObject
	enum { CID = CID_GANoiseParticle };
	static const CClass classCGANoiseParticle;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGANoiseParticle


	// CTOR / DTOR
	CGANoiseParticle();
	~CGANoiseParticle();

protected:

	FLOAT32	m_fDuration; // duration in millisec
	UINT32	m_nEnvelope; // type of envelope

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGANoiseParticle_
// --------------------------------------------------------------------------------------------------------
