/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// TextBoxes.h

#ifndef TEXTBOXES_H
#define TEXTBOXES_H

enum {
	OUTPUTMODE_COMMANDS,
	OUTPUTMODE_EVENTS,
	OUTPUTMODE_PITCHES,
	OUTPUTMODE_AMPLITUDES,
	OUTPUTMODE_PANS,
	OUTPUTMODE_VOICES,
	OUTPUTMODE_RELEASES,
	OUTPUTMODE_CPUUSAGE,
	OUTPUTMODE_ERRORS,
	OUTPUTMODE_COUNT
};

// Output Text Box variables

extern bool g_bOutputTextBoxEnabled;
extern int g_nOutputMode;

void TextBoxes_Init();
void TextBoxes_Exit();

void TextBoxes_Keys_Clear();
void TextBoxes_Keys_Printf( char* pFormatString, ... );
void TextBoxes_Credits_Clear();
void TextBoxes_Credits_Printf( char* pFormatString, ... );
void TextBoxes_Stats_Clear();
void TextBoxes_Stats_Printf( char* pFormatString, ... );
void TextBoxes_Output_Clear( int nID );
void TextBoxes_Output_Printf( int nID, char* pFormatString, ... );

void TextBoxes_Draw();
void TextBoxes_DrawFPS( float fps );

#endif