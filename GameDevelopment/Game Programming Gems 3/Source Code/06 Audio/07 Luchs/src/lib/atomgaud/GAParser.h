// GAParser - Interface
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

#ifndef _atomosGAParser_
#define _atomosGAParser_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAParser : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGAParser : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAParser };
	static const CClass classCGAParser;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAParser


	// CTOR / DTOR
	CGAParser();
	~CGAParser();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAParser_
// --------------------------------------------------------------------------------------------------------
