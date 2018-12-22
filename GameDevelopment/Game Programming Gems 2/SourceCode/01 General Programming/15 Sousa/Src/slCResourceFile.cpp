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
// Filename:    slCResourceFile.cpp
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:00:54
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
// 20-03-2001 2:00:54 Bruno Sousa (BS)
// File created.
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Included header files
//--------------------------------------------------------------------------------------------------
	//------------------
	// Complement header file (slCResourceFile)
#include "slCResourceFile.h"


//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Name:	      slCResourceFile
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:25:59
// Version:     1.00.00
// 
// Description: Default constructor. Just resets the class members.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
slCResourceFile::slCResourceFile (void)
{
	m_ulVersion			       	= 0;
	m_ulFlags			        	= 0;
	m_ulNumberOfLumps   		= 0;
	m_ulOffset			      	= 0;
	m_acCurrentDirectory[0] = '\0';
}


//--------------------------------------------------------------------------------------------------
// Name:	      ~slCResourceFile
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:26:27
// Version:     1.00.00
// 
// Description: Default destructor. Just resets the class members.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
slCResourceFile::~slCResourceFile (void)
{ 
	m_ulVersion		      		= 0;
	m_ulFlags			        	= 0;
	m_ulNumberOfLumps	    	= 0;
	m_ulOffset		      		= 0;
	m_acCurrentDirectory[0] = '\0';
}


//--------------------------------------------------------------------------------------------------
// Name:	      Open
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:27:41
// Version:     1.00.00
// 
// Description: Opens the resource file. Read header and checks the signature. Returns true on 
//			      	success, or false if any error ocurred.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
bool slCResourceFile::Open (char * acLumpName)
{
	// if file is already open, return false
	if (m_kFile.is_open ())
	{
		return false;
	}
	else
	{
		// attempt to open file
    m_kFile.open (acLumpName, ios::in | ios::binary);
	}
	// all went well, return true
	return true;
}


//--------------------------------------------------------------------------------------------------
// Name:	      Close
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:28:27
// Version:     1.00.00
// 
// Description: Just closes the resource file.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::Close (void)
{
	if (m_kFile.is_open ())
	{
		m_kFile.close ();
	}
}


//--------------------------------------------------------------------------------------------------
// Name:	      OpenLump
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:53:35
// Version:     1.00.00
// 
// Description: Finds the lump inside the resource file, checks which type of lump is and call the
//				correct method for opening the lump.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::OpenLump (char * acLumpName, slCLump& rkLump)
{
	slSLumpHeader tkLumpHeader;
	unsigned long tulLump;
	char		  tacDirectoryLumpName [256];

  m_kFile.seekg (0, ios::beg);

  slSResourceHeader tkHeader;
	m_kFile.read ((char *)&tkHeader, 555);

	m_ulVersion			  = tkHeader.ulVersion;
	m_ulFlags			    = tkHeader.ulFlags;
	m_ulNumberOfLumps	= tkHeader.ulNumberOfLumps;
	m_ulOffset			  = tkHeader.ulOffset;

	// goes to first lump position
	m_kFile.seekg (m_ulOffset, ios::beg);

	// attaches directory and lump name together
	strcpy (tacDirectoryLumpName, m_acCurrentDirectory);
	strcat (tacDirectoryLumpName, "/");
	strcat (tacDirectoryLumpName, acLumpName);

	// goes though all the lumps until finding the desired one or returning false
	for (tulLump = 0; tulLump < m_ulNumberOfLumps; tulLump++)
	{
		m_kFile.read ((char*) &tkLumpHeader, sizeof (slSLumpHeader));
		// if lump found check which type of lump and open it accordingly
		if (!strcmp (tacDirectoryLumpName, tkLumpHeader.acName))
		{
			// lump compressed and encrypted
			if (tkLumpHeader.ulFlags & slLumpCompressedEncrypted)
			{
				OpenLumpCompressedEncrypted (rkLump, tkLumpHeader);
			}
			// lump compressed
			else if (tkLumpHeader.ulFlags & slLumpCompressed)
			{
				OpenLumpCompressed (rkLump, tkLumpHeader);
			}
			// lump encrypted
			else if (tkLumpHeader.ulFlags & slLumpEncrypted)
			{
				OpenLumpEncrypted (rkLump, tkLumpHeader);
			}
			// raw lump
			else
			{
				OpenLumpRaw (rkLump, tkLumpHeader);
			}
		}
    else
    {
		  // not correct lump, move on
		  m_kFile.seekg (tkLumpHeader.ulSize, ios::cur);
    }
	}
}


