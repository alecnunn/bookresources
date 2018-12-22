// GAGain - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The gain processor generates its sample output by multiplying the sample input by
//				the value at AM input.
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

#ifndef _atomosGAGain_
#define _atomosGAGain_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAGain : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAGain : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAGain };
	static const CClass classCGAGain;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);
	

	// IGAGain

	// CTOR / DTOR
	CGAGain();
	~CGAGain();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAGain_
// --------------------------------------------------------------------------------------------------------
