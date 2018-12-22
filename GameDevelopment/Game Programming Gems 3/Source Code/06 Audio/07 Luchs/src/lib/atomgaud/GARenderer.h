// GARenderer - Interface
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

#ifndef _atomosGARenderer_
#define _atomosGARenderer_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGARenderer : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGARenderer : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GARenderer };
	static const CClass classCGARenderer;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGARenderer


	// CTOR / DTOR
	CGARenderer();
	~CGARenderer();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGARenderer_
// --------------------------------------------------------------------------------------------------------