//--------------------------------------------------------------------------------------------------
// Name:	      IsLumpValid
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:45:54
// Version:     1.00.00
// 
// Description: Checks if a lump's name is valid, this is, if it exists in the resource file.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
bool slCResourceFile::IsLumpValid (char * acLumpName)
{
	slSLumpHeader tkLumpHeader;
	unsigned long tulLump;
	char		  tacDirectoryLumpName [256];

	// goes to first lump position
	m_kFile.seekg (m_ulOffset, ios::beg);

	// attaches directory and lump name together
	strcpy (tacDirectoryLumpName, m_acCurrentDirectory);
	strcat (tacDirectoryLumpName, "/");
	strcat (tacDirectoryLumpName, acLumpName);

	// goes though all the lumps until finding the desired one or returning false
	for (tulLump = 0; tulLump < m_ulNumberOfLumps; tulLump++)
	{
		m_kFile.read ((char*) &tkLumpHeader, sizeof (slSLumpHeader));
		if (!strcmp (tacDirectoryLumpName, tkLumpHeader.acName))
		{
			return true;
		}
		// not correct lump, move on
		m_kFile.seekg (tkLumpHeader.ulSize + 1, ios::cur);
	}
	return false;
}


//--------------------------------------------------------------------------------------------------
// Name:	      SetCurrentDirectory
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:29:50
// Version:     1.00.00
// 
// Description: Sets the resource lump current directory.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::SetCurrentDirectory (char * acDirectory)
{
	strcpy ((char *)m_acCurrentDirectory, acDirectory);
}


//--------------------------------------------------------------------------------------------------
// Name:	      GetCurrentDirectory
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  20-03-2001 2:30:03
// Version:     1.00.00
// 
// Description: Returns the resource lump current directory.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
char * slCResourceFile::GetCurrentDirectory (void)
{
	return (char *)m_acCurrentDirectory;
}


//--------------------------------------------------------------------------------------------------
// Name:	      OpenLumpCompressed
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  21-03-2001 12:18:03
// Version:     1.00.00
// 
// Description: Open a compressed lump inside a resource.
// Remarks:     Compressing algorithm (RLE) doesn't work in all cases, removed until another one can
//              be implement.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::OpenLumpCompressed (slCLump& rkLump, slSLumpHeader SLumpHeader) 
{
	unsigned char * tpData = new unsigned char [SLumpHeader.ulSize];
  memset (tpData,0,SLumpHeader.ulSize);

  // read the data from the resource file
	tpData = new unsigned char [SLumpHeader.ulSize];
  memset (tpData, 0, SLumpHeader.ulSize);
	m_kFile.read ((char*)tpData, SLumpHeader.ulSize);

	// set the lump information and data
	rkLump.SetLumpName (SLumpHeader.acName);
	rkLump.SetLumpSize (SLumpHeader.ulSize);
	rkLump.SetDataPointer (tpData);
}


//--------------------------------------------------------------------------------------------------
// Name:	      OpenLumpEncrypted
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  21-03-2001 12:18:29
// Version:     1.00.00
// 
// Description: Open an encrypted lump inside a resource.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::OpenLumpEncrypted (slCLump& rkLump, slSLumpHeader SLumpHeader) 
{
	unsigned char * tpData;
	unsigned long tulPosition;

	// read the data from the resource lump
	tpData = new unsigned char [SLumpHeader.ulSize];
	m_kFile.read ((char*)tpData, SLumpHeader.ulSize);

	// decrypt file
	for (tulPosition = 0; tulPosition < SLumpHeader.ulSize; tulPosition++)
	{
		tpData [tulPosition] ^= SLumpHeader.byCheckSum;
	}

	// set the lump information and data
	rkLump.SetLumpName (SLumpHeader.acName);
	rkLump.SetLumpSize (SLumpHeader.ulSize);
	rkLump.SetDataPointer (tpData);
}


//--------------------------------------------------------------------------------------------------
// Name:	      OpenLumpCompressedEncrypted
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  21-03-2001 12:25:02
// Version:     1.00.00
// 
// Description: Open a compressed encrypted file inside a resource.
// Remarks:     Compressing algorithm (RLE) doesn't work in all cases, removed until another one can
//              be implement.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::OpenLumpCompressedEncrypted (slCLump& rkLump, slSLumpHeader SLumpHeader) 
{
	unsigned char * tpData;
  unsigned long tulPosition;

	// read the data from the resource lump
	tpData = new unsigned char [SLumpHeader.ulSizeUncompressed];

	m_kFile.read ((char*)tpData, SLumpHeader.ulSize);

	// decrypt lump
	for (tulPosition = 0; tulPosition < SLumpHeader.ulSizeUncompressed; tulPosition++)
	{
		tpData [tulPosition] ^= SLumpHeader.byCheckSum;
	}

	// set the lump information and data
	rkLump.SetLumpName (SLumpHeader.acName);
	rkLump.SetLumpSize (SLumpHeader.ulSizeUncompressed);
	rkLump.SetDataPointer (tpData);

}


