// GAPatches - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
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

#ifndef _atomosGAPatches_
#define _atomosGAPatches_

#include <vector>
 
#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAPatches : public IGAObject
{
	virtual IGAPatch*		GetAt(UINT32 nIndex)=0;
	virtual IGAPatch*		GetNext()=0;
	virtual IGAPatch*		GetPrevious()=0;
	virtual IGAPatch*		GetCurrent()=0;
	virtual UINT32			SetCurrent(IGAPatch* pPatch)=0;

	virtual UINT32		GetCount()=0;
	virtual UINT32		Append(IGAPatch* pob)=0;

	virtual UINT32		RemoveAll()=0;


}; // interface

typedef std::vector<IGAPatch*> PatchArray;
typedef PatchArray::iterator PatchArrayIter;

class ATOMGAUD_DLLCLASS CGAPatches : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAPatches };
	static const CClass classCGAPatches;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	

	// IGAPatches
	// IObjects
	virtual IGAPatch*		GetAt(UINT32 nIndex);
	virtual IGAPatch*		GetNext();
	virtual IGAPatch*		GetPrevious();
	virtual IGAPatch*		GetCurrent();
	virtual UINT32			SetCurrent(IGAPatch* pPatch);

	virtual UINT32			GetCount();
	virtual UINT32			Append(IGAPatch* pob);

	virtual UINT32			RemoveAll();



	// CTOR / DTOR
	CGAPatches();
	~CGAPatches();

protected:

	PatchArray		m_PatchArray;
	UINT32			m_nCurrPatch;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPatches_
// --------------------------------------------------------------------------------------------------------
