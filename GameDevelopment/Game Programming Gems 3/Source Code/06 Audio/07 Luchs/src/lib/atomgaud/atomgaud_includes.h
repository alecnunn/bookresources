// includes 
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


#ifndef _atomgaud_includes_
#define _atomgaud_includes_


#include "..\\atombase\\atombase_includes.h"
#include "atomgaud_defines.h"
#include "portaudio\portaudio_includes.h"



#undef ATOMBASE_DLLCLASS
#ifdef ATOMBASE_EXPORTS
#	define ATOMBASE_DLLCLASS _declspec(dllexport)
#	pragma message("exporting atombase")
#else
#	define ATOMBASE_DLLCLASS _declspec(dllimport)
#	pragma message("importing atombase")
#endif


/*
// class definitions needed by some atomgaud classes
#include "Class.h"
#include "Object.h"
#include "NamedObject.h"
#include "System.h"
*/

#undef ATOMGAUD_DLLCLASS
#ifdef ATOMGAUD_EXPORTS
#	define ATOMGAUD_DLLCLASS _declspec(dllexport)
#	pragma message("exporting atomgaud")
#else
#	define ATOMGAUD_DLLCLASS _declspec(dllimport)
#	pragma message("importing atomgaud")
#endif

#include "atomgaud.h"

#include "GAObject.h"
#include "GADump.h"
#include "GAMath.h"
#include "GADevice.h"
#include "GAStream.h"
#include "GAPin.h"
#include "GAPins.h"
#include "GAProcessor.h"
#include "GAProcessors.h"
#include "GASignalFormat.h"
#include "GASignal.h"
#include "GASignals.h"
#include "GASignalRandomizer.h"
#include "GAConnection.h"
#include "GAConnections.h"
#include "GAPatch.h"
#include "GAPatches.h"
#include "GAFunctionTable.h"
#include "GAGenerator.h"
#include "GARenderer.h"
#include "GAOscillator.h"
#include "GASawtoothOscillator.h"
#include "GASineOscillator.h"
#include "GASquareOscillator.h"
#include "GAWavetableOscillator.h"
#include "GAShapeOscillator.h"
#include "GANoiseGenerator.h"
#include "GASystem.h"
#include "GAButterworthFilter.h"
#include "GAButterworthLPF.h"
#include "GAGain.h"
#include "GAController.h"
#include "GASampleAndHold.h"
#include "GAScaleMapper.h"
#include "GABipolarToUnipolar.h"
#include "GARandom.h"
#include "GARandomGenerator.h"
#include "GAPhaser.h"
#include "GAResonator.h"
#include "GAAbs.h"
#include "GAFile.h"
#include "GAWaveFile.h"
#include "GABalance.h"
#include "GADelay.h"
#include "GALine.h"
#include "GA303Filter.h"
#include "GAImpulseGenerator.h"
#include "GASelector.h"
#include "GASelector2.h"
#include "GAExpCurve.h"
#include "GASawtoothOscDSF.h"
#include "GATriangleOscillator.h"
#include "GACubic.h"
#include "GANoiseParticle.h"
#include "GAEnvelope.h"
#include "GAGaussEnvelope.h"
#include "GATrapezEnvelope.h"
#include "GATriangleEnvelope.h"
#include "GAInverter.h"
#include "GASphereEnvelope.h"
#include "GARampEnvelope.h"
#include "GAPulseOscillator.h"
#include "GAClippedSineOscillator.h"
#include "GASampleWriter.h"
#include "GAKeyboard.h"
#include "GAKeyboardController.h"
#include "GANumPadController.h"
#include "GAGlide.h"
#include "GAAverager.h"
#include "GASequencer16.h"
#include "GASineXOscillator.h"
#include "GASinCosOscillator.h"
#include "GAClipper.h"
#include "GASawtoothOscBLIT.h"
#include "GACrossFade2.h"
#include "GACrossFade3.h"
#include "GAMixer.h"
#include "GAMixer2.h"
#include "GAMixer3.h"
#include "GAMixer4.h"
#include "GAMixer6.h"

#include "GAInterleaver.h"
#include "GARoarOscillator.h"
#include "GANoiseMSineOsc.h"
#include "GAHalfCosineEnvelope.h"
#include "GAThreshold.h"


#include "GAObjectReader.h"
#include "GAConfigurationReader.h"

#include "GAToken.h"
#include "GATokenizer.h"
#include "GAParser.h"







#endif // _atomgaud_includes_