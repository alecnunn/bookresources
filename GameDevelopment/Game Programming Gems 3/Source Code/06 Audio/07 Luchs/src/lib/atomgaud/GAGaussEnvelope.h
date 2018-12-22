// GAGaussEnvelope - Interface
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

#ifndef _atomosGAGaussEnvelope_
#define _atomosGAGaussEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAGaussEnvelope : public IGAEnvelope
{



}; // interface


class ATOMGAUD_DLLCLASS CGAGaussEnvelope : public CGAEnvelope
{
public:

	// IObject
	enum { CID = CID_GAGaussEnvelope };
	static const CClass classCGAGaussEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAGaussEnvelope


	// CTOR / DTOR
	CGAGaussEnvelope();
	~CGAGaussEnvelope();

protected:

	FLOAT32	m_fSlope;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAGaussEnvelope_
// --------------------------------------------------------------------------------------------------------
 