//--------------------------------------------------------------------------------------------------
// Name:	      OpenLumpRaw
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  21-03-2001 12:18:42
// Version:     1.00.00
// 
// Description: Open a raw lump inside a resource.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::OpenLumpRaw (slCLump& rkLump, slSLumpHeader SLumpHeader) 
{
	unsigned char * tpData = new unsigned char [SLumpHeader.ulSize];
  memset (tpData,0,SLumpHeader.ulSize);

  // read the data from the resource file
	tpData = new unsigned char [SLumpHeader.ulSize];
  memset (tpData, 0, SLumpHeader.ulSize);
	m_kFile.read ((char*)tpData, SLumpHeader.ulSize);

	// set the lump information and data
	rkLump.SetLumpName (SLumpHeader.acName);
	rkLump.SetLumpSize (SLumpHeader.ulSize);
	rkLump.SetDataPointer (tpData);
}


//--------------------------------------------------------------------------------------------------
// Name:	      CompressLump
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  15-04-2001 5:54:23
// Version:     1.00.00
// 
// Description: Compresses a lump.
// Remarks:     Compressing algorithm (RLE) doesn't work in all cases, removed until another one can
//              be implement.
//--------------------------------------------------------------------------------------------------
unsigned char* slCResourceFile::CompressLump (slSLumpWritingHeader kWritingHeader, 
                                               slSLumpHeader& rkLumpHeader)
{
  unsigned char* pOriginalData;

  pOriginalData = new unsigned char [kWritingHeader.ulSize];

  // read original data
  ifstream fInput;
  fInput.open (kWritingHeader.acFileName, ios::in | ios::binary);
  fInput.read (pOriginalData,kWritingHeader.ulSize);
  fInput.close ();

  // set lump header information
  strcpy (rkLumpHeader.acName, kWritingHeader.acName);
  rkLumpHeader.byCheckSum = kWritingHeader.byCheckSum;
  rkLumpHeader.ulFlags = kWritingHeader.ulFlags;
  rkLumpHeader.ulSize = kWritingHeader.ulSize;
  rkLumpHeader.ulSizeUncompressed = kWritingHeader.ulSize;

  return pOriginalData;
}


//--------------------------------------------------------------------------------------------------
// Name:	      EncryptLump
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  15-04-2001 8:15:02
// Version:     1.00.00
// 
// Description: Encrypts a file
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
unsigned char* slCResourceFile::EncryptLump (slSLumpWritingHeader kWritingHeader, slSLumpHeader& rkLumpHeader)
{
  unsigned char* pOriginalData;
  unsigned char* pEncryptedData;

  pOriginalData = new unsigned char [kWritingHeader.ulSize];
  pEncryptedData = new unsigned char [kWritingHeader.ulSize];
  
  // read original data
  ifstream fInput;
  fInput.open (kWritingHeader.acFileName, ios::in | ios::binary);
  fInput.read (pOriginalData,kWritingHeader.ulSize);
  fInput.close ();

  memcpy (pEncryptedData, pOriginalData, kWritingHeader.ulSize);

	unsigned long tulPosition;

	// encrypt file
	for (tulPosition = 0; tulPosition < kWritingHeader.ulSize; tulPosition++)
	{
		pEncryptedData [tulPosition] ^= kWritingHeader.byCheckSum;
	}

  delete [] pOriginalData;

 	// set the lump information and data
  strcpy (rkLumpHeader.acName, kWritingHeader.acName);
  rkLumpHeader.byCheckSum = kWritingHeader.byCheckSum;
  rkLumpHeader.ulFlags = kWritingHeader.ulFlags;
  rkLumpHeader.ulSize = kWritingHeader.ulSize;
  rkLumpHeader.ulSizeUncompressed = kWritingHeader.ulSize;

  return pEncryptedData;
}


