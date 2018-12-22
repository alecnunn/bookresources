// GAMixer6 - Interface
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

#ifndef _atomosGAMixer6_
#define _atomosGAMixer6_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMixer6 : public IGAMixer
{



}; // interface


class ATOMGAUD_DLLCLASS CGAMixer6 : public CGAMixer  
{
public:

	// IObject
	enum { CID = CID_GAMixer6 };
	static const CClass classCGAMixer6;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);


	// CTOR / DTOR
	CGAMixer6();
	~CGAMixer6();

protected:


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMixer6_
// --------------------------------------------------------------------------------------------------------
