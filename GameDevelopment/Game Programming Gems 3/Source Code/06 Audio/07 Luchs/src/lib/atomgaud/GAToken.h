// GAToken - Interface
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

#ifndef _atomosGAToken_
#define _atomosGAToken_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAToken : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGAToken : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAToken };
	static const CClass classCGAToken;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAToken


	// CTOR / DTOR
	CGAToken();
	CGAToken(CHAR8 ch, UINT32 nPos = 0, UINT32 nLine= 0);
	CGAToken(CHAR8* psz, UINT32 nPos= 0, UINT32 nLine= 0);
	~CGAToken();

protected:

	UINT32	m_nPos;
	UINT32	m_nLine;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAToken_
// --------------------------------------------------------------------------------------------------------
