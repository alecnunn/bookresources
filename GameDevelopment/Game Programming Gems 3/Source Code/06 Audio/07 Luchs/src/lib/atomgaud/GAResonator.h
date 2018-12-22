// GAResonator - Interface
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

#ifndef _atomosGAResonator_
#define _atomosGAResonator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAResonator : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAResonator : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAResonator };
	static const CClass classCGAResonator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob);
	
	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAResonator
//	virtual void SetCutoff(FLOAT32 fCut);
	virtual void SetFrequency(FLOAT32 fFreqHz);
	virtual void SetFMAttenuation(FLOAT32 fAttenuation);

	
	virtual void Update();

	// CTOR / DTOR
	CGAResonator();
	~CGAResonator();

protected:


	FLOAT32		m_fFM;			// current center frequency modulation factor
	FLOAT32		m_fBaseFreq;
	FLOAT32		m_fCurrFreq;
	FLOAT32		m_fFMAttenuation;
	FLOAT32		m_fBandWidth;
	FLOAT32		m_fGainCorrection;

	FLOAT32 a0;
    FLOAT32 b1;
    FLOAT32 b2;
	
	FLOAT32 y1;
	FLOAT32 y2;

	FLOAT32	m_fAtt; // gain correction
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAResonator_
// --------------------------------------------------------------------------------------------------------
