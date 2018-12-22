// GACubic - Interface
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

#ifndef _atomosGACubic_
#define _atomosGACubic_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGACubic : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGACubic : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GACubic };
	static const CClass classCGACubic;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGACubic


	// CTOR / DTOR
	CGACubic();
	~CGACubic();

protected:

//	FLOAT32*	m_pfOutput;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGACubic_
// --------------------------------------------------------------------------------------------------------
