// GACrossFade2 - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	GACrossFade2 mixes 2 input signals controlled by the input at 'Balance'. The default  
//				balance is 0.5.
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

#ifndef _atomosGACrossFade2_
#define _atomosGACrossFade2_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGACrossFade2 : public IGAProcessor
{

	virtual void SetBalance(FLOAT32 fLevel1)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGACrossFade2 : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GACrossFade2 };
	static const CClass classCGACrossFade2;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGACrossFade2
	virtual void SetBalance(FLOAT32 fLevel1);

	// CTOR / DTOR
	CGACrossFade2();
	~CGACrossFade2();

protected:

//	FLOAT32*	m_pfOutput;
	FLOAT32	m_fBalance;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGACrossFade2_
// --------------------------------------------------------------------------------------------------------
