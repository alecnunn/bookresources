// GASystem Implementation
// --------------------------------------------------------------------------------------------------------
// System:		GA
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/GA/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of the GA Engine
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GASystem.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASystem() { return( new CGASystem); }
const CClass CGASystem::classCGASystem(CID_GASystem, CID_System, "CGASystem", CreateCGASystem);
const IClass*	CGASystem::GetClass() { return((const IClass*)&classCGASystem); }

CGASystem	g_GASystem;

ISystem*  GetSystem()
{
	return((ISystem*)&g_GASystem);
}



// CTOR
CGASystem::CGASystem()
{
	SetName("GASystem");
	m_pSignalFormat = NULL;

//	m_fSampleRate = 44100.0f;
//	m_nFrameCount = 1024;			// frames per buffer

}

// DTOR
CGASystem::~CGASystem()
{
	Terminate();
}


void CGASystem::Terminate()
{
	ATOM_DELETE(m_pSignalFormat);

	m_Signals.RemoveAll();
	m_Patches.RemoveAll();

	
	CSystem::Terminate();


	StreamArrayIter iter;

	for (iter = m_StreamArray.begin(); iter != m_StreamArray.end(); iter++) {
		
		IGAStream* pStream = *iter;
		pStream->Terminate();
		delete(pStream);
		pStream = NULL;

	}
	m_StreamArray.clear();

	Pa_Terminate();

}


