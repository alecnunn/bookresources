// GAWaveFile Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GAWaveFile.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAWaveFile() { return( new CGAWaveFile); }
const CClass CGAWaveFile::classCGAWaveFile(CID_GAWaveFile, CID_GAFile, "CGAWaveFile", CreateCGAWaveFile);
const IClass*	CGAWaveFile::GetClass() { return((const IClass*)&classCGAWaveFile); }


// CTOR
CGAWaveFile::CGAWaveFile()
{

}

// DTOR
CGAWaveFile::~CGAWaveFile()
{
	Terminate();
}

void CGAWaveFile::Terminate()
{
	CGAFile::Terminate();
}


ATOM_WAVEFILEHEADER* CGAWaveFile::ReadHeader()
// Typically formatsize is 44 bytes:
// RIFFTag+RiffSize+WaveTag+fmtTag+fmtSize+WaveFormatExSize(16bytes)+dataTag+dataSize
{

	BOOL b = Read(&m_FileHeader, sizeof(ATOM_WAVEFILEHEADER));
	return(b ? &m_FileHeader : NULL);

}




#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
