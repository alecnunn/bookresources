/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

#include "StdAfx.h"

#ifdef WIN32
#include "win32.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Def.h"
#include "TextBoxes.h"
#include "MusicSys.h"
#include "SequenceData.h"
#include "InstrumentData.h"

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

bool g_bOutputTextBoxEnabled = true;
int g_nOutputMode = OUTPUTMODE_COMMANDS;
bool g_bPlayTune = false;
int g_nTargetMode = 0;

float g_fps = 0.0f;
float g_lastTime = 0;
int g_count = 0;

bool g_bMSS_Enabled = false;
MusicSequencerSystem_t *g_pMSS;

typedef struct {
	char *pSampleName;
	int nID;
	int nType;
}InstrumentInfo_t;

/*
Tune 1 - Music Sample Info from Dan Forden:

Trk #	Track Name		Sample		   Program Change    Root Key
-------------------------------------------------------------------
1	new chirp		 new chirp.aif     13	             G1
2	chirp			 chirp.aif         2	             G1
3	rev snr			 revsnare.aif      17	             F2
4	rev cym			 revcym.aif	       16                E2
5	bpf (sample hot) bpfpad.aif	       1                 C4
6	cl hat 1		 clhat1.aif	       3                 F#1
7	cl hat 2		 clhat2.aif	       4                 G#1
8	op hat			 ophat.aif	       14                A#1
9	crash			 crash.aif	       5                 C#2
10	lite snare 1	 litesnr1.aif      10	             D4
11	trisnr1			 trisnr1.aif       19	             C3
12	asnr6dt1		 asnr6dt1.aif      0	             C3
13	full kick		 fullkick.aif      6	             C3
14	rthmbdst1		 rthmbpc4.aif      18                C4
15	nar kick		 narkick.aif       12                C3
16	xxl org ch 4 flt xxlorg.aif	       20                C3 
17	lite snare 2	 litesnr2.aif      11	             F#4
18	retro 6			 retro6.aif	       15                C3
19	h drag 1		 hdrag1.aif	       7                 C3
20	h drag 2		 hdrag2.aif	       8                 C3
21	h drag 4		 hdrag4.aif	       9                 C3
*/

const InstrumentInfo_t _aTune1InstrumentInfo[] = {
	{ "samples\\asnr6dt1.aif",   0, 1 },
	{ "samples\\bpfpad.aif",	 1, 1 },
	{ "samples\\chirp.aif",      2, 1 },
	{ "samples\\clhat1.aif",     3, 1 },
	{ "samples\\clhat2.aif",     4, 1 },
	{ "samples\\crash.aif",      5, 1 },
	{ "samples\\fullkick.aif",   6, 0 },
	{ "samples\\hdrag1.aif",     7, 0 },
	{ "samples\\hdrag2.aif",     8, 0 },
	{ "samples\\hdrag4.aif",     9, 0 },
	{ "samples\\litesnr1.aif",  10, 0 },
	{ "samples\\litesnr2.aif",  11, 0 },
	{ "samples\\narkick.aif",   12, 0 },
	{ "samples\\new chirp.aif", 13, 1 },
	{ "samples\\ophat.aif",     14, 1 },
	{ "samples\\retro6.aif",    15, 1 },
	{ "samples\\revcym.aif",    16, 1 },
	{ "samples\\revsnare.aif",  17, 1 },
	{ "samples\\rthmbpc4.aif",  18, 1 },
	{ "samples\\trisnr1.aif",   19, 1 },
	{ "samples\\xxlorg.aif",    20, 1 },
};

const int _nTune1InstrumentInfoCount = sizeof( _aTune1InstrumentInfo ) / sizeof( _aTune1InstrumentInfo[0] );

const char _acTune1Midi[] = "midi\\tune1.mid";