// append all classes for this system
// this is sorted alphabetically
BOOL CGASystem::Initialize(IObject* pob)
{
#ifdef _DEBUG
	::OutputDebugString("init CGASystem...\r\n");
#endif

	AppendClass((IClass*)&CGAKeyboard::classCGAKeyboard);
	AppendClass((IClass*)&CGAKeyboardController::classCGAKeyboardController);
	AppendClass((IClass*)&CGAToken::classCGAToken);
	AppendClass((IClass*)&CGATokenizer::classCGATokenizer);
	AppendClass((IClass*)&CGAParser::classCGAParser);

	AppendClass((IClass*)&CGA303Filter::classCGA303Filter);
	AppendClass((IClass*)&CGAAbs::classCGAAbs);
	AppendClass((IClass*)&CGAAverager::classCGAAverager);
	AppendClass((IClass*)&CGABalance::classCGABalance);
	AppendClass((IClass*)&CGABipolarToUnipolar::classCGABipolarToUnipolar);
	AppendClass((IClass*)&CGAButterworthFilter::classCGAButterworthFilter);
	AppendClass((IClass*)&CGAButterworthLPF::classCGAButterworthLPF);
	AppendClass((IClass*)&CGASawtoothOscDSF::classCGASawtoothOscDSF);
	AppendClass((IClass*)&CGAClippedSineOscillator::classCGAClippedSineOscillator);
	AppendClass((IClass*)&CGAClipper::classCGAClipper);
	AppendClass((IClass*)&CGAConfigurationReader::classCGAConfigurationReader);
	AppendClass((IClass*)&CGAConnection::classCGAConnection);
	AppendClass((IClass*)&CGAConnections::classCGAConnections);
	AppendClass((IClass*)&CGAController::classCGAController);
	AppendClass((IClass*)&CGACrossFade2::classCGACrossFade2);
	AppendClass((IClass*)&CGACrossFade3::classCGACrossFade3);
	AppendClass((IClass*)&CGACubic::classCGACubic);
	AppendClass((IClass*)&CGADelay::classCGADelay);
	AppendClass((IClass*)&CGADevice::classCGADevice);
	AppendClass((IClass*)&CGAEnvelope::classCGAEnvelope);
	AppendClass((IClass*)&CGAExpCurve::classCGAExpCurve);
	AppendClass((IClass*)&CGAFile::classCGAFile);
	AppendClass((IClass*)&CGAFunctionTable::classCGAFunctionTable);
	AppendClass((IClass*)&CGAGain::classCGAGain);
	AppendClass((IClass*)&CGAGaussEnvelope::classCGAGaussEnvelope);
	AppendClass((IClass*)&CGAGenerator::classCGAGenerator);
	AppendClass((IClass*)&CGAGlide::classCGAGlide);
	AppendClass((IClass*)&CGAHalfCosineEnvelope::classCGAHalfCosineEnvelope);
	AppendClass((IClass*)&CGAImpulseGenerator::classCGAImpulseGenerator);
	AppendClass((IClass*)&CGAInterleaver::classCGAInterleaver);
	AppendClass((IClass*)&CGAInverter::classCGAInverter);
	AppendClass((IClass*)&CGALine::classCGALine);
	AppendClass((IClass*)&CGAMath::classCGAMath);
	AppendClass((IClass*)&CGAMixer::classCGAMixer);
	AppendClass((IClass*)&CGAMixer2::classCGAMixer2);
	AppendClass((IClass*)&CGAMixer3::classCGAMixer3);
	AppendClass((IClass*)&CGAMixer4::classCGAMixer4);
	AppendClass((IClass*)&CGAMixer6::classCGAMixer6);
	AppendClass((IClass*)&CGANoiseGenerator::classCGANoiseGenerator);
	AppendClass((IClass*)&CGANoiseMSineOsc::classCGANoiseMSineOsc);
	AppendClass((IClass*)&CGANoiseParticle::classCGANoiseParticle);
	AppendClass((IClass*)&CGANumPadController::classCGANumPadController);
	AppendClass((IClass*)&CGAObject::classCGAObject);
	AppendClass((IClass*)&CGAObjectReader::classCGAObjectReader);
	AppendClass((IClass*)&CGAOscillator::classCGAOscillator);
	AppendClass((IClass*)&CGAPatch::classCGAPatch);
	AppendClass((IClass*)&CGAPatches::classCGAPatches);
	AppendClass((IClass*)&CGAPhaser::classCGAPhaser);
	AppendClass((IClass*)&CGAPin::classCGAPin);
	AppendClass((IClass*)&CGAProcessor::classCGAProcessor);
	AppendClass((IClass*)&CGAProcessors::classCGAProcessors);
	AppendClass((IClass*)&CGAPulseOscillator::classCGAPulseOscillator);
	AppendClass((IClass*)&CGARampEnvelope::classCGARampEnvelope);
	AppendClass((IClass*)&CGARandom::classCGARandom);
	AppendClass((IClass*)&CGARandomGenerator::classCGARandomGenerator);
	AppendClass((IClass*)&CGARenderer::classCGARenderer);
	AppendClass((IClass*)&CGAResonator::classCGAResonator);
	AppendClass((IClass*)&CGARoarOscillator::classCGARoarOscillator);
	AppendClass((IClass*)&CGASampleAndHold::classCGASampleAndHold);
	AppendClass((IClass*)&CGASampleWriter::classCGASampleWriter);
	AppendClass((IClass*)&CGASawtoothOscBLIT::classCGASawtoothOscBLIT);
	AppendClass((IClass*)&CGASawtoothOscillator::classCGASawtoothOscillator);
	AppendClass((IClass*)&CGAScaleMapper::classCGAScaleMapper);
	AppendClass((IClass*)&CGASelector::classCGASelector);
	AppendClass((IClass*)&CGASelector2::classCGASelector2);
	AppendClass((IClass*)&CGASequencer16::classCGASequencer16);
	AppendClass((IClass*)&CGAShapeOscillator::classCGAShapeOscillator);
	AppendClass((IClass*)&CGASignal::classCGASignal);
	AppendClass((IClass*)&CGASignalFormat::classCGASignalFormat);
	AppendClass((IClass*)&CGASignals::classCGASignals);
	AppendClass((IClass*)&CGASignalRandomizer::classCGASignalRandomizer);
	AppendClass((IClass*)&CGASineOscillator::classCGASineOscillator);
	AppendClass((IClass*)&CGASineXOscillator::classCGASineXOscillator);
	AppendClass((IClass*)&CGASinCosOscillator::classCGASinCosOscillator);
	AppendClass((IClass*)&CGASphereEnvelope::classCGASphereEnvelope);
	AppendClass((IClass*)&CGASquareOscillator::classCGASquareOscillator);
	AppendClass((IClass*)&CGAStream::classCGAStream);
	AppendClass((IClass*)&CGASystem::classCGASystem);
	AppendClass((IClass*)&CGAThreshold::classCGAThreshold);
	AppendClass((IClass*)&CGATrapezEnvelope::classCGATrapezEnvelope);
	AppendClass((IClass*)&CGATriangleEnvelope::classCGATriangleEnvelope);
	AppendClass((IClass*)&CGATriangleOscillator::classCGATriangleOscillator);
	AppendClass((IClass*)&CGAWaveFile::classCGAWaveFile);
	AppendClass((IClass*)&CGAWavetableOscillator::classCGAWavetableOscillator);


	InitPortAudio();

//	ReadConfiguration();

	// init some tables
	m_SinTable.Initialize(IGAFunctionTable::Sin);
	m_NDivSinTable.Initialize(IGAFunctionTable::NDivSin);;


	return(TRUE);
}

