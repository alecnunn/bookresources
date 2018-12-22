// defines 
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


#ifndef _atomgaud_defines_
#define _atomgaud_defines_


#define GADUMP	atomos::g_GASystem.m_Dump.Write
#define ATT_MAX	10 // max fm modulation = +- ATT_MAX octaves


#define MID_ATOMGAUD			256*88
typedef enum eGEMSAUDIO_CID
{
	
	CID_GASystem=MID_ATOMGAUD,
	CID_GAObject,
	CID_GADump,
	CID_GADevice,
	CID_GAProcessor,
	CID_GAProcessors,
	CID_GAStream,
	CID_GARenderer,
	CID_GAGenerator,
	CID_GAConnection,

	CID_GAConnections, // 10
	CID_GASignal,
	CID_GASignals,
	CID_GASignalRandomizer,
	CID_GAOscillator,
	CID_GASineOscillator,
	CID_GAClippedSineOscillator,
	CID_GASawtoothOscillator,
	CID_GASquareOscillator,
	CID_GAWavetableOscillator,

	CID_GAShapeOscillator, // 20
	CID_GAPatch,
	CID_GAPatches,
	CID_GAPin,
	CID_GAPins,
	CID_GAButterworthFilter,
	CID_GAButterworthLPF,
	CID_GAGain,
	CID_GAController,
	CID_GASampleAndHold,

	CID_GAScaleMapper, // 30
	CID_GABipolarToUnipolar,
	CID_GARandom,
	CID_GARandomGenerator,
	CID_GAPhaser,
	CID_GAResonator,
	CID_GAAbs,
	CID_GAFile,
	CID_GAWaveFile,
	CID_GABalance,

	CID_GADelay, // 40
	CID_GALine,
	CID_GA303Filter,
	CID_GAMath,
	CID_GASelector,
	CID_GASelector2,
	CID_GAImpulseGenerator,
	CID_GAExpCurve,
	CID_GASawtoothOscDSF,
	CID_GATriangleOscillator,
	CID_GACubic,

	CID_GANoiseParticle, // 50
	CID_GANoiseGenerator,
	CID_GAEnvelope,
	CID_GAGaussEnvelope,
	CID_GATrapezEnvelope,
	CID_GATriangleEnvelope,
	CID_GAMixer3,
	CID_GAMixer4,
	CID_GAInverter,
	CID_GASphereEnvelope,

	CID_GARampEnvelope, // 60
	CID_GAPulseOscillator,
	CID_GASampleWriter,
	CID_GAKeyboard,
	CID_GAKeyboardController,
	CID_GAGlide,
	CID_GAInterleaver,
	CID_GAObjectReader,
	CID_GAConfigurationReader,
	CID_GAToken,

	CID_GATokenizer,	//70
	CID_GAParser,
	CID_GAAverager,
	CID_GASequencer16,
	CID_GASineXOscillator,
	CID_GASinCosOscillator,
	CID_GAClipper,
	CID_GANumPadController,
	CID_GASawtoothOscBLIT,
	CID_GAFunctionTable,


	CID_GACrossFade2, // 80
	CID_GACrossFade3, 
	CID_GAMixer2,
	CID_GARoarOscillator,
	CID_GAMixer,
	CID_GAMixer6,
	CID_GASignalFormat,
	CID_GANoiseMSineOsc,
	CID_GAHalfCosineEnvelope,
	CID_GAThreshold,


} GEMSAUDIO_CID;



// predefined type ids
// also defines direction: n >= SampleOut defines out pin
typedef enum ePinType
{
	SampleIn=0,
	SampleThru=SampleIn,
	SampleIn1=SampleIn+0,
	SampleIn2=SampleIn+1,
	SampleIn3=SampleIn+2,
	SampleIn4=SampleIn+3,
	SampleIn5=SampleIn+4,
	SampleIn6=SampleIn+5,
	SampleIn7=SampleIn+6,
	SampleIn8=SampleIn+7,
	ControlIn=SampleIn+16,
	ControlIn1=ControlIn+0,
	ControlIn2=ControlIn+1,
	ControlIn3=ControlIn+2,
	ControlIn4=ControlIn+3,
	FM =SampleIn+16,
	AM =SampleIn+32,
	Phase =SampleIn+40,
	Balance=SampleIn+48,
	BalanceU=SampleIn+49,
	Delay=SampleIn+56,
	SampleTime=SampleIn+64,
	HoldTime=SampleIn+72,
	TRIG = SampleIn+80,
	Cutoff = SampleIn+88,
	Harmonics = SampleIn+96,
	Center = SampleIn+104,
	StartTime = SampleIn+112,
	Duration = SampleIn+120,
	PulseWidth = SampleIn+128,
	Frequency =  SampleIn+136,
	Slope  =  SampleIn+144,
	Amplitude  =  SampleIn+156,
	Clipping  =  SampleIn+164,


	SampleOut=256,
	SampleOut1=SampleOut+0,
	SampleOut2=SampleOut+1,
	SampleOut3=SampleOut+2,
	SampleOut4=SampleOut+3,
	ControlOut=SampleOut+256,
	DeltaOut=SampleOut+512,


} PinType;



// ATOM_WAVEFORMATEX has same names and layout as microsoft WAVEFORMATEX
typedef struct structAtomWaveFormatEx
{
	UINT16  wFormatTag; 
    UINT16  nChannels; 
    UINT32  nSamplesPerSec; 
    UINT32  nAvgBytesPerSec; 
    UINT16  nBlockAlign; 
    UINT16  wBitsPerSample; 
    UINT16  cbSize; 

} ATOM_WAVEFORMATEX;


// simple wave file header, 44 bytes
typedef struct structAtomWaveFileHeader
{
	CHAR8	tagRiff[4];
	UINT32	nRiffSize;
	CHAR8	tagWave[4];
	CHAR8	tagfmt[4];
	UINT32	nFormatSize;

	UINT16  wFormatTag; 
    UINT16  nChannels; 
    UINT32  nSamplesPerSec; 
    UINT32  nAvgBytesPerSec; 
    UINT16  nBlockAlign; 
    UINT16  wBitsPerSample; 

	CHAR8	tagdata[4];
	UINT32	nDataSize;


} ATOM_WAVEFILEHEADER;




#endif // _atomgaud_defines_
