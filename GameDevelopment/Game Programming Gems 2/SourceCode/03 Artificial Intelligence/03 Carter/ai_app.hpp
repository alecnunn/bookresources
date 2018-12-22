#pragma once

#ifndef __AI_APP_HPP__
#define __AI_APP_HPP__

#include "d3dx8.h"
#include "d3dapp.h"
#include "entities.hpp"


class CEntity;

template<class TCIterator> TCIterator FindEntityWithID(TCIterator start, TCIterator end, int id)
{
	while(start!=end)
	{
		if(start->GetID()==id)
		{
			return(start);
		}
		++start;
	}
	return(end);
}

class CAIApp : public CD3DApplication
{
protected:
	std::vector<CEntity*> Entities;	
	int GameTurn;
	int CurrentEntityID;
	CD3DFont* PFont;
	
protected:
    HRESULT InitDeviceObjects();
    HRESULT FrameMove();
    HRESULT DeleteDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
		
		
    HRESULT OneTimeSceneInit();
    HRESULT FinalCleanup();
    HRESULT Render();
	void	CreateEntity(EEntityType type, const C2DCoordF& pos);

	
		


public:
	CAIApp();
	float GetMapWidth(void) const		{ return(256.0f); };
	float GetMapHeight(void) const		{ return(256.0f); };

	float GetDisplayWidth(void) const	{ return(m_d3dsdBackBuffer.Width); };
	float GetDisplayHeight(void) const	{ return(m_d3dsdBackBuffer.Height); };
	CD3DFont& GetFont(void)				{ return(*PFont); };

	CEntity* FindNearestEntity(EEntityType type, const C2DCoordF& pos);
	
	CEntity* GetEntity(int id);
	
};

#endif //__AI_APP_HPP__