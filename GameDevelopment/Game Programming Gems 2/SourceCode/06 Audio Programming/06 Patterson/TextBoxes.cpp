/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// TextBoxes.cpp

#include "StdAfx.h"
#include "TextBoxes.h"
#include "Text.h"

static TextBox *g_pKeysTextBox;
static TextBox *g_pCreditsTextBox;
static TextBox *g_pStatsTextBox;
static TextBox *g_pOutputTextBox[OUTPUTMODE_COUNT];

static char *aOutputTitles[OUTPUTMODE_COUNT] = {
	"Commands", //OUTPUTMODE_COMMANDS,
	"Events", //OUTPUTMODE_EVENTS,
	"Pitches", //OUTPUTMODE_PITCHES,
	"Amplitudes", //OUTPUTMODE_AMPLITUDES,
	"Pans", //OUTPUTMODE_PANS,
	"Voices", //OUTPUTMODE_VOICES,
	"Releases", //OUTPUTMODE_RELEASES,
	"CSynUsage", //OUTPUTMODE_CPUUSAGE,
	"Errors", //OUTPUTMODE_ERRORS,
};

void TextBoxes_Init()
{
	int iXStart = 0;
	int iXMiddle1 = 380;
	int iXMiddle2 = 220;
	int iXStop = 640;
	int iYStart = 0;
	int iYMiddle = 180;
	int iYStop = 480;

	int iBorder = 10;

	// upper left box
	g_pStatsTextBox = new TextBox(iXStart + iBorder, iXMiddle1 - iBorder, iYStart + iBorder, iYMiddle - iBorder);
	g_pStatsTextBox->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_BOX);

	// upper right box
	g_pCreditsTextBox = new TextBox(iXMiddle1, iXStop - iBorder, iYStart + iBorder, iYMiddle - iBorder);
	//g_pCreditsTextBox->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_BOX);

	// lower left box
	g_pKeysTextBox = new TextBox(iXStart + iBorder, iXMiddle2 - iBorder, iYMiddle, iYStop - iBorder);
	g_pKeysTextBox->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_BOX);

	for( int i=0; i<OUTPUTMODE_COUNT; i++ )
	{
		// lower right box
		g_pOutputTextBox[i] = new TextBox(iXMiddle2, iXStop - iBorder, iYMiddle, iYStop - iBorder);
		g_pOutputTextBox[i]->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_BOX);
	}

}

void TextBoxes_Exit()
{
	delete g_pKeysTextBox;
	delete g_pCreditsTextBox;
	delete g_pStatsTextBox;
	for( int i=0; i<OUTPUTMODE_COUNT; i++ )
	{
		delete g_pOutputTextBox[i];
	}
}

void TextBoxes_Keys_Clear()
{
	g_pKeysTextBox->Clear();
}

void TextBoxes_Keys_Printf( char* pFormatString, ... )
{
	if( g_pKeysTextBox )
	{
		va_list varglist;
		char buff[MAX_STR];
		
		va_start( varglist, pFormatString );
		vsprintf(buff, pFormatString, varglist);
		g_pKeysTextBox->Printf( buff );
		va_end( varglist );
	}
}

void TextBoxes_Credits_Clear()
{
	g_pCreditsTextBox->Clear();
}

void TextBoxes_Credits_Printf( char* pFormatString, ... )
{
	if( g_pCreditsTextBox )
	{
		va_list varglist;
		char buff[MAX_STR];
		
		va_start( varglist, pFormatString );
		vsprintf(buff, pFormatString, varglist);
		g_pCreditsTextBox->Printf( buff );
		va_end( varglist );
	}
}

void TextBoxes_Stats_Clear()
{
	g_pStatsTextBox->Clear();
}

void TextBoxes_Stats_Printf( char* pFormatString, ... )
{
	if( g_pStatsTextBox )
	{
		va_list varglist;
		char buff[MAX_STR];
		
		va_start( varglist, pFormatString );
		vsprintf(buff, pFormatString, varglist);
		g_pStatsTextBox->Printf( buff );
		va_end( varglist );
	}
}

void TextBoxes_Output_Clear( int nID )
{
	g_pOutputTextBox[nID]->Clear();
}

void TextBoxes_Output_Printf( int nID, char* pFormatString, ... )
{
	if( !g_bOutputTextBoxEnabled )
	{
		return;
	}

	if( g_pOutputTextBox )
	{
		va_list varglist;
		char buff[MAX_STR];
		
		va_start( varglist, pFormatString );
		vsprintf(buff, pFormatString, varglist);
		g_pOutputTextBox[nID]->Printf( buff );
		va_end( varglist );
	}
}

void TextBoxes_Draw()
{
	if( g_pKeysTextBox )
	{
		g_pKeysTextBox->Draw();
	}
	if( g_pCreditsTextBox )
	{
		g_pCreditsTextBox->Draw();
	}
	if( g_pStatsTextBox )
	{
		g_pStatsTextBox->Draw();
	}
	if( g_pOutputTextBox )
	{
		g_pOutputTextBox[g_nOutputMode]->Draw();
	}
}

void TextBoxes_DrawFPS( float fps )
{
	static char g_string[64];

	if( g_pCreditsTextBox )
	{
		// Draw FPS
		g_pCreditsTextBox->Begin();

		sprintf(g_string, "FPS: %0.1f", fps);
		g_pCreditsTextBox->DrawStr(550, 4, g_string);

		char *p = g_bOutputTextBoxEnabled ? "Enabled" : "Disabled";
		sprintf(g_string, "Output State: %s - Output Mode: %s", p, aOutputTitles[g_nOutputMode]);
		g_pCreditsTextBox->DrawStr(235, 170, g_string);

		g_pCreditsTextBox->End();
	}
}


