// GameAudioSystem - Interface
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

#ifndef _atomosGASystem_
#define _atomosGASystem_

#include <vector>
#include "GAKeyboard.h"
#include "GASampleWriter.h"
#include "GAFunctionTable.h"
#include "GASignalFormat.h"


#define DEV_ID_1            (13)
#define DEV_ID_2            (15)

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:
interface IGAPatch;

extern ISystem*  GetSystem()


interface IGADevice;
typedef std::vector<IGADevice*> DeviceArray;
typedef DeviceArray::iterator DeviceArrayIter;


interface IGAStream;
typedef std::vector<IGAStream*> StreamArray;
typedef StreamArray::iterator StreamArrayIter;




interface IGASystem : public ISystem
{
	virtual BOOL				InitPortAudio()=0;
	virtual void				Start()=0;
	virtual void				Stop()=0;
	virtual void				Flip()=0;
	virtual void				Trigger()=0;
	virtual void				Record()=0;

	virtual UINT32				AppendDevice(IGADevice* pDevice)=0;
	virtual UINT32				AppendStream(IGAStream* pStream)=0;
	virtual IGAPatches*			GetPatches()=0;
	virtual IGASignals*			GetSignals()=0;
	virtual IGAKeyboard*		GetKeyboard()=0;

	virtual void				HandleError(PaError err)=0;

	virtual void				SetSignalFormat(IGASignalFormat* pFormat)=0;
	virtual IGASignalFormat*	GetSignalFormat()=0;
	virtual FLOAT32				GetSampleRate()=0;
	virtual UINT32				GetFrameCount()=0;

	virtual INT32				Process(void *pIn, void *pOut, UINT32 nFrames, PaTimestamp dbTime, void* pUser)=0;


	virtual UINT32				UpdateDeviceList();
	virtual UINT32				GetDevice(INT32 nType, UINT32 nIndex)=0;

	virtual PortAudioCallback*	GetCallback()=0;

	virtual UINT32				InitProcessors()=0;
	virtual	UINT32				Command(UINT32 nCmd, FLOAT fValue)=0;

	virtual IGASignal*			CreateSignal(UINT32 nSize=0)=0;
	virtual IGAFunctionTable*	GetFunctionTable(UINT32 nType)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGASystem : public CSystem  
{
public:

	// IObject
	enum { CID = CID_GASystem };
	static const CClass classCGASystem;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual BOOL	Initialize(IObject* pob=NULL);

	// ISystem
	virtual UINT32				ReadConfiguration(ATOM_CHAR* pszFileName);


	// IGASystem
	virtual BOOL				InitPortAudio();
	virtual void				Start();
	virtual void				Stop();
	virtual void				Flip();
	virtual void				Trigger();
	virtual void				Record();

	virtual UINT32				AppendDevice(IGADevice* pDevice);
	virtual UINT32				AppendStream(IGAStream* pStream);
	virtual IGAPatches*			GetPatches();
	virtual IGASignals*			GetSignals();
	virtual IGAKeyboard*		GetKeyboard();

	virtual void				HandleError(PaError err);

	virtual void				SetSignalFormat(IGASignalFormat* pFormat);
	virtual IGASignalFormat*	GetSignalFormat();
	virtual FLOAT32				GetSampleRate();
	virtual UINT32				GetFrameCount();

	virtual INT32				Process(void *pIn, void *pOut, UINT32 nFrames, PaTimestamp dbTime, void* pUser);

	virtual UINT32				UpdateDeviceList();
	virtual UINT32				GetDevice(INT32 nType, UINT32 nIndex);

	virtual PortAudioCallback*	GetCallback();

	virtual UINT32				InitProcessors();
	virtual	UINT32				Command(UINT32 nCmd, FLOAT fValue);

	virtual IGASignal*			CreateSignal(UINT32 nSize=0);
//	virtual UINT32				ReadConfiguration();

	virtual IGAFunctionTable*	GetFunctionTable(UINT32 nType);



	static INT32				PAEngineCallback(void *pIn, void *pOut, unsigned long nFrames,
								PaTimestamp dbTime, void *pUser );


public:

	CGADump			m_Dump;
	
	// CTOR / DTOR
	CGASystem();
	~CGASystem();

protected:

	PaError					m_Error;

	DeviceArray				m_DeviceArray;
	StreamArray				m_StreamArray;

	CGAPatches				m_Patches;	
	CGASignals				m_Signals;		// allocated frame buffers

//	FLOAT64					m_fSampleRate;
//	UINT32					m_nFrameCount; // frames per block
	IGASignalFormat*		m_pSignalFormat;

	CGAKeyboard				m_Keyboard;
//	CGAObjectReader			m_ObjectReader;

	CGASampleWriter			m_Writer; // for raw write

	// lookup tables
	CGAFunctionTable		m_SinTable;
	CGAFunctionTable		m_NDivSinTable;


};

extern CGASystem g_GASystem;


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS



#endif // _atomosGASystem_
// --------------------------------------------------------------------------------------------------------



