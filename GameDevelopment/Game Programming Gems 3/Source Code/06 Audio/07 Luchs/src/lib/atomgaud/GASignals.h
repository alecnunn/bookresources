// GASignal - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	array for objects of class GASignal
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

#ifndef _atomosGASignals_
#define _atomosGASignals_


namespace atomos
{
// forward declarations:



interface IGASignals : public IObject
{
	virtual IGASignal*		GetAt(UINT32 nIndex)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(IGASignal* pob)=0;
	virtual IGASignal*		Find(ATOM_CHAR* pszName)=0;
	virtual BOOL			Remove(IGASignal* pGASignal)=0;
	virtual UINT32			RemoveAll()=0;


}; // interface

typedef std::vector<IGASignal*> GASignalArray;
typedef GASignalArray::iterator GASignalArrayIter;



class ATOMGAUD_DLLCLASS CGASignals : public CObject  
{
public:

	// IObject
	enum { CID = CID_GASignals };
	static const CClass classCGASignals;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	// IGASignal
	virtual IGASignal*		GetAt(UINT32 nIndex);
	virtual UINT32			GetCount();
	virtual UINT32			Append(IGASignal* pob);
	virtual IGASignal*		Find(ATOM_CHAR* pszName);
	virtual BOOL			Remove(IGASignal* pGASignal);
	virtual UINT32			RemoveAll();


	// CTOR / DTOR
	CGASignals();
	~CGASignals();

protected:


	GASignalArray	m_GASignalArray;

};


} // namespace atomos

#endif // _atomosGASignals_
// --------------------------------------------------------------------------------------------------------