IGAFunctionTable* CGASystem::GetFunctionTable(UINT32 nType)
{
	IGAFunctionTable* pTable = NULL;
	
	switch(nType)
	{
	case IGAFunctionTable::Sin:
		pTable = (IGAFunctionTable*)&m_SinTable;
		break;
	case IGAFunctionTable::NDivSin:
		pTable = (IGAFunctionTable*)&m_NDivSinTable;
		break;

	}

	return(pTable);
}


// Initialize library before making any other calls. 
int CGASystem::InitPortAudio()
{


	m_Error = Pa_Initialize();
	if( m_Error != paNoError ) {
		HandleError(m_Error);
		return(m_Error);
	}

	UpdateDeviceList();
	InitProcessors();

	return 0;
}



UINT32 CGASystem::InitProcessors()
{
	// create your processors in derived class
	return(0);
}

// we always get owner of this created object
void CGASystem::SetSignalFormat(IGASignalFormat* pFormat)
{
	ATOM_DELETE(m_pSignalFormat);
	m_pSignalFormat = pFormat;
}

IGASignalFormat* CGASystem::GetSignalFormat()
{
	return(m_pSignalFormat);
}



FLOAT32	CGASystem::GetSampleRate()
{
	FLOAT32 fSR = 44100.0f;
	if(m_pSignalFormat != NULL)
		fSR = m_pSignalFormat->GetSampleRate();

	return(fSR);
}

// return frames per buffer in call of Process
UINT32 CGASystem::GetFrameCount()
{
	UINT32 nCount = 256;
	if(m_pSignalFormat != NULL)
		nCount = m_pSignalFormat->GetFrameCount();

	return(nCount);
}

// retrieve devices
// create a new device object for each device inf found
UINT32 CGASystem::UpdateDeviceList()
{
	UINT32 nCount = Pa_CountDevices();
	const PaDeviceInfo *pdi;

	for( UINT32 id=0; id < nCount; id++ ) {

		pdi = Pa_GetDeviceInfo( id );		





	}



	return(nCount);
}

UINT32 CGASystem::GetDevice(INT32 nType, UINT32 nIndex)
{
	return(0);
}

PortAudioCallback* CGASystem::GetCallback()
{
	return(PAEngineCallback);
}




void CGASystem::Flip()
{

	IGAStream* pStream = m_StreamArray.at(0);
	if(pStream != NULL)
		pStream->Flip();

}



void CGASystem::Start()
{
	m_Error = paNoError;

	// send start to current patch
	IGAPatch* pPatch = m_Patches.GetCurrent();
	
	if(pPatch != NULL) {
		
		pPatch->Start();

		IGAStream* pStream = m_StreamArray.at(0);
		pStream->Start();

		if( m_Error != paNoError )
			HandleError(m_Error);
	}

}

void CGASystem::Stop()
{
	m_Error = paNoError;
	IGAStream* pStream = m_StreamArray.at(0);
	if(pStream != NULL) {
		pStream->Stop();

		// send stop to current patch
		IGAPatch* pPatch = m_Patches.GetCurrent();
		if(pPatch != NULL)
			pPatch->Stop();


		if( m_Error != paNoError )
			HandleError(m_Error);
	}
}

// rec to file
void CGASystem::Record()
{
	m_Writer.Start();

}

// send trigger to each processor in current patch
void CGASystem::Trigger()
{
	IGAPatch* pPatch = m_Patches.GetCurrent();
	if(pPatch != NULL)
		pPatch->Trigger();
}


void CGASystem::HandleError(PaError err)
{
	/*
	if( err != paNoError ) {
		fprintf( stderr, "An error occured while using the portaudio stream\n" ); 
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );

	}
	*/
}

UINT32 CGASystem::AppendDevice(IGADevice* pDevice)
{
	UINT32 nID = m_DeviceArray.size();
	m_DeviceArray.push_back(pDevice);
	return(nID);
}


UINT32 CGASystem::AppendStream(IGAStream* pStream)
{
	UINT32 nID = m_StreamArray.size();
	m_StreamArray.push_back(pStream);
	return(nID);
}


IGAPatches* CGASystem::GetPatches()
{
	return((IGAPatches*)&m_Patches);
}

IGASignals* CGASystem::GetSignals()
{
	return((IGASignals*)&m_Signals);
}

IGAKeyboard* CGASystem::GetKeyboard()
{
	return((IGAKeyboard*)&m_Keyboard);
}



