// GASignalFormat - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	format setting for a GASignal
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

#ifndef _atomosGASignalFormat_
#define _atomosGASignalFormat_


namespace atomos
{
// forward declarations:



interface IGASignalFormat : public IGAObject
{
	virtual void	SetSampleRate(FLOAT32 fSampleRate)=0;
	virtual FLOAT32	GetSampleRate()=0;
	virtual void	SetFrameCount(UINT32 nFrameCount)=0;
	virtual UINT32	GetFrameCount()=0;
	virtual void	SetChannelCount(UINT32 nChannelCount)=0;
	virtual UINT32	GetChannelCount()=0;

}; // interface



class ATOMGAUD_DLLCLASS CGASignalFormat : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GASignalFormat };
	static const CClass classCGASignalFormat;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGASignal
	virtual void	SetSampleRate(FLOAT32 fSampleRate);
	virtual FLOAT32	GetSampleRate();
	virtual void	SetFrameCount(UINT32 nFrameCount);
	virtual UINT32	GetFrameCount();
	virtual void	SetChannelCount(UINT32 nChannelCount);
	virtual UINT32	GetChannelCount();



	// CTOR / DTOR
	CGASignalFormat();
	~CGASignalFormat();

protected:


	FLOAT32	m_fSampleRate;
	UINT32	m_nFrameCount;
	UINT32	m_nChannelCount;


};


} // namespace atomos

#endif // _atomosGASignalFormat_
// --------------------------------------------------------------------------------------------------------
