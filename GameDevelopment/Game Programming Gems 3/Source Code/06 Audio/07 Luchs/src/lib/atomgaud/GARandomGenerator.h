// GARandomGenerator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The CGARandomGenerator generates a new random signal with a duration specified by
//				the duration variable.
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

#ifndef _atomosGARandomGenerator_
#define _atomosGARandomGenerator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGARandomGenerator : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGARandomGenerator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GARandomGenerator };
	static const CClass classCGARandomGenerator;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL		SetParameter(CHAR8* pszName, CHAR8* pszValue);
	// IGARandomGenerator


	// CTOR / DTOR
	CGARandomGenerator();
	~CGARandomGenerator();

protected:

	FLOAT32		m_fCurrSample;
	FLOAT32		m_fPrevSample;
	CGARandom	m_Random;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGARandomGenerator_
// --------------------------------------------------------------------------------------------------------
