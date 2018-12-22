// NamedObject - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMBASE
// Description:	object with name support
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/atomos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of the Atomos Application Base
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosNamedObject_
#define _atomosNamedObject_


namespace atomos
{
// forward declarations:


/*
interface INamedObject : public IObject
{
	virtual void		SetName(const ATOM_CHAR* pszName)=0;
	virtual ATOM_CHAR*	GetName()=0;
	virtual UINT32		EqualsName(const ATOM_CHAR* pszName, UINT32 nFlags=0)=0;

}; // interface
*/

class ATOMBASE_DLLCLASS CNamedObject : public CObject  
{
public:

	typedef enum eCompareFlags
	{
		NoCase=0, // default
		Case,


		CompareFlagsCount

	} COMPAREFLAGS;


	// IObject
	enum { CID = CID_NamedObject };
	static const CClass classCNamedObject;
	virtual const IClass*		GetClass();

	virtual BOOL			SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// INamedObject
	virtual void		SetName(const ATOM_CHAR* pszName);
	virtual ATOM_CHAR*	GetName();
	virtual UINT32		EqualsName(const ATOM_CHAR* pszName, UINT32 nFlags=0);


	// CTOR / DTOR
	CNamedObject();
	~CNamedObject();

protected:


	CHAR8*	m_pszName;

};


} // namespace atomos

#endif // _atomosNamedObject_
// --------------------------------------------------------------------------------------------------------
