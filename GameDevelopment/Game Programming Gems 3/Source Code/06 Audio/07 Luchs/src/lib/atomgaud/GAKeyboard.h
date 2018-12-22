// GAKeyboard - Interface
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

#ifndef _atomosGAKeyboard_
#define _atomosGAKeyboard_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAKeyboard : public IGAObject
{
	virtual void	SetKey(UINT32 nKey, UINT32 nValue)=0;
	virtual UINT32	GetKey(UINT32 nKey)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAKeyboard : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAKeyboard };
	static const CClass classCGAKeyboard;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAKeyboard
	virtual void SetKey(UINT32 nKey, UINT32 nValue);
	virtual UINT32	GetKey(UINT32 nKey);


	// CTOR / DTOR
	CGAKeyboard();
	~CGAKeyboard();

protected:

    UINT32               m_nKey[256];



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAKeyboard_
// --------------------------------------------------------------------------------------------------------
