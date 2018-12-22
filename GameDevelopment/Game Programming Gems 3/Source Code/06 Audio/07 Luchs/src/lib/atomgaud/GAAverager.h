// GAAverager - Interface
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

#ifndef _atomosGAAverager_
#define _atomosGAAverager_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAAverager : public IGAProcessor
{
	virtual void		SetDuration(UINT32 nDuration)=0;
	virtual UINT32		GetDuration()=0;
	virtual void		SetDurationTime(FLOAT32 fDurationTime)=0;
	virtual FLOAT32		GetDurationTime()=0;



}; // interface


class ATOMGAUD_DLLCLASS CGAAverager : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAAverager };
	static const CClass classCGAAverager;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAAverager
	virtual void		SetDuration(UINT32 nDuration);
	virtual UINT32		GetDuration();
	virtual void		SetDurationTime(FLOAT32 fDurationTime);
	virtual FLOAT32		GetDurationTime();


	// CTOR / DTOR
	CGAAverager();
	~CGAAverager();

protected:

    FLOAT32		m_fSum;
    FLOAT32		m_fRecent;
	UINT32		m_nDuration; // in samples
	UINT32		m_nSpanIndex;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAAverager_
// --------------------------------------------------------------------------------------------------------
