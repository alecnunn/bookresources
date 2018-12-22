// GASampleWriter Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GASampleWriter.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASampleWriter() { return( new CGASampleWriter); }
const CClass CGASampleWriter::classCGASampleWriter(CID_GASampleWriter, CID_GAProcessor, "CGASampleWriter", CreateCGASampleWriter);
const IClass*	CGASampleWriter::GetClass() { return((const IClass*)&classCGASampleWriter); }


// CTOR
CGASampleWriter::CGASampleWriter()
{
	SetName("SampleWriter");
	AppendPin(SampleIn);
	m_nWritePos = 0;	
	m_bWorking=false;

}

// DTOR
CGASampleWriter::~CGASampleWriter()
{
	m_File.Close();
	Terminate();
}

// explicit terminate baseclass
void CGASampleWriter::Terminate()
{

	CGAProcessor::Terminate();
}

void CGASampleWriter::Start()
{
	Stop();
	m_File.Open("recorded.flt", IGAFile::CreateBin); // create, if not yet exists
	m_nWritePos = 0;
		m_bWorking=true;

}

void CGASampleWriter::Stop()
{
	for(int i=m_nWritePos;i < 88200;i++)
		m_Samples[i] = 0.0f;

	if(m_nWritePos > 0)
		m_File.Write(m_Samples, m_nWritePos * sizeof(FLOAT32));

	m_File.Close();
	m_bWorking=false;

}

void CGASampleWriter::Append(FLOAT32* pSignal, UINT32 nCount)
{
	if(m_bWorking == false)
		return;

	bool bStop = false;
	INT32 nSpace = 88200 - m_nWritePos;
	INT32 nToWrite = nCount;

	if(nSpace <  nToWrite) { 

		bStop = true;
		nToWrite = nSpace;
	}

	memcpy(m_Samples + m_nWritePos, pSignal, nToWrite* sizeof(FLOAT32));
	m_nWritePos += nToWrite;

	if(bStop == true)
		Stop();
}




// write samples to file
UINT32 CGASampleWriter::Process(IObject* pob)
{
/*
	UINT32 nFrames = g_GASystem.GetFrameCount();
	UINT32 nBytes = nFrames * sizeof(FLOAT32);
	FLOAT32* pfIn = GetSignalData(SampleIn);
	m_File.Write(pfIn, nBytes);
*/
	return(0);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
