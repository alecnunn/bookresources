// GARampEnvelope - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	simple ascending line envelope
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

#ifndef _atomosGARampEnvelope_
#define _atomosGARampEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGARampEnvelope : public IGAEnvelope
{



}; // interface


class ATOMGAUD_DLLCLASS CGARampEnvelope : public CGAEnvelope  
{
public:

	// IObject
	enum { CID = CID_GARampEnvelope };
	static const CClass classCGARampEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAProcessor
	virtual void Trigger();

	// IGARampEnvelope


	// CTOR / DTOR
	CGARampEnvelope();
	~CGARampEnvelope();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGARampEnvelope_
// --------------------------------------------------------------------------------------------------------
