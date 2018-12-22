// GADump - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	microsoft specific dump to output window using ::OutputDebugString
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0201
// Author:		Frank Luchs
// History:
//				1997-06-11	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGADump_
#define _atomosGADump_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGADump : public IGAObject
{

	virtual void	Write(ATOM_CHAR* psz)=0;
	virtual void	Write(FLOAT32 fValue)=0;
	virtual void	Write(UINT32 nValue)=0;
	virtual void	Write(INT32 nValue)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGADump : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GADump };
	static const CClass classCGADump;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGADump
	virtual void	Write(ATOM_CHAR* psz);
	virtual void	Write(FLOAT32 fValue);
	virtual void	Write(UINT32 nValue);
	virtual void	Write(INT32 nValue);

	// CTOR / DTOR
	CGADump();
	~CGADump();

protected:


	TCHAR	m_szBuffer[4096];

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGADump_
// --------------------------------------------------------------------------------------------------------
