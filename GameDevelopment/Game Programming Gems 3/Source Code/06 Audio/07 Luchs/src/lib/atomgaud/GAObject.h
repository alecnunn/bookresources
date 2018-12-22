// GAObject - Interface
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

#ifndef _atomosGAObject_
#define _atomosGAObject_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:


interface IGAObject  : public INamedObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGAObject  : public CNamedObject  
{
public:

	// IObject
	enum { CID = CID_GAObject };
	static const CClass classCGAObject;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();

	// IGAObject


	// CTOR / DTOR
	CGAObject();
	~CGAObject();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAObject_
// --------------------------------------------------------------------------------------------------------
