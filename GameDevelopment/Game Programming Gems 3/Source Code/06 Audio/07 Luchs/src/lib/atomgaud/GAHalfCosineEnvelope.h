// GAHalfCosineEnvelope - Interface
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

#ifndef _atomosGAHalfCosineEnvelope_
#define _atomosGAHalfCosineEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAHalfCosineEnvelope : public IGAEnvelope
{
	
	virtual void SetMode(UINT32 nMode)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAHalfCosineEnvelope : public CGAEnvelope  
{
public:

	// IObject
	enum { CID = CID_GAHalfCosineEnvelope };
	static const CClass classCGAHalfCosineEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);
	// IGAProcessor

	// IGAHalfCosineEnvelope
	virtual void SetMode(UINT32 nMode);

	// CTOR / DTOR
	CGAHalfCosineEnvelope();
	~CGAHalfCosineEnvelope();

protected:

	UINT32	m_nMode; // 0= descending 1= ascending

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAHalfCosineEnvelope_
// --------------------------------------------------------------------------------------------------------
