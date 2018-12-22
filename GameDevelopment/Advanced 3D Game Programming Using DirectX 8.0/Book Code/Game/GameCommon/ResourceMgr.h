/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: ResourceMgr.h
 *    Desc: Header for the resource manager
 *          Handles all of the data files in the game
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _RESOURCEMGR_H
#define _RESOURCEMGR_H

#include <map>
#include <list>
using namespace std;

typedef int resID;

class cResourceMgr  
{
	static cResourceMgr* m_pGlobalResourceMgr;

	map< resID, void* > m_resourceMap;

public:
	cResourceMgr( char* resFile );
	cResourceMgr();
	virtual ~cResourceMgr();

	static cResourceMgr* GetResources()
	{
		return m_pGlobalResourceMgr;
	}

	void* Get( resID id )
	{
		return m_resourceMap[id];
	}

	void Add( resID id, void* pRes )
	{
		m_resourceMap[id] = pRes;
	}
};

inline void CreateResources()
{
	new cResourceMgr();
}

inline cResourceMgr* Resources()
{
	return cResourceMgr::GetResources();
}


//==========--------------------------  Some convenient wrapper functions

#ifndef GAMESERVER

class cSoundWrapper
{
public:
	list< cSound* > m_list;

	cSoundWrapper( char* filename )
	{
		m_list.push_back( new cSound( filename ) );
	}

	~cSoundWrapper()
	{
		std::list< cSound* >::iterator iter;
		for( iter = m_list.begin(); 
			iter != m_list.end();
			iter++ )
		{
			delete (*iter);
		}
	}

	void Play()
	{
		std::list< cSound* >::iterator iter;
		for( 
			iter = m_list.begin(); 
			iter != m_list.end();
			iter++ )
		{
			if( !(*iter)->IsPlaying() )
			{
				(*iter)->Play();
				return;
			}
		}

		/**
		 * We need to add a new sound
		 */
		cSound* pNew = new cSound( *(cSound*)(*m_list.begin()) );
		pNew->Play();
		m_list.push_back( pNew );
	}
};

#endif

#ifdef GAMESERVER

class cSoundWrapper
{
public:
	cSoundWrapper( char* filename )
	{
	}

	~cSoundWrapper()
	{
	}

	void Play()
	{
	}
};

#endif

inline void ActivateTexture( resID id )
{
#ifndef GAMESERVER
	cTexture* pTex = (cTexture*)Resources()->Get( id );
	if( pTex )
	{
		Graphics()->GetDevice()->SetTexture( 0, pTex->GetTexture() );
	}
#endif
}


#endif //_RESOURCEMGR_H
