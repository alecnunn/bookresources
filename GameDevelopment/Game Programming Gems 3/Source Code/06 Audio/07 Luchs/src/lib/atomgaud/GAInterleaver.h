// GAInterleaver - Interface
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

#ifndef _atomosGAInterleaver_
#define _atomosGAInterleaver_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAInterleaver : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAInterleaver : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAInterleaver };
	static const CClass classCGAInterleaver;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);

	// IGAInterleaver


	// CTOR / DTOR
	CGAInterleaver();
	~CGAInterleaver();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAInterleaver_
// --------------------------------------------------------------------------------------------------------
