/* Copyright (C) Bruno Sousa, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruno Sousa, 2001"
 */
//--------------------------------------------------------------------------------------------------
// Filename:    slCLump.h
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 1:42:21
// Version:     1.00.00
// 
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
// You can use this file free of charge. You are free to modify it in anyway you want, redistribute
// it. Author takes no responsability on any damage resulting directly or indirectly by the use of
// this or any other related files.
//
// Bruno Sousa (BS)	(bsousa@fireworks-interactive.com)
// http://www.fireworks-interactive.com
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// 20-03-2001 1:42:21 Bruno Sousa (BS)
// File created.
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------------------------------------
#ifndef _slCLump_h_
#define _slCLump_h_

	//------------------
	// Seek type
enum slLumpSeek
{
	slSeekBegin				    = 0,
	slSeekCurrent				= 1,
	slSeekEnd					= 2,
};


//--------------------------------------------------------------------------------------------------
// Included header files
//--------------------------------------------------------------------------------------------------
	//------------------
	// C/C++ memory header file
#include <memory.h>
	//------------------
	// C/C++ standard library header file
#include <stdlib.h>
	//------------------
	// C/C++ string manipulation header file
#include <string.h>


//--------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Class name:	slCLump
// Author:		Bruno Sousa (BS)
// Created in:	20-03-2001 1:42:21
// Version:		1.00.00
// 
// Description:	The actual lump in memory. It's from this class that we manipulate the lump. The use
//				of this class is mostly to provide a layer of abstraction from the actual memory 
//				manipulation that we need.
//				
// Remarks:		None.
//--------------------------------------------------------------------------------------------------
class slCLump
{
private:
		//------------------
		// Private members

protected:
		//------------------
		// Protected members
	unsigned long					m_ulSize;
	unsigned long					m_ulPosition;
	char							m_acName [256];
	unsigned char *					m_pbyData;


public:
		//------------------
		// Public members

		//------------------
		// Constructor(s) / Destructor
	slCLump (void);
	~slCLump (void);

		//------------------
		// Retrival/status checker operations
	unsigned long GetLumpSize (void) const;
	unsigned long GetLumpPosition (void) const;
	char * GetLumpName (void) const;
	bool IsValid (void);

		//------------------
		// Set lump's attributes (to be used by slCResourceFile)
	void SetDataPointer (unsigned char * pData);
	void SetLumpSize (unsigned long ulSize);
	void SetLumpPosition (unsigned long ulPosition);
	void SetLumpName (char * acLumpName);

		//------------------
		// Actual lump manipulation
	unsigned long Read (unsigned char * abyBuffer, unsigned long dwReadSize);
	unsigned long Write (unsigned char * abyBuffer, unsigned long dwWriteSize);
	unsigned long Seek (unsigned long dwSeekPosition, slLumpSeek slLumpSeek);
};


	//------------------
	// End of file
#endif
