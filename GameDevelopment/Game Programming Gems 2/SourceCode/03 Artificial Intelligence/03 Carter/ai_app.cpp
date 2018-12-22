#include "standard_headers.hpp"
#include "ai_app.hpp"
#include "entities.hpp"

//*************************************************************************************************
CAIApp::CAIApp()
{
    // Override base class members
    m_strWindowTitle         = _T("Micro thread AI");
    m_bUseDepthBuffer        = TRUE;
	PFont = new CD3DFont("Times New Roman", 10, 0);
}

//*************************************************************************************************
HRESULT CAIApp::OneTimeSceneInit()
{
	GameTurn = 0;
	CurrentEntityID = 0;
	CreateEntity(ENTITY_TROLL, C2DCoordF(0, 0));
	//CreateEntity(ENTITY_TROLL, 100, 100);

	return(S_OK);
}

//*************************************************************************************************
HRESULT CAIApp::InitDeviceObjects()
{
    PFont->InitDeviceObjects( m_pd3dDevice );
	return(S_OK);
}

//*************************************************************************************************
HRESULT CAIApp::RestoreDeviceObjects()
{
    PFont->RestoreDeviceObjects();
	
	return(S_OK);
}

//*************************************************************************************************
HRESULT CAIApp::InvalidateDeviceObjects()
{
    PFont->InvalidateDeviceObjects();
	
	return(S_OK);
}

//*************************************************************************************************
HRESULT CAIApp::DeleteDeviceObjects()
{
    PFont->DeleteDeviceObjects();
	return(S_OK);
	
}

//*************************************************************************************************
HRESULT CAIApp::FinalCleanup()
{
	for(std::vector<CEntity*>::iterator it=Entities.begin(); it!=Entities.end();)
	{
		delete(*it);
		it=Entities.erase(it);
	}
		
	
	return S_OK; 
}

//*************************************************************************************************
HRESULT CAIApp::Render()
{
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0L );
    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		for(std::vector<CEntity*>::iterator it=Entities.begin(); it!=Entities.end(); ++it)
		{
			(*it)->Render();
		}
		m_pd3dDevice->EndScene();
	}
	return S_OK;
}

//*************************************************************************************************
HRESULT CAIApp::FrameMove()
{
	GameTurn++;
	for(std::vector<CEntity*>::iterator it=Entities.begin(); it!=Entities.end(); ++it)
	{
		(*it)->Update();
	}

	for(it=Entities.begin(); it!=Entities.end();)
	//for(std::vector<CEntity*>::iterator it=Entities.begin(); it!=Entities.end();)
	{
		if((*it)->IsDead())
		{
			delete *it;
			it = Entities.erase(it);
		}
		else
		{
			++it;
		}
	}
	

	if(GameTurn%5000==0)
	{
		C2DCoordF pos;
		pos.X = (rand()*GetMapWidth())/RAND_MAX;
		pos.Y = (rand()*GetMapHeight())/RAND_MAX;
		
		CreateEntity(ENTITY_FOOD, pos);
	}
	return S_OK;
}

//*************************************************************************************************
void CAIApp::CreateEntity(EEntityType type, const C2DCoordF& pos)
{
	Entities.push_back(new CEntity(*this, CurrentEntityID, type, pos));
}

//*************************************************************************************************
CEntity* CAIApp::FindNearestEntity(EEntityType type, const C2DCoordF& pos)
{
	float nearest_dist = 99999.0f;
	CEntity* pnearest_entity = NULL;
	for(std::vector<CEntity*>::iterator it=Entities.begin(); it!=Entities.end(); ++it)
	{
		CEntity& entity = **it;

		if(entity.GetType()==type)
		{
			float dist = Get2DDistance(pos, entity.GetPos());
		
			if(dist<nearest_dist)
			{
				nearest_dist = dist;
				pnearest_entity = &entity;
			}
		}
	}
		
	return(pnearest_entity);
}



