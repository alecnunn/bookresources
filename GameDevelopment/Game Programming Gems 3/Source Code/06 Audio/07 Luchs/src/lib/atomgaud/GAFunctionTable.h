// GAFunctionTable - Interface
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

#ifndef _atomosGAFunctionTable_
#define _atomosGAFunctionTable_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAFunctionTable : public IGAObject
{
	enum eType {
	
		Sin,
		Cos,
		NDivSin, // 1 / sin
	};

	virtual void	Initialize(UINT32 nType)=0;
	virtual FLOAT32 GetValue(UINT32 nPhase)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAFunctionTable : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAFunctionTable };
	static const CClass classCGAFunctionTable;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAFunctionTable
	virtual void	Initialize(UINT32 nType);
	virtual FLOAT32 GetValue(UINT32 nPhase);


	// CTOR / DTOR
	CGAFunctionTable();
	~CGAFunctionTable();

protected:


	UINT32		m_nSize;
	FLOAT32		m_fData[4096]; // @fix	

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAFunctionTable_
// --------------------------------------------------------------------------------------------------------
