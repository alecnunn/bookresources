// GAWavetableOscillator - Interface
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

#ifndef _atomosGAWavetableOscillator_
#define _atomosGAWavetableOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAWavetableOscillator : public IGAOscillator
{

	virtual BOOL SetWavetable( FLOAT32* pSamples, UINT32 nFrames)=0;
	virtual BOOL SetWavetable( IGASignal* pSignal)=0;
	virtual void SetRepeat( UINT32 nCycles)=0;
	virtual void SetStartTime( UINT32 nSamples)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAWavetableOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GAWavetableOscillator };
	static const CClass classCGAWavetableOscillator;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);
	virtual void			Reset();

	// IGAObject
	virtual BOOL		SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAWavetableOscillator
	virtual BOOL SetWavetable( FLOAT32* pSamples, UINT32 nFrames);
	virtual BOOL SetWavetable( IGASignal* pSignal);
	virtual void SetRepeat( UINT32 nCycles);
	virtual void SetStartTime( UINT32 nSamples);

	// CTOR / DTOR
	CGAWavetableOscillator();
	~CGAWavetableOscillator();

protected:

	UINT32		m_nCycles;
	UINT32		m_nCurrCycle;

	 FLOAT32*	m_pfWavetable; 
	UINT32		m_nSampleCount;
	UINT32		m_nSamplePos; // current sample position in wavetable

	FLOAT32		m_fLastImpulse;

	UINT32		m_nStartTime; // in samples;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAWavetableOscillator_
// --------------------------------------------------------------------------------------------------------
