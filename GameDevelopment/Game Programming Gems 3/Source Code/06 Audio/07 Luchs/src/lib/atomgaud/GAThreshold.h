// GAThreshold - Interface
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

#ifndef _atomosGAThreshold_
#define _atomosGAThreshold_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAThreshold : public IGAProcessor
{
	virtual void		SetLowerLevel(FLOAT32 fLevel)=0;
	virtual FLOAT32		GetLowerLevel()=0;
	virtual void		SetUpperLevel(FLOAT32 fLevel)=0;
	virtual FLOAT32		GetUpperLevel()=0;



}; // interface


class ATOMGAUD_DLLCLASS CGAThreshold : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAThreshold };
	static const CClass classCGAThreshold;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAThreshold
	virtual void		SetLowerLevel(FLOAT32 fLevel);
	virtual FLOAT32		GetLowerLevel();
	virtual void		SetUpperLevel(FLOAT32 fLevel);
	virtual FLOAT32		GetUpperLevel();


	// CTOR / DTOR
	CGAThreshold();
	~CGAThreshold();

protected:

    FLOAT32		m_fUpperLevel;
    FLOAT32		m_fLowerLevel;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAThreshold_
// --------------------------------------------------------------------------------------------------------
