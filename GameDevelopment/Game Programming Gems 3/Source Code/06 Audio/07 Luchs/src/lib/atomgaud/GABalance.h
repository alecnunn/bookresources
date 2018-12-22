// GABalance - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	GABalance mixes 2 input signals controlled by the input at 'Balance'. The default  
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

#ifndef _atomosGABalance_
#define _atomosGABalance_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGABalance : public IGAProcessor
{

	virtual void SetBalance(FLOAT32 fLevel1)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGABalance : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GABalance };
	static const CClass classCGABalance;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGABalance
	virtual void SetBalance(FLOAT32 fLevel1);

	// CTOR / DTOR
	CGABalance();
	~CGABalance();

protected:

	FLOAT32	m_fBalance;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGABalance_
// --------------------------------------------------------------------------------------------------------
