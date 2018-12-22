// GATokenizer - Interface
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

#ifndef _atomosGATokenizer_
#define _atomosGATokenizer_

#include <vector>
//#include <string>

#include "GAToken.h"

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGATokenizer : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGATokenizer : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GATokenizer };
	static const CClass classCGATokenizer;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGATokenizer

	virtual bool IsSpace(ATOM_CHAR c);
	virtual bool IsOperator(ATOM_CHAR c);
	virtual bool IsRemark(ATOM_CHAR c, int position);

	//virtual std::string GetNextToken(int *offset=NULL);
//	virtual std::string PeekNextToken(int *offset=NULL);
//	virtual void PopToPeek();
	virtual void Dump();

	virtual	void Process( ATOM_CHAR* pszText);
	virtual UINT32		GetTokenCount();
	virtual ATOM_CHAR*	GetToken(UINT32 nIndex);

	// CTOR / DTOR
	CGATokenizer();
	~CGATokenizer();

protected:


private:


	std::vector<CGAToken*> m_Tokens;

	int				peek_pos;
	int				last_line;
	bool			in_remark;

	ATOM_CHAR*		m_pszText;
	UINT32			m_nSize;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGATokenizer_
// --------------------------------------------------------------------------------------------------------
 