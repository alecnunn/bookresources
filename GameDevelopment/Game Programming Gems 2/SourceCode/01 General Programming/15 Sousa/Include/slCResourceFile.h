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
// Filename:    slCResourceFile.h
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 1:50:37
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
// 20-03-2001 1:50:37 Bruno Sousa (BS)
// File created.
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------------------------------------
#ifndef _slCResourceFile_h_
#define _slCResourceFile_h_


//--------------------------------------------------------------------------------------------------
// Included header files
//--------------------------------------------------------------------------------------------------
	//------------------
	// Seal Lump header file
#include "slCLump.h"

	//------------------
	// C/C++ file stream header file
#include <fstream.h>
	//------------------
	// C/C++ string manipulation header file
#include <string.h>


	//------------------
	// Type of file
enum slLumpFlags
{
	slLumpRaw				          = 0,
	slLumpCompressed			    = 1,
	slLumpEncrypted				    = 2,
	slLumpCompressedEncrypted	= 4
};


//--------------------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------------------
struct slSResourceHeader
{
	char							    acSignature [8];
	unsigned long 				ulVersion;
	unsigned long					ulFlags;
	unsigned long					ulNumberOfLumps;
	unsigned long					ulOffset;
};


struct slSLumpHeader
{
	unsigned long					ulSize;
	char							    acName [256];
	unsigned long					ulFlags;
	unsigned char					byCheckSum;
	unsigned long					ulSizeUncompressed;
};


struct slSLumpWritingHeader
{
	unsigned long					ulSize;
	char							    acName [256];
	char							    acFileName [256*6];
	unsigned long					ulFlags;
	unsigned char					byCheckSum;
};


//--------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Class name:	slCResourceFile
// Author:		Bruno Sousa (BS)
// Created in:	20-03-2001 1:50:37
// Version:		1.00.00
// 
// Description:	#description#
// Remarks:		None.
//--------------------------------------------------------------------------------------------------
class slCResourceFile
{
protected:
		//------------------
		// Protected members

		//------------------
		// Header of resource file, saves us the trouble to look it over and over again in the lump
	unsigned long 				m_ulVersion;
	unsigned long					m_ulFlags;
	unsigned long					m_ulNumberOfLumps;
	unsigned long					m_ulOffset;

		//------------------
		// Directory information
	char							    m_acCurrentDirectory [256];

		//------------------
		// Actual file stream
	ifstream						  m_kFile;

		//------------------
		// Open file with different methods
	void OpenLumpCompressed (slCLump& rkLump, slSLumpHeader sSLumpHeader);
	void OpenLumpEncrypted (slCLump& rkLump, slSLumpHeader sSLumpHeader);
	void OpenLumpCompressedEncrypted (slCLump& rkLump, slSLumpHeader sSLumpHeader);
	void OpenLumpRaw (slCLump& rkLump, slSLumpHeader sSLumpHeader);

public:
		//------------------
		// Public members

		//------------------
		// Constructor(s) / Destructor
	slCResourceFile (void);
	~slCResourceFile (void);

		//------------------
		// Resource file open/close
	bool Open (char * acFilename);
	void Close (void);

		//------------------
		// File open/verification
	void OpenLump (char * acLumpName, slCLump& rkLump);
	bool IsLumpValid (char * acLumpName);

		//------------------
		// Directory manipulation
	void SetCurrentDirectory (char * acDirectory);
	char * GetCurrentDirectory (void);

		//------------------
		// Resource file creation
	static unsigned char* CompressLump (slSLumpWritingHeader kWritingHeader, 
                                      slSLumpHeader& rkLumpHeader);
	static unsigned char* EncryptLump (slSLumpWritingHeader kWritingHeader, 
                                     slSLumpHeader& rkLumpHeader);
	static unsigned char* CompressEncryptLump (slSLumpWritingHeader kWritingHeader, 
                                             slSLumpHeader& rkLumpHeader);

	static void BuildResource (char* pstrFileName, long iNumberFiles, 
                             slSLumpWritingHeader *pkWritingHeader);
};


	//------------------
	// End of file
#endif
