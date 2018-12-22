// GASawtoothOscillator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
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

#ifndef _atomosGASawtoothOscillator_
#define _atomosGASawtoothOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASawtoothOscillator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASawtoothOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASawtoothOscillator };
	static const CClass classCGASawtoothOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);


	// IGAProcessor
	virtual void Trigger();
	virtual void Update();

	// IGASawtoothOscillator


	// CTOR / DTOR
	CGASawtoothOscillator();
	~CGASawtoothOscillator();

protected:

//	FLOAT32	m_fAccu;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASawtoothOscillator_
// --------------------------------------------------------------------------------------------------------
