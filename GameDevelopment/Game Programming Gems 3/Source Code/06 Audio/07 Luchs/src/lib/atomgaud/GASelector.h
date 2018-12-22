// GASelector - Interface
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

#ifndef _atomosGASelector_
#define _atomosGASelector_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASelector : public IGAProcessor
{

	virtual void	ActivateInputs(UINT32 nMask)=0;
	virtual void	SelectNext()=0;


}; // interface


class ATOMGAUD_DLLCLASS CGASelector : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GASelector };
	static const CClass classCGASelector;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGASelector
	virtual void	ActivateInputs(UINT32 nMask);
	virtual void	SelectNext();

	// CTOR / DTOR
	CGASelector();
	~CGASelector();

protected:

	UINT32	m_nMask;
	UINT32	m_nCurrInput;
	FLOAT32	m_fLastImpulse;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASelector_
// --------------------------------------------------------------------------------------------------------
