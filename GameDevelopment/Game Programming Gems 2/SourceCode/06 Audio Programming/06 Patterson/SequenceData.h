/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// SequenceData.h

class TrackData_t;

typedef std::list< TrackData_t * > TrackDataPtrList_t;

class SequenceData_t
{
public:
	SequenceData_t();
	~SequenceData_t();

	void SetFileName( String_t &sFileName );
	String_t GetFileName();

	void SetID( int nID );
	int GetID();

	u32 GetPpq();
	u32 GetTempo();

	int GetTrackCount();

	bool Load( const char *pFilename );
	void Print();

	void PushBackTrack( TrackData_t *pTrackData );

	TrackDataPtrList_t m_TrackDataPtrList;
	IntVectorVector_t *m_pTrackVolumeVectorVector;

private:
	String_t m_sFileName;
	int m_nID;

	bool Open( const char *pFilename );
	void Close();

    u32 m_nChunkLength;
    s8 m_nChunkId[4];
    u32 m_nFormat;
    u32 m_nTracks;
    u32 m_nPpq;
	u32 m_nTempo;
    FILE *m_pFile;
    u8 *m_pData;

};