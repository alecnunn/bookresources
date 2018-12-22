// GAStream - Interface
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

#ifndef _atomosGAStream_
#define _atomosGAStream_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAStream : public IGAObject
{
	virtual void	SetChannelCount(UINT32 nChannels)=0;
	virtual BOOL	Open(UINT32 id)=0;
	virtual void	Close()=0;
	virtual void	Start()=0;
	virtual void	Stop()=0;
	virtual void	Flip()=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAStream : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAStream };
	static const CClass classCGAStream;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	

	// IGAStream
	virtual void	SetChannelCount(UINT32 nChannels);
	virtual BOOL	Open(UINT32 id);
	virtual void	Close();
	virtual void	Start();
	virtual void	Stop();
	virtual void	Flip();




//protected:

	PortAudioStream* m_pStream;
	UINT32		m_nDataType;
	INT32		m_nDevice; // @todo make class CGemsAudioDevice
	UINT32		m_nChannels;
	UINT32		m_nFlags;

	// CTOR / DTOR
	CGAStream();
	~CGAStream();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAStream_
// --------------------------------------------------------------------------------------------------------
