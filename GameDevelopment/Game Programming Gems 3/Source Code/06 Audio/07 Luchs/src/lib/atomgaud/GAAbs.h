// GAAbs - Interface
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

#ifndef _atomosGAAbs_
#define _atomosGAAbs_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAAbs : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAAbs : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAAbs };
	static const CClass classCGAAbs;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAAbs


	// CTOR / DTOR
	CGAAbs();
	~CGAAbs();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAAbs_
// --------------------------------------------------------------------------------------------------------
