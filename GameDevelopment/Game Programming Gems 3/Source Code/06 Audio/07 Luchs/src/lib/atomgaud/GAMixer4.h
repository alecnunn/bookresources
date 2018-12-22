// GAMixer4 - Interface
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

#ifndef _atomosGAMixer4_
#define _atomosGAMixer4_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMixer4 : public IGAMixer
{


}; // interface


class ATOMGAUD_DLLCLASS CGAMixer4 : public CGAMixer
{
public:

	// IObject
	enum { CID = CID_GAMixer4 };
	static const CClass classCGAMixer4;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);


	// CTOR / DTOR
	CGAMixer4();
	~CGAMixer4();

protected:

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMixer4_
// --------------------------------------------------------------------------------------------------------
