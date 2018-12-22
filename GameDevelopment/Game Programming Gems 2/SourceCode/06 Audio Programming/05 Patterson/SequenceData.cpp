/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// SequenceData.cpp

#include "StdAfx.h"
#include "EventData.h"
#include "TrackData.h"
#include "SequenceData.h"
#include "Portable.h"

SequenceData_t::SequenceData_t()
{
    m_nChunkLength = 0;
    m_nChunkId[0] = 0;
    m_nChunkId[1] = 0;
    m_nChunkId[2] = 0;
    m_nChunkId[3] = 0;
    m_nFormat = 0;
    m_nTracks = 0;
    m_nPpq = 0;
	m_nTempo = 120;
    m_pFile = 0;
    m_pData = 0;
}

SequenceData_t::~SequenceData_t()
{
	TrackDataPtrList_t::iterator iTDL;

	for( iTDL = m_TrackDataPtrList.begin(); iTDL != m_TrackDataPtrList.end(); ++iTDL )
	{
		delete *iTDL;
	}
}

void SequenceData_t::SetFileName( String_t &sFileName )
{
	m_sFileName = sFileName;
}

String_t SequenceData_t::GetFileName()
{
	return m_sFileName;
}

void SequenceData_t::SetID( int nID )
{
	m_nID = nID;
}

int SequenceData_t::GetID()
{
	return m_nID;
}

u32 SequenceData_t::GetPpq()
{
	return m_nPpq;
}

u32 SequenceData_t::GetTempo()
{
	return m_nTempo;
}

int SequenceData_t::GetTrackCount()
{
	return m_nTracks;
}

bool SequenceData_t::Load( const char *pFilename )
{
	if( !pFilename )
	{
		return false;
	}

	if( Open( pFilename ) )
	{
		for( int i=0; i<m_nTracks; i++ )
		{
			TrackData_t *pTrackData = new TrackData_t;

			pTrackData->Load( m_pData, i );
			if( pTrackData->IsTempoTrack() )
			{
				m_nTempo = pTrackData->GetTempo();
			}
			PushBackTrack( pTrackData );
		}

		Close();
		return true;
	}

	return false;
}

bool SequenceData_t::Open( const char *pFilename )
{
	u8 aHeader[14];
	u32 nFilelength,nBytesRemaining;
	
	if ((m_pFile=fopen(pFilename,"rb")) != NULL)
    {
		fseek(m_pFile,0,SEEK_END);
		nFilelength = ftell(m_pFile);
		fseek(m_pFile,0,SEEK_SET);
		
		fread(aHeader,1,sizeof(aHeader),m_pFile);
		memcpy(m_nChunkId,aHeader,sizeof(m_nChunkId));
		if( strncmp( "MThd", m_nChunkId, 4 ) )
		{
			fclose( m_pFile );
			return false;
		}

		m_nChunkLength=Portable_ConvertStringToS32(&aHeader[4]);
		m_nFormat=Portable_ConvertStringToU16(&aHeader[8]);
		m_nTracks=Portable_ConvertStringToU16(&aHeader[10]);
		m_nPpq=Portable_ConvertStringToU16(&aHeader[12]);
		if( m_nChunkLength > 6 )
		{
			fseek(m_pFile,m_nChunkLength-6,SEEK_CUR);
		}
		
		nBytesRemaining = nFilelength-(m_nChunkLength+8);
		m_pData=(u8 *)malloc(nBytesRemaining);
		fread(m_pData,1,nBytesRemaining,m_pFile);

		return true;
    }
	return false;
}

void SequenceData_t::Close()
{
	fclose(m_pFile);

	if(m_pData)
	{
		free(m_pData);
	}
}

void SequenceData_t::Print()
{
	TrackDataPtrList_t::iterator iTDL;

	printf( "Sequence: \n" );

	printf( "  Chunk Length: %d\n", m_nChunkLength );
	printf( "  Format: %d\n", m_nFormat );
	printf( "  Tracks: %d\n", m_nTracks );
	printf( "  Ppq: %d\n", m_nPpq );

	printf( "\n" );

	for( iTDL = m_TrackDataPtrList.begin(); iTDL != m_TrackDataPtrList.end(); ++iTDL )
	{
		TrackData_t *pTrackData = *iTDL;

		pTrackData->Print();
	}
}

void SequenceData_t::PushBackTrack( TrackData_t *pTrackData )
{
	m_TrackDataPtrList.push_back( pTrackData );
}
