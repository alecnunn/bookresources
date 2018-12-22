// GATriangleOscillator - Interface
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

#ifndef _atomosGATriangleOscillator_
#define _atomosGATriangleOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGATriangleOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGATriangleOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GATriangleOscillator };
	static const CClass classCGATriangleOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGATriangleOscillator


	// CTOR / DTOR
	CGATriangleOscillator();
	~CGATriangleOscillator();

protected:


//	FLOAT32	m_fAccu;
	FLOAT32	m_fCenter;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGATriangleOscillator_
// --------------------------------------------------------------------------------------------------------
