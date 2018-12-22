/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: ResourceMgr.cpp
 *    Desc: Handles the loading of resources
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "ResourceMgr.h"
#include "..\gamelib\file.h"

extern int g_useTextures;

/**
 * instantiation of the global pointer to the resource manager.
 */
cResourceMgr* cResourceMgr::m_pGlobalResourceMgr = NULL;

cResourceMgr::cResourceMgr()
{
	if( m_pGlobalResourceMgr )
		throw cGameError( "cResourceMgr object already created!\n" );
	m_pGlobalResourceMgr = this;
}

cResourceMgr::cResourceMgr( char* resFile )
{
	if( m_pGlobalResourceMgr )
		throw cGameError( "cResourceMgr object already created!\n" );
	m_pGlobalResourceMgr = this;

	cFile file;

  try
  {
    file.Open( resFile );
  }
  catch( cFileCannotCreate )
  {
    throw cGameError( "cResourceMgr could not locate the resource file!\n" );
  }

	char buff[256];
	char type[20];
	char filename[ 256 ];
	int id;
	// first line is RES_BEGIN
	file.ReadNonCommentedLine( buff, '#' );		
	while( 1 )
	{
		file.ReadNonCommentedLine( buff, '#' );		

		sscanf( buff, "%s", type );
		if( 0 == strcmp( "RES_END", type ) )
			break;

		sscanf( buff, "%s %d %s", type, &id, filename );
		if( 0 == strcmp( "MODEL", type ) )
		{
			float scale;
			sscanf( buff, "%s %d %s %f", type, &id, filename, &scale );
			cModel* pModel = new cModel( filename );
			pModel->Scale( scale );
			m_resourceMap[id] = (void*)pModel;
		}
		else if( 0 == strcmp( "TEXTURE", type ) && g_useTextures )
		{
			cTexture* pTex = new cTexture( filename );
			m_resourceMap[id] = (void*)pTex;
		}
		else if( 0 == strcmp( "SOUND", type ) )
		{
			cSoundWrapper* pSound = new cSoundWrapper( filename );
			m_resourceMap[id] = (void*)pSound;
		}
	}
}

cResourceMgr::~cResourceMgr()
{
	/* iterate through the map, destroying everything. */
	map< resID, void* >::iterator iter;
	for( iter = m_resourceMap.begin();
		iter != m_resourceMap.end();
		iter++ )
	{
		delete (*iter).second;

		m_resourceMap[ (*iter).first ] = NULL;
	}

	m_pGlobalResourceMgr = NULL;
}
