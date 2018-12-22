// GANumPadController - Interface
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

#ifndef _atomosGANumPadController_
#define _atomosGANumPadController_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:

struct KeyValue
{
	UINT32	nKey;
	FLOAT32	fValue;

};


interface IGANumPadController : public IGAProcessor
{



}; // interface


typedef std::vector<KeyControl*> KeyControlArray;

class ATOMGAUD_DLLCLASS CGANumPadController : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GANumPadController };
	static const CClass classCGANumPadController;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGANumPadController


	// CTOR / DTOR
	CGANumPadController();
	~CGANumPadController();

protected:

	KeyValue		m_KeyValue[11];

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGANumPadController_
// --------------------------------------------------------------------------------------------------------
