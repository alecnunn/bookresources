// GAPins - Interface
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

#ifndef _atomosGAPins_
#define _atomosGAPins_

#include <vector>

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAPins : public IGAObject
{
	virtual IGAPin*		GetAt(UINT32 nIndex)=0;
	virtual IGAPin*		GetFirst()=0;
	virtual IGAPin*		GetLast()=0;
	virtual IGAPin*		Find(UINT32 nID)=0;

	virtual UINT32		GetCount(UINT32 nWhich= IGAPin::Input || IGAPin::Output)=0;
	virtual UINT32		Append(IGAPin* pob)=0;

	virtual FLOAT32*	GetSignalData(UINT32 nIndex)=0;
	virtual IGASignal*	GetSignal(UINT32 nIndex)=0;
	virtual BOOL		SetSignal(UINT32 nIndex, IGASignal* pSignal)=0;

}; // interface

typedef std::vector<IGAPin*> GAPinArray;
typedef GAPinArray::iterator GAPinArrayIter;

class ATOMGAUD_DLLCLASS CGAPins : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAPins };
	static const CClass classCGAPins;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();

	// IGAPins
	virtual IGAPin*		GetAt(UINT32 nIndex);
	virtual IGAPin*		GetFirst();
	virtual IGAPin*		GetLast();
	virtual IGAPin*		Find(UINT32 nID);

	virtual UINT32		GetCount(UINT32 nWhich= IGAPin::Input || IGAPin::Output );
	virtual UINT32		Append(IGAPin* pob);

	virtual FLOAT32*	GetSignalData(UINT32 nIndex);
	virtual IGASignal*	GetSignal(UINT32 nIndex);
	virtual BOOL		SetSignal(UINT32 nIndex, IGASignal* pSignal);

	// CTOR / DTOR
	CGAPins();
	~CGAPins();


	static	UINT32		FindPinID(CHAR8* pszName);
	static	CHAR8*		FindPinName(UINT32 nID);

protected:


	GAPinArray		m_PinArray;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPins_
// --------------------------------------------------------------------------------------------------------
