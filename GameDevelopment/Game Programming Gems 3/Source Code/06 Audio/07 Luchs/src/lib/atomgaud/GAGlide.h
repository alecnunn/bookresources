// GAGlide - Interface
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

#ifndef _atomosGAGlide_
#define _atomosGAGlide_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAGlide : public IGAProcessor
{

	virtual void	SetTime(FLOAT32 fTime)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAGlide : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAGlide };
	static const CClass classCGAGlide;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);
	// IGAGlide
	virtual void	SetTime(FLOAT32 fTime);

	// CTOR / DTOR
	CGAGlide();
	~CGAGlide();

protected:


	FLOAT32	m_nDuration; // glide duration in samples
	FLOAT32 m_fGlide;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAGlide_
// --------------------------------------------------------------------------------------------------------
