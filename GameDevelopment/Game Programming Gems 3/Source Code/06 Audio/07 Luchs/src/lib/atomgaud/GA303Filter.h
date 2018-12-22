// GA303Filter - Interface
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

#ifndef _atomosGA303Filter_
#define _atomosGA303Filter_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGA303Filter : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGA303Filter : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GA303Filter };
	static const CClass classCGA303Filter;
	virtual const IClass*		GetClass();

	
	virtual void	Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGAProcessor
	virtual void Update();

	// IGA303Filter


	// CTOR / DTOR
	CGA303Filter();
	~CGA303Filter();
	
protected:
	
//	FLOAT32*	m_pfOutput;
	FLOAT32 a;
	FLOAT32 b;
	FLOAT32 c;
	
	FLOAT32 d1;
	FLOAT32 d2;
	FLOAT32 d3;
	
	FLOAT32 c0;
	
	FLOAT m_fCutoff;
	FLOAT m_fResonance;
	
	FLOAT32	m_fFM; // cutoff modulation
	FLOAT32 m_fFMAmount;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGA303Filter_
// --------------------------------------------------------------------------------------------------------
