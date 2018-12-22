// GAEnvelope - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Baseclass for a simple scalable envelope
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

#ifndef _atomosGAEnvelope_
#define _atomosGAEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAEnvelope : public IGAProcessor
{
	virtual void SetSignalMode(UINT32 nMode)=0;
	virtual void SetDuration(FLOAT32 fDuration)=0;
	virtual void SetScale(FLOAT32 fScale)=0;



}; // interface


class ATOMGAUD_DLLCLASS CGAEnvelope : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAEnvelope };
	static const CClass classCGAEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAProcessor
	virtual void Trigger();
	virtual void Start();

	// IGAEnvelope
	virtual void SetSignalMode(UINT32 nMode);
	virtual void SetDuration(FLOAT32 fDuration);
	virtual void SetScale(FLOAT32 fScale);

	// CTOR / DTOR
	CGAEnvelope();
	~CGAEnvelope();

protected:

//	FLOAT32*	m_pfOutput;

	FLOAT32	m_fDuration;	// base duration in sec
	FLOAT32	m_fScale;
	FLOAT32	m_fDMAttenuator;		
	FLOAT32	m_fAmplitude;	// base amplitude
	FLOAT32	m_fAMAttenuator;

	UINT32	m_nSamplePos;		// current sample pos in envelope
//	FLOAT32	m_fSample;			// last sample value
	FLOAT32	m_fLastTrigger;

	UINT32	m_nSignalMode; // unipolar / bipolar
	FLOAT32	m_fPhase; // time as 0 to 1
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAEnvelope_
// --------------------------------------------------------------------------------------------------------
