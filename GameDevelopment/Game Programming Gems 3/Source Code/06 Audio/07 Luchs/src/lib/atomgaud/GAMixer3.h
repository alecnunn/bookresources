// GAMixer3 - Interface
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

#ifndef _atomosGAMixer3_
#define _atomosGAMixer3_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMixer3 : public IGAMixer
{


}; // interface


class ATOMGAUD_DLLCLASS CGAMixer3 : public CGAMixer
{
public:

	// IObject
	enum { CID = CID_GAMixer3 };
	static const CClass classCGAMixer3;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);


	// CTOR / DTOR
	CGAMixer3();
	~CGAMixer3();

protected:



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMixer3_
// --------------------------------------------------------------------------------------------------------
 