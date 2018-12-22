/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
#include <stdio.h>
#include <memory.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

class GameData
{
public:
	bool Save(char *fileName);
	bool Load(char *fileName);
	bool BufferedLoad(char *fileName);
	// Add accessors to get to your game data.
private:
	// Only open one file at a time.
	static FILE *fileDescriptor;
	// Game data goes here.
	int data[1000]; // Replace this with your data format.
};


bool GameData::Save(char *fileName)
{
	fileDescriptor = fopen(fileName, "wb");
	if(fileDescriptor)
	{
		fwrite(this, sizeof(GameData), 1, fileDescriptor);
		fclose(fileDescriptor);
		// Report success writing the file.
		return TRUE;
	}
	else
	{
		// Report an error writing the file.
		return FALSE;
	}
}

bool GameData::Load(char *fileName)
{
	// Open the file for reading.
	fileDescriptor = fopen(fileName, "rb");
	if(fileDescriptor)
	{
		fread(this, sizeof(GameData), 1, fileDescriptor);
		fclose(fileDescriptor);
		// Report success reading the file.
		return TRUE;
	}
	else
	{
		// Report an error reading the file.
		return FALSE;
	}
}

// Check your hardware to see what size of blocks it reads.
// Put that value into this define.
#define READ_GRANULARITY 2048

bool GameData::BufferedLoad(char *fileName)
{
	// Make sure there is room in the read buffer.
	// This could be made smaller to match the
	// known read size by making it a multiple of the
	// READ_GRANULARITY, but this way is a bit faster.
	char *tempBuffer = new char[sizeof(GameData) + READ_GRANULARITY];
	if(!tempBuffer)
	{
		// Could not allocate the buffer.
		// Return an error code.
		return FALSE;
	}
	fileDescriptor = fopen(fileName, "rb");
	if(fileDescriptor)
	{
		fread(tempBuffer, sizeof(GameData), 1, fileDescriptor);
		fclose(fileDescriptor);
		memcpy(this, tempBuffer, sizeof(GameData));
		delete tempBuffer;
		// Report success reading the file.
		return TRUE;
	}
	else
	{
		delete tempBuffer;
		// Report an error reading the file.
		return FALSE;
	}
}

