// GASequencer16 - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	a simple 16 step sequencer
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

#ifndef _atomosGASequencer16_
#define _atomosGASequencer16_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASequencer16 : public IGAOscillator
{

	virtual void	SetSteps(UINT32 nSteps)=0;
	virtual void	SetSteps(CHAR8* pszValue)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGASequencer16 : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GASequencer16 };
	static const CClass classCGASequencer16;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGASequencer16
	virtual void	SetSteps(UINT32 nSteps);
	virtual void	SetSteps(CHAR8* pszValue);

	// CTOR / DTOR
	CGASequencer16();
	~CGASequencer16();

protected:

	UINT32	m_nSteps;
	UINT32	m_nCurrStep;
	UINT32	m_nCyclePos;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASequencer16_
// --------------------------------------------------------------------------------------------------------
