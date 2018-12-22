// GAObjectReader - Interface
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

#ifndef _atomosGAObjectReader_
#define _atomosGAObjectReader_

#include "GATokenizer.h"

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAObjectReader : public IGAFile
{
	virtual UINT32 Read(const ATOM_CHAR* pszPathName)=0;
	virtual ATOM_CHAR* GetPathName()=0;
	virtual UINT32 Process()=0;



}; // interface


class ATOMGAUD_DLLCLASS CGAObjectReader : public CGAFile 
{
public:

	// IObject
	enum { CID = CID_GAObjectReader };
	static const CClass classCGAObjectReader;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAObjectReader
	virtual UINT32 Read(const ATOM_CHAR* pszPathName);
	virtual ATOM_CHAR* GetPathName();
	virtual UINT32 Process();


	// CTOR / DTOR
	CGAObjectReader();
	~CGAObjectReader();

protected:

	ATOM_CHAR*	m_pszText;
	CGATokenizer	m_Tokenizer;
	ATOM_PATHNAME m_szPathName; // [260];

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAObjectReader_
// --------------------------------------------------------------------------------------------------------
