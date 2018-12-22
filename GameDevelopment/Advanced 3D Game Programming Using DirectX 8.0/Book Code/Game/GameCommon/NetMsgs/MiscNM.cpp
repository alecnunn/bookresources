/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MiscNM.cpp
 *    Desc: Miscellaneous Net Messages
 *          Grok NetMessages.h before you explore here.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "MiscNM.h"


//==========--------------------------  


cNM_ScoreChange::cNM_ScoreChange( 
	int entID, 
	int newScore )
: m_entID( entID )
, m_newScore( newScore )
{
	LogPrint("cNM_ScoreChange construction: %d score should go to %d",
		m_entID,
		m_newScore );
}

int cNM_ScoreChange::SerializeTo( uchar* pOutput )
{
	int dataSize = 4 + 4; // id, score
	*pOutput++ = 13;
	memcpy( pOutput, &m_entID, dataSize );
	return 1 + dataSize;
}

void cNM_ScoreChange::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + 4; // id, score
	memcpy( &m_entID, pFromData + 1, dataSize );
}

void cNM_ScoreChange::Exec()
{
	DebPrint("Changing %d's score", m_entID );
	cGamePlayerEnt* pPlayer = (cGamePlayerEnt*)MsgDaemon()->Get( m_entID );

	if( !pPlayer )
	{
		LogPrint("ERROR: score change on bad player %d", m_entID);
		return;
	}

	DebPrint("Changing %d score to %d", m_entID, m_newScore );
	
	pPlayer->SetScore( m_newScore );
}
