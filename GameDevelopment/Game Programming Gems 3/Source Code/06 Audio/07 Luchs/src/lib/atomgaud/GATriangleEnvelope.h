// GATriangleEnvelope - Interface
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

#ifndef _atomosGATriangleEnvelope_
#define _atomosGATriangleEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGATriangleEnvelope : public IGAEnvelope
{
	
	virtual void SetCenter(FLOAT32 fCenter)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGATriangleEnvelope : public CGAEnvelope  
{
public:

	// IObject
	enum { CID = CID_GATriangleEnvelope };
	static const CClass classCGATriangleEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);
	// IGAProcessor

	// IGATriangleEnvelope
	virtual void SetCenter(FLOAT32 fCenter);

	// CTOR / DTOR
	CGATriangleEnvelope();
	~CGATriangleEnvelope();

protected:

	FLOAT32	m_fCenter; // from 0 to 1 


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGATriangleEnvelope_
// --------------------------------------------------------------------------------------------------------
