// GACrossFade3 - Interface
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

#ifndef _atomosGACrossFade3_
#define _atomosGACrossFade3_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGACrossFade3 : public IGAProcessor
{

	virtual BOOL SetInputGain(UINT32 nChannel, FLOAT32 fGain)=0;
	virtual void SetBalance(FLOAT32 fBalance)=0;

}; // interface


class ATOMGAUD_DLLCLASS CGACrossFade3 : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GACrossFade3 };
	static const CClass classCGACrossFade3;
	virtual const IClass*		GetClass();

	virtual void	Terminate();
	virtual UINT32	Process(IObject* pContext=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGACrossFade3
	virtual BOOL SetInputGain(UINT32 nChannel, FLOAT32 fGain);
	virtual void SetBalance(FLOAT32 fBalance);


	// CTOR / DTOR
	CGACrossFade3();
	~CGACrossFade3();

protected:


	FLOAT32		m_fAtt[3];
	FLOAT32		m_fBalance;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGACrossFade3_
// --------------------------------------------------------------------------------------------------------
 