// GASawtoothOscBLIT - Interface
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

#ifndef _atomosGASawtoothOscBLIT_
#define _atomosGASawtoothOscBLIT_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:


interface IGASawtoothOscBLIT : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASawtoothOscBLIT : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASawtoothOscBLIT };
	static const CClass classCGASawtoothOscBLIT;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGASawtoothOscBlit


	// CTOR / DTOR
	CGASawtoothOscBLIT();
	~CGASawtoothOscBLIT();

protected:

	IGAFunctionTable* m_pSinTable;
	IGAFunctionTable* m_pNDivSinTable;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASawtoothOscBLIT_
// --------------------------------------------------------------------------------------------------------
