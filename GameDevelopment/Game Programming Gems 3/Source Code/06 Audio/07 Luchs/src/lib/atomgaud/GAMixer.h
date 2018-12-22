// GAMixer4 - Interface
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

#ifndef _atomosGAMixer_
#define _atomosGAMixer_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMixer : public IGAProcessor
{

	virtual BOOL SetInputGain(UINT32 nChannel, FLOAT32 fGain)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAMixer : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAMixer };
	static const CClass classCGAMixer;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAMixer4
	virtual BOOL SetInputGain(UINT32 nChannel, FLOAT32 fGain);


	// CTOR / DTOR
	CGAMixer();
	~CGAMixer();

protected:


	FLOAT32		m_fAtt[8];
	UINT32		m_nChannels; // used channels, 8 MAX
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMixer_
// --------------------------------------------------------------------------------------------------------
