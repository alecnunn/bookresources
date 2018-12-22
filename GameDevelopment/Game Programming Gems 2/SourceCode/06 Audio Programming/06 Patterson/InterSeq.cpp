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
int g_nTargetState = 0;

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
Tune 2 - Music Sample Info from Dan Forden:

Trk #	Track Name		Sample File		     Program Change     Root Key
-------------------------------------------------------------------------
1	tuneA slovox		t1slovoxc3.aif		 0                 c3
2	tuneA slovox		t1slovoxc5.aif		 20                c5
3	tuneA k2ehat1		t1k2ehat1.aif		 1                 c3
4	tuneA k2ehat2		t1k2ehat2.aif		 2                 c3
5	tuneA k2ehat3		t1k2ehat3.aif		 3                 c3
6	tuneA rkckvrb2		t1rkckvrb2g2.aif	 4                 g2
7	tuneA asnr21dt		t1asnr21dt.aif		 5                 c3
8	tuneA asnr31v1		t1asnr31v1.aif		 6                 c3
9	tuneA asnr22v1		t1asnr22v1.aif		 7                 c3
10	tuneA rthmbpdt		t1rthmbpdtc3.aif     8                 c3
11	tuneA rthmbpdt      t1rthmbpdtc4.aif	 21                c4
12	tuneA abass3		t1abass3.aif		 9                 c3
13	tuneA crash		    t1crash.aif		     10                c3
14	tuneB glass		    t2glassc4.aif		 11                c4
15	tuneB glass	        t2glassc5.aif		 22                c5
16	tuneB k2ehat1		t1k2ehat1.aif		 1                 c3
17	tuneB k2ehat2		t1k2ehat2.aif		 2                 c3
18	tuneB k2ehatsnr		t1k2ehat3.aif		 3                 c3
19	tuneB clave		    t2clavec5.aif		 12                c5
20	tuneB woody low		t2woodylowb3.aif	 13                b3
21	tuneB woody lower	t2woodylowerf3.aif	 14                f3
22	tuneB woody wood	t2woodywoode4.aif	 15                e4
23	tuneB asnr31v1		t1asnr31v1.aif		 6                 c3
24	tuneB asnr22v1		t1asnr22v1.aif		 7                 c3
25	tuneB logbass		t2logbassc3.aif		 16                c3
				        t2logbassc2.aif		   c2 (not used)
26	tuneB big log		t2biglogc3.aif		 17                c3
				        t2biglogc2.aif		   c2 (not used)
27	tuneB rama		    t2rama.aif		     18                c3
28	tuneB gong (hot)	t2gong.aif		     19                c3
29	tuneA slovox		t1slovoxc3.aif		 0                 c3 split from track 1
*/

const InstrumentInfo_t _aTune2InstrumentInfo[] = {
	{ "samples\\t1slovoxc3.aif",     0, 1 },
	{ "samples\\t1slovoxc5.aif",     20, 1 },
	{ "samples\\t1k2ehat1.aif",      1, 0 },
	{ "samples\\t1k2ehat2.aif",      2, 0 },
	{ "samples\\t1k2ehat3.aif",      3, 0 },
	{ "samples\\t1rkckvrb2g2.aif",   4, 1 },
	{ "samples\\t1asnr21dt.aif",     5, 1 },
	{ "samples\\t1asnr22v1.aif",     6, 1 },
	{ "samples\\t1asnr31v1.aif",     7, 1 },
	{ "samples\\t1rthmbpdtc4.aif",   8, 1 },
	{ "samples\\t1rthmbpdtc3.aif",   21, 1 },
	{ "samples\\t1abass3.aif",       9,  1 },
	{ "samples\\t1crash.aif",        10, 1 },
	{ "samples\\t2glassc5.aif",      11, 1 },
	{ "samples\\t2newglassc3.aif",   22, 1 },
	{ "samples\\t2clavec5.aif",      12, 0 },
	{ "samples\\t2woodylowb3.aif",   13, 1 },
	{ "samples\\t2woodylowerf3.aif", 14, 1 },
	{ "samples\\t2woodywoode4.aif",  15, 1 },
	{ "samples\\t2logbassc3.aif",    16, 1 },
	{ "samples\\t2biglogc3.aif",     17, 1 },
	{ "samples\\t2rama.aif",         18, 1 },
	{ "samples\\t2gong.aif",         19, 1 },
};

const int _nTune2InstrumentInfoCount = sizeof( _aTune2InstrumentInfo ) / sizeof( _aTune2InstrumentInfo[0] );

