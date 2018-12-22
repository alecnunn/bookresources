// GAExpCurve - Interface
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

#ifndef _atomosGAExpCurve_
#define _atomosGAExpCurve_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAExpCurve : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAExpCurve : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAExpCurve };
	static const CClass classCGAExpCurve;
	virtual const IClass*		GetClass();

	
	virtual void	Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGAProcessor
	virtual void Trigger();
	virtual void Update();

	// IGALine


	virtual void	SetStartValue(FLOAT32 fStartValue);
	virtual void	SetEndValue(FLOAT32 fEndValue);
	// IGAExpCurve


	// CTOR / DTOR
	CGAExpCurve();
	~CGAExpCurve();

protected:

	UINT32	m_nDuration;
	UINT32	m_nPos;		// current sample pos in duration

	FLOAT*	m_pfOutput; 

	FLOAT32	m_fLevel;	// current level
	FLOAT32	m_fStartValue;	// current level
	FLOAT32	m_fEndValue;	// current level
	FLOAT32 m_fIncr;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAExpCurve_
// --------------------------------------------------------------------------------------------------------
