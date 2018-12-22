// GADelay - Interface
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

#ifndef _atomosGADelay_
#define _atomosGADelay_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGADelay : public IGAProcessor
{
	virtual bool	SetDelayTime(FLOAT32 fTime)=0;
	virtual FLOAT32 GetDelayTime()=0;

	virtual bool	SetDelay(UINT32 nSamples)=0;
	virtual UINT32	GetDelay()=0;



}; // interface


class ATOMGAUD_DLLCLASS CGADelay : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GADelay };
	static const CClass classCGADelay;
	virtual const IClass*		GetClass();

	
	virtual void	Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGADelay
	virtual bool	SetDelayTime(FLOAT32 fTime);
	virtual FLOAT32 GetDelayTime();

	virtual bool	SetDelay(UINT32 nSamples);
	virtual UINT32	GetDelay();


	// CTOR / DTOR
	CGADelay();
	~CGADelay();

protected:

//	FLOAT32		m_fDelayTime;	// delay in seconds

//	UINT32		m_nDelay;	// delay in samples
	FLOAT32		m_fDelay;	// delay in samples
	FLOAT32		m_fDMAttenuation;

	INT32		m_nWritePos; // current write position in sample buffer
//	UINT32		m_nReadPos; // current read position in sample buffer
	UINT32		m_nDelayBufferSize; // max delay
	FLOAT32*	m_pfDelayBuffer;

	
	FLOAT32*	m_pfOutput;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGADelay_
// --------------------------------------------------------------------------------------------------------
