// GAClipper - Interface
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

#ifndef _atomosGAClipper_
#define _atomosGAClipper_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAClipper : public IGAProcessor
{

	virtual void		SetClipLevel(FLOAT32 fLevel)=0;
	virtual FLOAT32		GetClipLevel()=0;
	virtual void		SetCMAttenuation(FLOAT32 fAttenuation)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAClipper : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAClipper };
	static const CClass classCGAClipper;
	virtual const IClass*		GetClass();

	virtual void		Terminate();
	virtual UINT32		Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL		SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAClipper
	virtual void		SetClipLevel(FLOAT32 fLevel);
	virtual FLOAT32		GetClipLevel();

	virtual void		SetCMAttenuation(FLOAT32 fAttenuation);


	// CTOR / DTOR
	CGAClipper();
	~CGAClipper();

protected:

	FLOAT32	m_fClipLevel;
	FLOAT32	m_fCMAttenuation;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAClipper_
// --------------------------------------------------------------------------------------------------------
