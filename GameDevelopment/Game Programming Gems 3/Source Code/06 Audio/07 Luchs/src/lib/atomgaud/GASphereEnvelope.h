// GASphereEnvelope - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	
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

#ifndef _atomosGASphereEnvelope_
#define _atomosGASphereEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASphereEnvelope : public IGAEnvelope
{



}; // interface


class ATOMGAUD_DLLCLASS CGASphereEnvelope : public CGAEnvelope  
{
public:

	// IObject
	enum { CID = CID_GASphereEnvelope };
	static const CClass classCGASphereEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAProcessor
	virtual void Trigger();

	// IGASphereEnvelope


	// CTOR / DTOR
	CGASphereEnvelope();
	~CGASphereEnvelope();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASphereEnvelope_
// --------------------------------------------------------------------------------------------------------
