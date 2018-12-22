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
// Filename:    slCLump.cpp
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:57:49
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
// 20-03-2001 2:57:49 Bruno Sousa (BS)
// File created.
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Included header files
//--------------------------------------------------------------------------------------------------
	//------------------
	// Complement header file (slCLump)
#include "slCLump.h"


//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Name:	      slCLump
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:06:39
// Version:     1.00.00
// 
// Description: Default constructor. Just resets the class members.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
slCLump::slCLump (void)
{
	m_ulSize		  = 0;
	m_ulPosition	= 0;
	m_acName[0]		= '\0';
	m_pbyData		  = NULL;
}


//--------------------------------------------------------------------------------------------------
// Name:	      ~slCLump
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:07:00
// Version:     1.00.00
// 
// Description: Default destructor. Cleans up any memory used and resets the class members.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
slCLump::~slCLump (void)
{
	m_ulSize		  = 0;
	m_ulPosition	= 0;
	m_acName[0]		= '\0';

	// if any memory was used free it
	if (NULL != m_pbyData)
	{
		delete [] m_pbyData;
		m_pbyData = NULL;
	}
}


//--------------------------------------------------------------------------------------------------
// Name:	      GetLumpSize
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:11:01
// Version:     1.00.00
// 
// Description: Returns the lump size.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned long slCLump::GetLumpSize (void) const
{
	return m_ulSize;
}


//--------------------------------------------------------------------------------------------------
// Name:	      GetLumpPosition
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:11:11
// Version:     1.00.00
// 
// Description: Returns the lump position.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned long slCLump::GetLumpPosition (void) const
{
	return m_ulPosition;
}


//--------------------------------------------------------------------------------------------------
// Name:	      IsValid
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:11:27
// Version:     1.00.00
// 
// Description: Checks if lump is valid.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
bool slCLump::IsValid (void)
{
	// if there is no data return false
	if (NULL == m_pbyData)
	{
		return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------------------
// Name:	      SetDataPointer
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:22:25
// Version:     1.00.00
// 
// Description: Sets the lump pointer data.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCLump::SetDataPointer (unsigned char * pData)
{
	m_pbyData = pData;
}


//--------------------------------------------------------------------------------------------------
// Name:	      SetFileSize
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:22:42
// Version:     1.00.00
// 
// Description: Sets the lump size for slCResourceFile use.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCLump::SetLumpSize (unsigned long ulSize)
{
	m_ulSize = ulSize;
}


//--------------------------------------------------------------------------------------------------
// Name:	      SetFilePosition
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:22:50
// Version:     1.00.00
// 
// Description: Sets the lump position for slCResourceFile use.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCLump::SetLumpPosition (unsigned long ulPosition)
{
	m_ulPosition = ulPosition;
}


//--------------------------------------------------------------------------------------------------
// Name:	      SetFileName
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:23:06
// Version:     1.00.00
// 
// Description: Sets the lump name for slCResourceFile use.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCLump::SetLumpName (char * acLumpName)
{
	strcpy (m_acName, acLumpName);
}


//--------------------------------------------------------------------------------------------------
// Name:	      Read
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:17:30
// Version:     1.00.00
// 
// Description: Reads a raw chunk of memory from lump.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned long slCLump::Read (unsigned char * abyBuffer, unsigned long dwReadSize)
{
	memcpy (abyBuffer, &m_pbyData [m_ulPosition], dwReadSize);
	m_ulPosition += dwReadSize;

	return m_ulPosition;
}


//--------------------------------------------------------------------------------------------------
// Name:	      Write
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:17:48
// Version:     1.00.00
// 
// Description: Writes a raw chunk of memory to lump.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned long slCLump::Write (unsigned char * abyBuffer, unsigned long dwWriteSize)
{
	memcpy (&m_pbyData [m_ulPosition], abyBuffer, dwWriteSize);
	m_ulPosition += dwWriteSize;

	return m_ulPosition;
}


//--------------------------------------------------------------------------------------------------
// Name:	      Seek
// Parent:	    slCLump
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 3:17:57
// Version:     1.00.00
// 
// Description: Changes the position of the lump marker.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned long slCLump::Seek (unsigned long dwSeekPosition, slLumpSeek eSeekType)
{
	switch (eSeekType)
	{
	case slSeekBegin:
		m_ulPosition = dwSeekPosition;
		break;
	case slSeekCurrent:
		m_ulPosition += dwSeekPosition;
		break;
	case slSeekEnd:
		m_ulPosition = m_ulSize - dwSeekPosition;
		break;
	}

	return m_ulPosition;
}


	//------------------
	// End of file
