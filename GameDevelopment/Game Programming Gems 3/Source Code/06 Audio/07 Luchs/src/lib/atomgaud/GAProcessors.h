// GAProcessors - Interface
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

#ifndef _atomosGAProcessors_
#define _atomosGAProcessors_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAProcessors : public IGAObject
{
	virtual IGAProcessor*	GetAt(UINT32 nIndex)=0;
	virtual IGAProcessor*	GetFirst()=0;
	virtual IGAProcessor*	GetLast()=0;
	virtual IGAProcessor*	Find(ATOM_CHAR* pszName)=0;

	virtual UINT32			GetCount()=0;
	virtual UINT32			GetIndex(ATOM_CHAR* pszName)=0;
	virtual UINT32			Append(IGAProcessor* pob)=0;

	virtual void Update()=0;
	virtual void Trigger()=0;


}; // interface

typedef std::vector<IGAProcessor*> GAProcessorArray;
typedef GAProcessorArray::iterator GAProcessorArrayIter;

class ATOMGAUD_DLLCLASS CGAProcessors : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAProcessors };
	static const CClass classCGAProcessors;
	virtual const IClass*		GetClass();

	virtual void Terminate();	
	virtual UINT32 Process(IObject* pob);

	// IGAProcessors

	virtual IGAProcessor*	GetAt(UINT32 nIndex);
	virtual IGAProcessor*	GetFirst();
	virtual IGAProcessor*	GetLast();
	virtual IGAProcessor*	Find(ATOM_CHAR* pszName);

	virtual UINT32			GetCount();
	virtual UINT32			GetIndex(ATOM_CHAR* pszName);
	virtual UINT32			Append(IGAProcessor* pob);

	virtual void Update();
	virtual void Trigger();




	// CTOR / DTOR
	CGAProcessors();
	~CGAProcessors();

protected:


	GAProcessorArray		m_ProcessorArray;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAProcessors_
// --------------------------------------------------------------------------------------------------------
