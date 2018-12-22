// GAKeyboardController - Interface
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

#ifndef _atomosGAKeyboardController_
#define _atomosGAKeyboardController_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAKeyboardController : public IGAProcessor
{

	virtual void AppendKey(UINT32 nOut, UINT32 nKey, FLOAT32 fValue, FLOAT32 fLag = 0.0f)=0;


}; // interface

struct OutValue
{
	FLOAT32	fCurrValue; // the current value
//	FLOAT32 fValue;		

};

struct KeyControl
{
	UINT32		nOut;
	UINT32		nKey;
	FLOAT32		fLag;		// time to reach the value
	FLOAT32		fTarget;		// target value	
	OutValue*	pValue;

};




typedef std::vector<KeyControl*> KeyControlArray;

class ATOMGAUD_DLLCLASS CGAKeyboardController : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAKeyboardController };
	static const CClass classCGAKeyboardController;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAKeyboardController
	virtual void AppendKey(UINT32 nOut, UINT32 nKey, FLOAT32 fValue, FLOAT32 fLag = 0.0f);


	// CTOR / DTOR
	CGAKeyboardController();
	~CGAKeyboardController();

protected:

	OutValue		m_OutValue[256];
	FLOAT32			m_fAmount;
	KeyControlArray	m_Array;	

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAKeyboardController_
// --------------------------------------------------------------------------------------------------------