const char _acTune2Midi[] = "midi\\tune2.mid";

void SetupMixDefinition();
IntVectorVector_t TVVV;

void GameInit()
{
	int i;

	// define mixes
	SetupMixDefinition();

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
		"\n"
		"\n1 - Target State 1"
		"\n"
		"\n2 - Target State 2"
		"\n"
		"\n3 - Target State 3"
		"\n"
		"\n4 - Target State 4"
		"\n"
		"\n5 - Target State 5"
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

	MusicSequencerSystemConfig_t MSSConfig;
	// add more voices for this tune
	MSSConfig.SetVoiceCount( 1, 40 );

	g_pMSS = new MusicSequencerSystem_t();

	if( g_pMSS->Init( MSSConfig ) )
	{
		// load instruments
		for( i=0; i<_nTune2InstrumentInfoCount; i++ )
		{
			// here we transfer from our info struct to the instrument data class
			InstrumentData_t *pInstrumentData = new InstrumentData_t;
			pInstrumentData->SetFileName( String_t(_aTune2InstrumentInfo[i].pSampleName) );
			pInstrumentData->SetType( _aTune2InstrumentInfo[i].nType );
			pInstrumentData->SetID( _aTune2InstrumentInfo[i].nID );
			g_pMSS->LoadInstrument( pInstrumentData );
		}

		// modify the instrument 0 envelope
		g_pMSS->SetSlowInstrumentEnvelope( 0 );
		g_pMSS->SetSlowInstrumentEnvelope( 20 );

		// load sequences
		SequenceData_t *pSequenceData = new SequenceData_t;
		pSequenceData->SetFileName( String_t(_acTune2Midi) );
		pSequenceData->SetID( 1 );
		g_pMSS->LoadSequence( pSequenceData );
		g_pMSS->SetSequenceMixes( pSequenceData, &TVVV );

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
					TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nPlay Sequence at Target State %d...\n", g_nTargetState + 1 );
					g_pMSS->PlaySequence( 1, g_nTargetState );
				} else {
					TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nStop Sequence...\n");
					g_pMSS->StopSequences();
				}
			break;
			case '1':
				TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nUpdate Sequence to Target State 1...\n");
				g_nTargetState = 0;
				g_pMSS->UpdateSequenceMix( 1, g_nTargetState );
			break;
			case '2':
				TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nUpdate Sequence to Target State 2...\n");
				g_nTargetState = 1;
				g_pMSS->UpdateSequenceMix( 1, g_nTargetState );
			break;
			case '3':
				TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nUpdate Sequence to Target State 3...\n");
				g_nTargetState = 2;
				g_pMSS->UpdateSequenceMix( 1, g_nTargetState );
			break;
			case '4':
				TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nUpdate Sequence to Target State 4...\n");
				g_nTargetState = 3;
				g_pMSS->UpdateSequenceMix( 1, g_nTargetState );
			break;
			case '5':
				TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nUpdate Sequence to Target State 5...\n");
				g_nTargetState = 4;
				g_pMSS->UpdateSequenceMix( 1, g_nTargetState );
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

// the mix definition defines the volumes of each track for each target state
void SetupMixDefinition()
{
	TVVV.resize( 5 );

	TVVV[0].resize( 29 );
	TVVV[0][0]  =   0;  //	tuneA slovox
	TVVV[0][1]  =   0;  //	tuneA slovox
	TVVV[0][2]  =   0;  //	tuneA k2ehat1
	TVVV[0][3]  =   0;  //	tuneA k2ehat2
	TVVV[0][4]  =   0;  //	tuneA k2ehat3
	TVVV[0][5]  =   0;  //	tuneA rkckvrb2
	TVVV[0][6]  =   0;  //	tuneA asnr21dt
	TVVV[0][7]  =   0;  //	tuneA asnr31v1
	TVVV[0][8]  =   0;  //	tuneA asnr22v1
	TVVV[0][9]  =   0;  //	tuneA rthmbpdt
	TVVV[0][10] =   0; // 	tuneA rthmbpdt
	TVVV[0][11] =   0; //	tuneA abass3
	TVVV[0][12] =   0;  //	tuneA crash
	TVVV[0][13] = 112;  //	tuneB glass
	TVVV[0][14] = 112;  //	tuneB glass
	TVVV[0][15] = 127;  //	tuneB k2ehat1
	TVVV[0][16] = 127;  //	tuneB k2ehat2
	TVVV[0][17] = 127;  //	tuneB k2ehatsnr
	TVVV[0][18] = 127;  //	tuneB clave
	TVVV[0][19] = 127;  //	tuneB woody low
	TVVV[0][20] = 127;  //	tuneB woody lower
	TVVV[0][21] = 127;  //	tuneB woody wood
	TVVV[0][22] = 127;  //	tuneB asnr31v1
	TVVV[0][23] = 127;  //	tuneB asnr22v1
	TVVV[0][24] = 127;  //	tuneB logbass
	TVVV[0][25] = 127;  //	tuneB big log
	TVVV[0][26] = 127;  //	tuneB rama
	TVVV[0][27] = 127;  //	tuneB gong (hot)
	TVVV[0][28] =   0;  //	tuneA slovox

	TVVV[1].resize( 29 );
	TVVV[1][0]   =   0;  //	tuneA slovox
	TVVV[1][1]   =   0;  //	tuneA slovox
	TVVV[1][2]   =  64;  //	tuneA k2ehat1
	TVVV[1][3]   =   0;  //	tuneA k2ehat2
	TVVV[1][4]   =  64;  //	tuneA k2ehat3
	TVVV[1][5]   =   0;  //	tuneA rkckvrb2
	TVVV[1][6]   =   0;  //	tuneA asnr21dt
	TVVV[1][7]   =   0;  //	tuneA asnr31v1
	TVVV[1][8]   =   0;  //	tuneA asnr22v1
	TVVV[1][9]   =   0;  //	tuneA rthmbpdt
	TVVV[1][10]  =   0;  //	tuneA rthmbpdt
	TVVV[1][11]  =   0;  //	tuneA abass3
	TVVV[1][12]  =   0;  //	tuneA crash
	TVVV[1][13]  = 112;  //	tuneB glass
	TVVV[1][14]  = 112;  //	tuneB glass
	TVVV[1][15]  = 127;  //	tuneB k2ehat1
	TVVV[1][16]  = 127;  //	tuneB k2ehat2
	TVVV[1][17]  = 127;  //	tuneB k2ehatsnr
	TVVV[1][18]  =  64;  //	tuneB clave
	TVVV[1][19]  = 127;  //	tuneB woody low
	TVVV[1][20]  = 127;  //	tuneB woody lower
	TVVV[1][21]  =  64;  //	tuneB woody wood
	TVVV[1][22]  = 127;  //	tuneB asnr31v1
	TVVV[1][23]  = 127;  //	tuneB asnr22v1
	TVVV[1][24]  = 127;  //	tuneB logbass
	TVVV[1][25]  = 127;  //	tuneB big log
	TVVV[1][26]  = 127;  //	tuneB rama
	TVVV[1][27]  = 127;  //	tuneB gong (hot)
	TVVV[1][28]  =   0;  //	tuneA slovox

	TVVV[2].resize( 29 );
	TVVV[2][0]   =   0;  //	tuneA slovox
	TVVV[2][1]   =   0;  //	tuneA slovox
	TVVV[2][2]   =  64;  //	tuneA k2ehat1
	TVVV[2][3]   =   0;  //	tuneA k2ehat2
	TVVV[2][4]   =  64;  //	tuneA k2ehat3
	TVVV[2][5]   =   0;  //	tuneA rkckvrb2
	TVVV[2][6]   =   0;  //	tuneA asnr21dt
	TVVV[2][7]   =   0;  //	tuneA asnr31v1
	TVVV[2][8]   =   0;  //	tuneA asnr22v1
	TVVV[2][9]   = 127;  //	tuneA rthmbpdt
	TVVV[2][10]  = 127; // 	tuneA rthmbpdt
	TVVV[2][11]  = 127;  //	tuneA abass3
	TVVV[2][12]  = 127;  //	tuneA crash
	TVVV[2][13]  = 112;  //	tuneB glass
	TVVV[2][14]  = 112;  //	tuneB glass
	TVVV[2][15]  =   0;  //	tuneB k2ehat1
	TVVV[2][16]  =   0;  //	tuneB k2ehat2
	TVVV[2][17]  = 127;  //	tuneB k2ehatsnr
	TVVV[2][18]  =   0;  //	tuneB clave
	TVVV[2][19]  =   0;  //	tuneB woody low
	TVVV[2][20]  =   0;  //	tuneB woody lower
	TVVV[2][21]  =   0;  //	tuneB woody wood
	TVVV[2][22]  = 127;  //	tuneB asnr31v1
	TVVV[2][23]  =   0;  //	tuneB asnr22v1
	TVVV[2][24]  = 127;  //	tuneB logbass
	TVVV[2][25]  = 127;  //	tuneB big log
	TVVV[2][26]  =   0;  //	tuneB rama
	TVVV[2][27]  = 127;  //	tuneB gong (hot)
	TVVV[2][28]  =   0;  //	tuneA slovox

	TVVV[3].resize( 29 );
	TVVV[3][0]   =  60;  //	tuneA slovox
	TVVV[3][1]   =  50;  //	tuneA slovox
	TVVV[3][2]   = 127;  //	tuneA k2ehat1
	TVVV[3][3]   = 127;  //	tuneA k2ehat2
	TVVV[3][4]   = 127;  //	tuneA k2ehat3
	TVVV[3][5]   = 127;  //	tuneA rkckvrb2
	TVVV[3][6]   = 127;  //	tuneA asnr21dt
	TVVV[3][7]   = 127;  //	tuneA asnr31v1
	TVVV[3][8]   =   0;  //	tuneA asnr22v1
	TVVV[3][9]   = 127;  //	tuneA rthmbpdt
	TVVV[3][10]  = 127; // 	tuneA rthmbpdt
	TVVV[3][11]  = 127;  //	tuneA abass3
	TVVV[3][12]  = 127;  //	tuneA crash
	TVVV[3][13]  =  90;  //	tuneB glass
	TVVV[3][14]  =  90;  //	tuneB glass
	TVVV[3][15]  =   0;  //	tuneB k2ehat1
	TVVV[3][16]  =   0;  //	tuneB k2ehat2
	TVVV[3][17]  =   0;  //	tuneB k2ehatsnr
	TVVV[3][18]  = 127;  //	tuneB clave
	TVVV[3][19]  =   0;  //	tuneB woody low
	TVVV[3][20]  =   0;  //	tuneB woody lower
	TVVV[3][21]  =   0;  //	tuneB woody wood
	TVVV[3][22]  =   0;  //	tuneB asnr31v1
	TVVV[3][23]  =   0;  //	tuneB asnr22v1
	TVVV[3][24]  =   0;  //	tuneB logbass
	TVVV[3][25]  = 127;  //	tuneB big log
	TVVV[3][26]  = 127;  //	tuneB rama
	TVVV[3][27]  = 127;  //	tuneB gong (hot)
	TVVV[3][28]  =  60;  //	tuneA slovox

	TVVV[4].resize( 29 );
	TVVV[4][0]   = 112;  //	tuneA slovox
	TVVV[4][1]   =  90;  //	tuneA slovox
	TVVV[4][2]   = 127;  //	tuneA k2ehat1
	TVVV[4][3]   = 127;  //	tuneA k2ehat2
	TVVV[4][4]   = 127;  //	tuneA k2ehat3
	TVVV[4][5]   = 127;  //	tuneA rkckvrb2
	TVVV[4][6]   = 127;  //	tuneA asnr21dt
	TVVV[4][7]   = 127;  //	tuneA asnr31v1
	TVVV[4][8]   = 127;  //	tuneA asnr22v1
	TVVV[4][9]   = 127;  //	tuneA rthmbpdt
	TVVV[4][10]  = 127; // 	tuneA rthmbpdt
	TVVV[4][11]  = 127;  //	tuneA abass3
	TVVV[4][12]  = 127;  //	tuneA crash
	TVVV[4][13]  =   0;  //	tuneB glass
	TVVV[4][14]  =   0;  //	tuneB glass
	TVVV[4][15]  =   0;  //	tuneB k2ehat1
	TVVV[4][16]  =   0;  //	tuneB k2ehat2
	TVVV[4][17]  =   0;  //	tuneB k2ehatsnr
	TVVV[4][18]  =   0;  //	tuneB clave
	TVVV[4][19]  =   0;  //	tuneB woody low
	TVVV[4][20]  =   0;  //	tuneB woody lower
	TVVV[4][21]  =   0;  //	tuneB woody wood
	TVVV[4][22]  =   0;  //	tuneB asnr31v1
	TVVV[4][23]  =   0;  //	tuneB asnr22v1
	TVVV[4][24]  =   0;  //	tuneB logbass
	TVVV[4][25]  =   0;  //	tuneB big log
	TVVV[4][26]  =   0;  //	tuneB rama
	TVVV[4][27]  =   0;  //	tuneB gong (hot)
	TVVV[4][28]  = 112;  //	tuneA slovox
}