// GAProcessor - Interface
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

#ifndef _atomosGAProcessor_
#define _atomosGAProcessor_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAProcessor : public IGAObject
{
	virtual FLOAT32*	GetSignalData(UINT32 nIndex)=0;
	virtual IGASignal*	GetSignal(UINT32 nPinID)=0;
	virtual BOOL		SetSignal(UINT32 nPinID, IGASignal* pSignal)=0;
	virtual IGAPins*	GetPins()=0;
	virtual IGAPin*		AppendPin(UINT32 nPinID, ATOM_CHAR* pszName=NULL, IGASignal* pSignal=NULL)=0;
	virtual void		Trigger()=0;
	virtual void		Start()=0;
	virtual void		Stop()=0;
	virtual void		Update()=0;
	virtual void		SetBypass(bool b)=0;

//	virtual FLOAT32*		GetPin(UINT32 nType, UINT32 nIndex)=0;

	virtual BOOL		Connect(UINT32 npinOut, IGAProcessor* pNext, UINT32 nPinIn)=0;
//	virtual BOOL		SetParameter(UINT32 idParam, FLOAT32 fValue)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAProcessor : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAProcessor };
	static const CClass classCGAProcessor;
	virtual const IClass*		GetClass();

	virtual void			Terminate();	
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAObject
	virtual BOOL		SetParameter(CHAR8* pszName, CHAR8* pszValue);
	
	// IGAProcessor
	virtual FLOAT32*	GetSignalData(UINT32 nIndex);
	virtual IGASignal*	GetSignal(UINT32 nPinID);
	virtual BOOL		SetSignal(UINT32 nPinID, IGASignal* pSignal);

	virtual IGAPins*	GetPins();
	virtual IGAPin*		AppendPin(UINT32 nPinID, ATOM_CHAR* pszName=NULL,IGASignal* pSignal=NULL);

	virtual void		Trigger();
	virtual void		Start();
	virtual void		Stop();
	virtual void		Update();
	virtual void		SetBypass(bool b);

	virtual BOOL		Connect(UINT32 npinOut, IGAProcessor* pNext, UINT32 nPinIn);
//	virtual BOOL		SetParameter(UINT32 idParam, FLOAT32 fValue);



	// CTOR / DTOR
	CGAProcessor();
	~CGAProcessor();


//	FLOAT*	m_pfOutput; // this has to be greater than the framecount in 'Process'


protected:

	CGAPins	m_Pins;
	bool	m_bBypass;
	FLOAT32	m_fSample;		// the current sample
	UINT32	m_nSampleCounter;
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAProcessor_
// --------------------------------------------------------------------------------------------------------
