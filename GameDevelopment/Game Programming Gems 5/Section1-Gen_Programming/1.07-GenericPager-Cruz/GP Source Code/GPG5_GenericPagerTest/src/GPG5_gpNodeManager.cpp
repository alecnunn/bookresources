// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeManager.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeManager Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeManager.h>

#include <GPG5_gpNode.h>

#include <GPG5_gpNodeWorld.h>
#include <GPG5_gpNodeWindow.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::GPG5_gpNodeManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeManager::GPG5_gpNodeManager()
{
	genericPagerEventManager_.Clean();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::~GPG5_gpNodeManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeManager::~GPG5_gpNodeManager()
{
	genericPagerEventManager_.Clean();
}

// +---------------------------
// |	Instance. Singleton Method
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::GetInstance
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeManager& GPG5_gpNodeManager::GetInstance()
{
	static GPG5_gpNodeManager instance;
	return instance;
}

// +---------------------------
// |	Events Manager
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::GetEventManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpEventManager& GPG5_gpNodeManager::GetEventManager()
{
	return genericPagerEventManager_;
}

// +---------------------------
// |	Process Nodes
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::ProcessNodes
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeManager::ProcessNodes()
{
	LockMe();
	
	//Destroy Nodes
	DestroyNodes();

	//Process Nodes
	GPG5_gpNodeList::iterator it		= nodeList_.begin();
	GPG5_gpNodeList::iterator itEnd = nodeList_.end();

	while ( it != itEnd )
	{
		(*it)->ProcessNode();
		it++;
	}

	UnLockMe();
}

// +---------------------------
// |	Terminate
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::Terminate
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeManager::Terminate()
{
	genericPagerEventManager_.Clean();
	CleanDestroyList();
	CleanAndDestroy();

	exit(0);
}

// +---------------------------
// |	ReInit GPworld and GPwindows with a new Index Size
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeManager::ReInit
// | Description     : 
// |
// | Parameters : 
// |   tileWidth     : 
// |   tileHeight    : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeManager::ReInit(int tileWidth, int tileHeight)
{
	GPG5_gpNodeWorld* pWorld = dynamic_cast<GPG5_gpNodeWorld*> ( GetNode("World Node") );
	if ( pWorld )
	{
		pWorld->Clean();
		pWorld->SetWorldLimits(0, 2000, 0, 1000, 2000.0/(float)tileWidth, 1000.0/(float)tileHeight);
	}

	GPindex mainIndex(0,0,tileWidth,tileHeight);
	
	GPG5_gpNodeWindow* pRedWindow = dynamic_cast<GPG5_gpNodeWindow*> ( GetNode("Red Window" ) );
	if ( pRedWindow )
	{
		GPindex indexRed = mainIndex.GetIndex(0,0);
		pRedWindow->ChangeIndex(indexRed, false);
	}

	GPG5_gpNodeWindow* pGreenWindow = dynamic_cast<GPG5_gpNodeWindow*> ( GetNode("Green Window" ) );
	if ( pGreenWindow )
	{
		GPindex indexGreen = mainIndex.GetIndex(900,700);
		pGreenWindow->ChangeIndex(indexGreen, false);
	}

	GPG5_gpNodeWindow* pBlueWindow = dynamic_cast<GPG5_gpNodeWindow*> ( GetNode("Blue Window" ) );
	if ( pBlueWindow )
	{
		GPindex indexBlue = mainIndex.GetIndex(1500,300);
		pBlueWindow->ChangeIndex(indexBlue, false);
	}
}
