// GAPin - Interface
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

#ifndef _atomosGAPin_
#define _atomosGAPin_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:
interface IGASignal;
interface IGAProcessor;


interface IGAPin : public IGAObject
{
	// IGAPin
	enum eDirection
	{
		Output,
		Input

	} DIRECTION;

	virtual FLOAT32*		GetSignalData()=0;
	virtual IGASignal*		GetSignal()=0;
	virtual BOOL			SetSignal(IGASignal* pSignal)=0;
	virtual void			SetConnectedPin(IGAPin* pPin)=0;
	virtual IGAPin*			GetConnectedPin()=0;
	virtual IGAProcessor*	GetProcessor()=0;
	virtual BOOL			IsInput();

}; // interface


class ATOMGAUD_DLLCLASS CGAPin : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAPin };
	static const CClass classCGAPin;
	virtual const IClass*		GetClass();


	virtual void Terminate();
	virtual UINT32			GetID();


	
	virtual FLOAT32*		GetSignalData();
	virtual IGASignal*		GetSignal();
	virtual BOOL			SetSignal(IGASignal* pSignal);
	virtual void			SetConnectedPin(IGAPin* pPin);
	virtual IGAPin*			GetConnectedPin();
	virtual IGAProcessor*	GetProcessor();
	virtual BOOL			IsInput();

	// CTOR / DTOR
	CGAPin(IGAProcessor* pProcessor=NULL, UINT32 nPinID=0, ATOM_CHAR* pszName=NULL, IGASignal* pSignal=NULL);
	~CGAPin();


protected:

	void SetDefaultName();

protected:

	IGAProcessor*		m_pProcessor;	// owner
	IGAPin*				m_pConnectedPin;	// owner
	IGASignal*			m_pSignal;

	UINT32				m_nPinID;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPin_
// --------------------------------------------------------------------------------------------------------
