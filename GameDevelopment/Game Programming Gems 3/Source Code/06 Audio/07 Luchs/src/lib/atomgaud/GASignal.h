// GASignal - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Wrapper for data (currently floats) prepared for additional information
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

#ifndef _atomosGASignal_
#define _atomosGASignal_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASignal : public IGAObject
{

	virtual void		SetCount(UINT32 nCount)=0;
	virtual UINT32		GetCount()=0;
	virtual FLOAT32*	GetData()=0;
	virtual void		Read(FLOAT32* pSamples, UINT32 nCount)=0;
	virtual UINT32		ReadFile( CHAR8* pszFileName)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGASignal : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GASignal };
	static const CClass classCGASignal;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGASignal
	virtual void		SetCount(UINT32 nCount);
	virtual UINT32		GetCount();
	virtual FLOAT32*	GetData();
	virtual UINT32		Read(FLOAT32* pSamples, UINT32 nCount);
	virtual UINT32		ReadFile( CHAR8* pszFileName);


	// CTOR / DTOR
	CGASignal(UINT32 nCount=0);
	~CGASignal();

protected:

	FLOAT32*	m_pfData;
	UINT32		m_nCount; // size as count of elements eg floats

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASignal_
// --------------------------------------------------------------------------------------------------------
 