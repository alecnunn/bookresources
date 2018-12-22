// GAInverter - Interface
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

#ifndef _atomosGAInverter_
#define _atomosGAInverter_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAInverter : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAInverter : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAInverter };
	static const CClass classCGAInverter;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAInverter


	// CTOR / DTOR
	CGAInverter();
	~CGAInverter();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAInverter_
// --------------------------------------------------------------------------------------------------------