void GameInit()
{
	TextBoxes_Init();

	srand(12345);

	// Enable state for cube (state is preserved by text module)
	glClearColor(0.0, 0.0, 1.0, 0.5);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(33.0, 640.0F/480.0F, 1, 300);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	TextBoxes_Keys_Printf("\nKeys:"
		"\n"
		"\nESC - Quit"
		"\n"
		"\nC - Clear Output"
		"\n"
		"\nM - Switch Output Mode"
		"\n"
		"\nS - Toggle Output State"
		"\n"
		"\nSpacebar - Start / Stop"
	);

	//"\nC - Clear Output Window"
	//"\nPage Up - Page Up Output Window"
	//"\nPage Down - Page Down Output Window"

	TextBoxes_Credits_Printf("\nCredits:"
		"\n\nSequencer Programming:\n  Scott Patterson\n    scottp@tonebyte.com"
		"\n\nSoftSynth Programming:\n  Phil Burk\n    philburk@softsynth.com"
		"\n\nMusic Composition:\n  Dan Forden\n    dforden@yahoo.com"
	);

	TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nInitializing...\n");

	TextBoxes_Stats_Printf("\nStats...\n");

	int i;

	MusicSequencerSystemConfig_t MSSConfig;

	g_pMSS = new MusicSequencerSystem_t();

	if( g_pMSS->Init( MSSConfig ) )
	{
		// load instruments
		for( i=0; i<_nTune1InstrumentInfoCount; i++ )
		{
			// here we transfer from our info struct to the instrument data class
			InstrumentData_t *pInstrumentData = new InstrumentData_t;
			pInstrumentData->SetFileName( String_t(_aTune1InstrumentInfo[i].pSampleName) );
			pInstrumentData->SetType( _aTune1InstrumentInfo[i].nType );
			pInstrumentData->SetID( _aTune1InstrumentInfo[i].nID );
			g_pMSS->LoadInstrument( pInstrumentData );
		}

		// load sequences
		SequenceData_t *pSequenceData = new SequenceData_t;
		pSequenceData->SetFileName( String_t(_acTune1Midi) );
		pSequenceData->SetID( 1 );
		g_pMSS->LoadSequence( pSequenceData );

		TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nSystem is Up...\n");

		TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nPress Spacebar to hear the tune...\n");


		g_bMSS_Enabled = true;
	} else {
		TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nSystem Init Failure...\n");

		g_bMSS_Enabled = false;
	}
}

void GameExit()
{
	if( g_bMSS_Enabled )
	{
		g_pMSS->Shutdown();
	}
}

void GameFrame()
{
	if( g_bMSS_Enabled )
	{
		g_pMSS->AudioFrame();

		TextBoxes_Output_Printf( OUTPUTMODE_CPUUSAGE, "\nCSyn CPU Usage = %.2f%%%%", (g_pMSS->GetCSynUsage() * 100.0) );

		int i;
		// Capture Keyboard
		for (i = 0; i < winKey.downCnt; i++) {
			switch(winKey.down[i]) {
			case 'C':
				TextBoxes_Output_Clear( g_nOutputMode );
			break;
			case 'M':
				g_nOutputMode++;
				if( g_nOutputMode >= OUTPUTMODE_COUNT )
				{
					g_nOutputMode = 0;
				}
			break;
			case 'S':
				g_bOutputTextBoxEnabled = !g_bOutputTextBoxEnabled;
			break;
			case VK_SPACE:
				g_bPlayTune = !g_bPlayTune;
				if( g_bPlayTune )
				{
					TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nPlay Sequence...\n");
					g_pMSS->PlaySequence( 1 );
				} else {
					TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nStop Sequence...\n");
					g_pMSS->StopSequences();
				}
			break;
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw any other OpenGL stuff here...

	// Draw Text
	TextBoxes_Draw();

	TextBoxes_DrawFPS( g_fps );

	glFinish();

	// calculate FPS
	float currTime;
	currTime = GetSysTime();
	if (g_count++ % 32 == 0) {
		g_fps = 32.0f / (currTime - g_lastTime);
		g_lastTime = currTime;
	}
}

void GameIdle()
{
}