//--------------------------------------------------------------------------------------------------
// Name:	      CompressEncryptLump
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  15-04-2001 8:22:02
// Version:     1.00.00
// 
// Description: Encrypts and compressed a file.
// Remarks:     Compressing algorithm (RLE) doesn't work in all cases, removed until another one can
//              be implement.
//--------------------------------------------------------------------------------------------------
unsigned char* slCResourceFile::CompressEncryptLump (slSLumpWritingHeader kWritingHeader, slSLumpHeader& rkLumpHeader)
{
  unsigned char* pOriginalData;
  unsigned long   tulPosition;

  pOriginalData = new unsigned char [kWritingHeader.ulSize];
  
  // read original data
  ifstream fInput;
  fInput.open (kWritingHeader.acFileName, ios::in | ios::binary);
  fInput.read (pOriginalData,kWritingHeader.ulSize);
  fInput.close ();

	for (tulPosition = 0; tulPosition < kWritingHeader.ulSize; tulPosition++)
	{
		pOriginalData [tulPosition] = pOriginalData [tulPosition] ^ kWritingHeader.byCheckSum;
	}
  
  // set the lump information and data
  strcpy (rkLumpHeader.acName, kWritingHeader.acName);
  rkLumpHeader.byCheckSum = kWritingHeader.byCheckSum;
  rkLumpHeader.ulFlags = kWritingHeader.ulFlags;
  rkLumpHeader.ulSize = kWritingHeader.ulSize;
  rkLumpHeader.ulSizeUncompressed = kWritingHeader.ulSize;

  return pOriginalData;

}


//--------------------------------------------------------------------------------------------------
// Name:	      Build resouce
// Parent:	    slCResourceFile
// Author:      Bruno Sousa (BS)
// Created in:  15-04-2001 8:29:02
// Version:     1.00.00
// 
// Description: Builds a resource file.
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
void slCResourceFile::BuildResource (char* pstrFileName, long iNumberLumps, slSLumpWritingHeader *pkWritingHeader)
{
  ofstream fResource;
  slSResourceHeader kResourceHeader;

  fResource.open (pstrFileName, ios::out | ios::binary);

  strcpy (kResourceHeader.acSignature, "SealRFS");
  kResourceHeader.ulFlags = 0;
  kResourceHeader.ulNumberOfLumps = iNumberLumps;
  kResourceHeader.ulOffset = sizeof (slSResourceHeader);
  kResourceHeader.ulVersion = 1;

  fResource.write ((unsigned char*)&kResourceHeader, sizeof (slSResourceHeader));

  long iLump;

  unsigned char *pData;
  slSLumpHeader kLumpHeader;

  for (iLump=0; iLump < iNumberLumps; iLump++)
  {
			// lump compressed and encrypted
			if (pkWritingHeader [iLump].ulFlags & slLumpCompressedEncrypted)
			{
        pData = CompressEncryptLump (pkWritingHeader [iLump], kLumpHeader);
        fResource.write ((unsigned char*)&kLumpHeader, sizeof (slSLumpHeader));
        fResource.write (pData, kLumpHeader.ulSize);
        delete [] pData;
			}
			// lump compressed
			else if (pkWritingHeader [iLump].ulFlags & slLumpCompressed)
			{
        pData = CompressLump (pkWritingHeader [iLump], kLumpHeader);
        fResource.write ((unsigned char*)&kLumpHeader, sizeof (slSLumpHeader));
        fResource.write (pData, kLumpHeader.ulSize);
        delete [] pData;
			}
			// lump encrypted
			else if (pkWritingHeader [iLump].ulFlags & slLumpEncrypted)
			{
        pData = EncryptLump (pkWritingHeader [iLump], kLumpHeader);
        fResource.write ((unsigned char*)&kLumpHeader, sizeof (slSLumpHeader));
        fResource.write (pData, kLumpHeader.ulSize);
        delete [] pData;
			}
			// raw lump
			else
			{
        pData = new unsigned char [pkWritingHeader [iLump].ulSize];
  
	      // read the data from the file
        ifstream kFileRaw;
        kFileRaw.open (pkWritingHeader [iLump].acFileName, ios::in | ios::binary);
        kFileRaw.read (pData, pkWritingHeader [iLump].ulSize);
        kFileRaw.close ();
        // set the lump information
        strcpy (kLumpHeader.acName, pkWritingHeader [iLump].acName);
        kLumpHeader.byCheckSum = pkWritingHeader [iLump].byCheckSum;
        kLumpHeader.ulFlags = pkWritingHeader [iLump].ulFlags;
        kLumpHeader.ulSize = pkWritingHeader [iLump].ulSize;
        kLumpHeader.ulSizeUncompressed = pkWritingHeader [iLump].ulSize;
  
        // write the data to file
        fResource.write ((unsigned char*)&kLumpHeader, sizeof (slSLumpHeader));
        fResource.write (pData, kLumpHeader.ulSize);

        delete []pData;
			}
  }

  fResource.close ();
}


	//------------------
	// End of file
