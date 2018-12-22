// GAOscillator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	GAOscillator is the base class for phase-controlled oscillators
//				Some derived classes are Sawtooth, Triangle, Sine and SquareOscillator
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

#ifndef _atomosGAOscillator_
#define _atomosGAOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAOscillator : public IGAGenerator
{

	virtual void SetFrequency(FLOAT32 fFreq)=0;
	virtual void SetFMAttenuation(FLOAT32 fAttenuation)=0;
	virtual void SetPhase(FLOAT32 fPhase)=0;
	virtual void SetPMAttenuation(FLOAT32 fPhase);


}; // interface


class ATOMGAUD_DLLCLASS CGAOscillator : public CGAGenerator  
{
public:

	// IObject
	enum { CID = CID_GAOscillator };
	static const CClass classCGAOscillator;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);
	virtual void	Reset();

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAProcessor
	BOOL			SetParameter(UINT32 idParam, FLOAT32 fValue);
	virtual void	Trigger();


	// IGAOscillator
	virtual void		SetFrequency(FLOAT32 fFreq);
	virtual void		SetFMAttenuation(FLOAT32 fAttenuation);
	virtual void		SetPhase(FLOAT32 fPhase);
	virtual void		SetPMAttenuation(FLOAT32 fPhase);


	// CTOR / DTOR
	CGAOscillator();
	~CGAOscillator();

protected:


	FLOAT32	m_fBaseFreq;		// base frequency 
	FLOAT32	m_fFMAttenuation;	// FM trimmer

	FLOAT32 m_fPhase;			// current phase from 0 to 1
	FLOAT32 m_fPhaseOffset;		// phase start point from 0 to 1
	FLOAT32	m_fPMAttenuation;	// PM trimmer

	UINT32	m_nPhase;			// integer phase, wrapping at INT_MAX	
	

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAOscillator_
// --------------------------------------------------------------------------------------------------------
