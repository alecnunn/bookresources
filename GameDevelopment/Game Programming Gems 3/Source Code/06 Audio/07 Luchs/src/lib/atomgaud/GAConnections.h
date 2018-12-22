// GAConnection - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	array for objects of class GAConnection
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

#ifndef _atomosGAConnections_
#define _atomosGAConnections_


namespace atomos
{
// forward declarations:



interface IGAConnections : public IObject
{
	virtual IGAConnection*		GetAt(UINT32 nIndex)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(IGAConnection* pob)=0;
	virtual IGAConnection*		Find(ATOM_CHAR* pszName)=0;
	virtual BOOL			Remove(IGAConnection* pGAConnection)=0;
	virtual UINT32			RemoveAll()=0;


}; // interface

typedef std::vector<IGAConnection*> GAConnectionArray;
typedef GAConnectionArray::iterator GAConnectionArrayIter;



class ATOMGAUD_DLLCLASS CGAConnections : public CObject  
{
public:

	// IObject
	enum { CID = CID_GAConnections };
	static const CClass classCGAConnections;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	// IGAConnection
	virtual IGAConnection*		GetAt(UINT32 nIndex);
	virtual UINT32			GetCount();
	virtual UINT32			Append(IGAConnection* pob);
	virtual IGAConnection*		Find(ATOM_CHAR* pszName);
	virtual BOOL			Remove(IGAConnection* pGAConnection);
	virtual UINT32			RemoveAll();


	// CTOR / DTOR
	CGAConnections();
	~CGAConnections();

protected:


	GAConnectionArray	m_GAConnectionArray;

};


} // namespace atomos

#endif // _atomosGAConnections_
// --------------------------------------------------------------------------------------------------------
