/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: NetMessages.cpp
 *    Desc: All of the network messages the game uses
 *          gives abstract defenitions, hides ugly details 
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "NetMessages.h"
#include "LoginNM.h"
#include "EntNM.h"
#include "MiscNM.h"

/**
 * Need to define the registry variable up here, before
 * the makers, so it gets constructed.
 */
typedef std::map<unsigned long int, cNetMessageMaker *> tNetMessageMakerMap;
tNetMessageMakerMap cNetMessageMaker::m_registry = tNetMessageMakerMap();


//==========--------------------------  BEGIN MAKER DEFINITIONS

DEFINE_MAKER( cNM_LoginRequestMaker, cNM_LoginRequest, 1 );
DEFINE_MAKER( cNM_LoginAcceptedMaker, cNM_LoginAccepted, 2 );
DEFINE_MAKER( cNM_EntAddMaker, cNM_EntAdd, 3);
DEFINE_MAKER( cNM_EntStateChangeMaker, cNM_EntStateChange, 4 );
DEFINE_MAKER( cNM_SpawnReqMaker, cNM_SpawnReq, 5 );
DEFINE_MAKER( cNM_EntStateChangeRequestMaker, cNM_EntStateChangeRequest, 6 );
DEFINE_MAKER( cNM_LogoutNoticeMaker, cNM_LogoutNotice, 7 );
DEFINE_MAKER( cNM_NewPlayerMaker, cNM_NewPlayer, 8 );
DEFINE_MAKER( cNM_MiniStateChangeMaker, cNM_MiniStateChange, 9 );
DEFINE_MAKER( cNM_MiniStateChangeRequestMaker, cNM_MiniStateChangeRequest, 10 );
DEFINE_MAKER( cNM_ProjFireRequestMaker, cNM_ProjFireRequest, 11 );
DEFINE_MAKER( cNM_PlayerAddMaker, cNM_PlayerAdd, 12 );
DEFINE_MAKER( cNM_ScoreChangeMaker, cNM_ScoreChange, 13 );

//==========--------------------------  END MAKER DEFINITIONS


cNetMessage* cNetMessageMaker::ConstructMessage( 
	uchar *pData, 
	int datasize )
{
	// Get a message ID (first char of the message)
	uchar msgID = *pData;

	try 
	{
		// find the appropriate factory in the map of factories...
		cNetMessageMaker* pMaker = 
			(*m_registry.find(msgID)).second;

		if( !pMaker )
			throw cGameError( "Bad Message ID" );

		// use that factory to construct the net_message derivative
		return pMaker->MakeMessage( pData, datasize );
	} 
	catch(...) 
	{
		// Complain.  we got a message we don't grok.
		DP1( 
"cNetMessageMaker::ConstructMessage: logic error, I don't \
know how to (or can't) construct message ID %d!", (int)msgID );
		throw;
	}
	return NULL;
}

