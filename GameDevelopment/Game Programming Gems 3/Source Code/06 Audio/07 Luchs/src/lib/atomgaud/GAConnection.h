// GAConnection - Interface
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

#ifndef _atomosGAConnection_
#define _atomosGAConnection_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAConnection : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGAConnection : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAConnection };
	static const CClass classCGAConnection;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAConnection


	// CTOR / DTOR
	CGAConnection();
	~CGAConnection();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAConnection_
// --------------------------------------------------------------------------------------------------------