/*
// STEREO
// called by callback of portaudio
// this delegates to the processor chain
// return 0 on success
// this handles mono or stereo streams
INT32 CGASystem::Process(void *pIn, void *pDest, UINT32 nFrames, PaTimestamp dbTime, void* pUser)
{
	CGAStream* pStream = (CGAStream*)pUser;

	IGAPatch* pPatch = m_Patches.GetCurrent();

	if(pPatch != NULL) {

		pPatch->Process(NULL);
		IGAProcessors* pProcessors = pPatch->GetProcessors();

		IGAProcessor* pProcessor = pProcessors->GetLast();
		if(pProcessor != NULL) {

			IClass* pClass = (IClass*)pProcessor->GetClass();


			// copy from last processors sample out to output
			IGASignal* pSignal = pProcessor->GetSignal(SampleOut);
			FLOAT32* pfDest = (FLOAT32*) pDest; 
			FLOAT32* pfRecord = pfDest; 
			FLOAT32* pfOut = pSignal->GetData() ; 
			UINT32 nSamples = pSignal->GetCount();

			if(pClass->IsA(CID_GAInterleaver)) {


				for(UINT32 i=0;i < nSamples;i++) {

					FLOAT32 fSample = pfOut[i];
					if(fSample > 1.0f)		// limit
						fSample = 1.0f;
					else if(fSample < -1.0f)
						fSample = -1.0f;

					pfDest[i] = fSample;
				}

				if(m_Writer.m_bWorking == true)
					m_Writer.Append(pfRecord,nSamples); 


			}
			else {

				for(UINT32 i=0;i < nSamples;i++) {

					FLOAT32 fSample = pfOut[i];
					if(fSample > 1.0f)		// limit
						fSample = 1.0f;
					else if(fSample < -1.0f)
						fSample = -1.0f;

					*pfDest++ = fSample;
					*pfDest++ = -fSample; // pseudo stereo
				}

			if(m_Writer.m_bWorking == true)
				m_Writer.Append(pfRecord,nSamples*2); 


			} // if



		} // if processor
	}	// if current patch

	return 0;

}
*/


// MONO
  // called by callback of portaudio
// this delegates to the processor chain
// return 0 on success
INT32 CGASystem::Process(void *pIn, void *pOut, UINT32 nFrames, PaTimestamp dbTime, void* pUser)
{
	static double dbPrevTime = 0.0f;
	double dbElapsedTime =dbTime - dbPrevTime;
	dbPrevTime = dbTime;
/*
// in case of bad loops, limit processing to maxsteps
#ifdef _DEBUG
	static int nPanic = 0;

	if(nPanic > 5000)
		Stop();

	nPanic++;
#endif
*/
	CGAStream* pStream = (CGAStream*)pUser;

	IGAPatch* pPatch = m_Patches.GetCurrent();

	if(pPatch != NULL) {
		IGAProcessors* pProcessors = pPatch->GetProcessors();
		pProcessors->Process(NULL);

		IGAProcessor* pProcessor = pProcessors->GetLast();


		// copy from last processors sample out to output
		FLOAT32* pfDest = (FLOAT32*) pOut;
		FLOAT32* pfRecord = pfDest; 
		FLOAT32* pfIn = pProcessor->GetSignalData(SampleOut) ; 

		for(UINT32 i=0;i < nFrames;i++) {

			FLOAT32 fSample = pfIn[i];
			if(fSample > 1.0f)		// limit
				fSample = 1.0f;
			else if(fSample < -1.0f)
				fSample = -1.0f;

			pfDest[i] = fSample;


		}

		if(m_Writer.m_bWorking == true)
			m_Writer.Append(pfRecord,nFrames); 


	} // if current patch

	return 0;

}



// This routine will be called by the PortAudio engine when audio is needed.
// It may called at interrupt level on some machines so don't do anything
// that could mess up the system like calling malloc() or free().
//
// delegate to class method
// return 0 on success
INT32 CGASystem::PAEngineCallback(void *pIn, void *pOut, unsigned long nFrames,
				PaTimestamp dbTime, void *pUser )
{

	INT32 nResult = g_GASystem.Process(pIn, pOut, nFrames, dbTime, pUser);
	return nResult;
}


UINT32 CGASystem::Command(UINT32 nCmd, FLOAT fValue)
{
/*
	switch (nCmd)
	{
	case Osc_FM:

		break;



	}
	*/
		return(0);

}

// allocate a new signal, wrapping a frame buffer and
// append it to list of signals
IGASignal* CGASystem::CreateSignal(UINT32 nSize)
{
	if(nSize == 0)
		nSize = GetFrameCount(); // use current size

	CGASignal* pSignal = new CGASignal(nSize);


	m_Signals.Append((IGASignal*)pSignal);

	return((IGASignal*)pSignal);
}



UINT32 CGASystem::ReadConfiguration(ATOM_CHAR* pszFileName)
{
	CGAConfigurationReader reader;
	reader.Read(pszFileName);

	return(0);
}




















#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

