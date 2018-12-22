/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GameFactoryImpl_t.cpp: implementation of the GameFactoryImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameFactoryImpl_t.h"
#include "GameFactoryIDs.h"
#include "GameInstanceIDs.h"
#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Framework/LogicLayer_t.h"
#include "GFInclude/Framework/Task_t.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "GFInclude/Systems/FactorySys_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"

#include "AVLayerObj_Intro_t.h"
#include "AVLayerObj_StartMenu_t.h"
#include "AVLayerObj_GameConfig_t.h"
#include "AVLayerObj_Game_t.h"
#include "AVLayerObj_GameHud_t.h"
#include "AVLayerObj_GamePause_t.h"
#include "AVLayerObj_SystemConfig_t.h"
#include "AVLayerObj_GameWin_t.h"
#include "AVLayerObj_GameLose_t.h"
#include "AVLayerObj_GameViewer_t.h"
#include "AVLayerObj_GameEditor_t.h"
#include "AVLayerConfigObj_Intro_t.h"
#include "AVLayerConfigObj_StartMenu_t.h"
#include "AVLayerConfigObj_GameConfig_t.h"
#include "AVLayerConfigObj_Game_t.h"
#include "AVLayerConfigObj_GameHud_t.h"
#include "AVLayerConfigObj_GamePause_t.h"
#include "AVLayerConfigObj_SystemConfig_t.h"
#include "AVLayerConfigObj_GameWin_t.h"
#include "AVLayerConfigObj_GameLose_t.h"
#include "AVLayerConfigObj_GameViewer_t.h"
#include "AVLayerConfigObj_GameEditor_t.h"
#include "LogicLayerObj_Intro_t.h"
#include "LogicLayerObj_StartMenu_t.h"
#include "LogicLayerObj_GameConfig_t.h"
#include "LogicLayerObj_Game_t.h"
#include "LogicLayerObj_GamePause_t.h"
#include "LogicLayerObj_SystemConfig_t.h"
#include "LogicLayerObj_GameWin_t.h"
#include "LogicLayerObj_GameLose_t.h"
#include "LogicLayerObj_GameViewer_t.h"
#include "LogicLayerObj_GameEditor_t.h"
#include "LogicLayerConfigObj_Intro_t.h"
#include "LogicLayerConfigObj_StartMenu_t.h"
#include "LogicLayerConfigObj_GameConfig_t.h"
#include "LogicLayerConfigObj_Game_t.h"
#include "LogicLayerConfigObj_GamePause_t.h"
#include "LogicLayerConfigObj_SystemConfig_t.h"
#include "LogicLayerConfigObj_GameWin_t.h"
#include "LogicLayerConfigObj_GameLose_t.h"
#include "LogicLayerConfigObj_GameViewer_t.h"
#include "LogicLayerConfigObj_GameEditor_t.h"
#include "TaskObj_StartApp_t.h"
#include "TaskObj_ChangeDriver_t.h"
#include "TaskConfigObj_Intro_t.h"
#include "TaskConfigObj_Game_t.h"
#include "TaskConfigObj_GameConfig_t.h"
#include "TaskConfigObj_SystemConfig_t.h"

/*
	This is the general outline of collection data

	COLLECTION_TAG,
		TYPE_TAG, nTypeID,
			FACTORY_TAG, nFactoryID,
				INSTANCE_TAG, nInstanceID, nConfigID,
			FACTORY_END_TAG,
		TYPE_END_TAG,
	COLLECTION_END_TAG,
	END_TAG,
*/

int _aCollData[] = {
	COLLECTION_TAG,
		TYPE_TAG, TYPE_ID_TASK,
			FACTORY_TAG, FACTORY_ID_TASK_START_APP,
				INSTANCE_TAG, INSTANCE_ID_TASK_START_APP, FACTORY_ID_TASK_CONFIG_NONE,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_TASK_CHANGE_DRIVER,
				INSTANCE_TAG, INSTANCE_ID_TASK_CHANGE_DRIVER, FACTORY_ID_TASK_CONFIG_NONE,
			FACTORY_END_TAG,
		TYPE_END_TAG,
	COLLECTION_END_TAG,
	COLLECTION_TAG,
		TYPE_TAG, TYPE_ID_AVLAYER,
			FACTORY_TAG, FACTORY_ID_AVLAYER_INTRO,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_INTRO, FACTORY_ID_AVLAYER_CONFIG_INTRO,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_STARTMENU,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_STARTMENU, FACTORY_ID_AVLAYER_CONFIG_STARTMENU,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMECONFIG,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMECONFIG, FACTORY_ID_AVLAYER_CONFIG_GAMECONFIG,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAME,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAME, FACTORY_ID_AVLAYER_CONFIG_GAME,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMEHUD,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMEHUD, FACTORY_ID_AVLAYER_CONFIG_GAMEHUD,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMEPAUSE,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMEPAUSE, FACTORY_ID_AVLAYER_CONFIG_GAMEPAUSE,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_SYSTEMCONFIG,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_SYSTEMCONFIG, FACTORY_ID_AVLAYER_CONFIG_SYSTEMCONFIG,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMEWIN,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMEWIN, FACTORY_ID_AVLAYER_CONFIG_GAMEWIN,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMELOSE,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMELOSE, FACTORY_ID_AVLAYER_CONFIG_GAMELOSE,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMEVIEWER,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMEVIEWER, FACTORY_ID_AVLAYER_CONFIG_GAMEVIEWER,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_AVLAYER_GAMEEDITOR,
				INSTANCE_TAG, INSTANCE_ID_AVLAYER_GAMEEDITOR, FACTORY_ID_AVLAYER_CONFIG_GAMEEDITOR,
			FACTORY_END_TAG,
		TYPE_END_TAG,
		TYPE_TAG, TYPE_ID_LOGICLAYER,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_INTRO,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_INTRO, FACTORY_ID_LOGICLAYER_CONFIG_INTRO,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_STARTMENU,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_STARTMENU, FACTORY_ID_LOGICLAYER_CONFIG_STARTMENU,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMECONFIG,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMECONFIG, FACTORY_ID_LOGICLAYER_CONFIG_GAMECONFIG,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAME,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAME, FACTORY_ID_LOGICLAYER_CONFIG_GAME,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMEPAUSE,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMEPAUSE, FACTORY_ID_LOGICLAYER_CONFIG_GAMEPAUSE,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_SYSTEMCONFIG,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_SYSTEMCONFIG, FACTORY_ID_LOGICLAYER_CONFIG_SYSTEMCONFIG,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMEWIN,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMEWIN, FACTORY_ID_LOGICLAYER_CONFIG_GAMEWIN,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMELOSE,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMELOSE, FACTORY_ID_LOGICLAYER_CONFIG_GAMELOSE,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMEVIEWER,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMEVIEWER, FACTORY_ID_LOGICLAYER_CONFIG_GAMEVIEWER,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_LOGICLAYER_GAMEEDITOR,
				INSTANCE_TAG, INSTANCE_ID_LOGICLAYER_GAMEEDITOR, FACTORY_ID_LOGICLAYER_CONFIG_GAMEEDITOR,
			FACTORY_END_TAG,
		TYPE_END_TAG,
		TYPE_TAG, TYPE_ID_TASK,
			FACTORY_TAG, FACTORY_ID_TASK_FRAME_PLAYER,
				INSTANCE_TAG, INSTANCE_ID_TASK_INTRO, FACTORY_ID_TASK_CONFIG_INTRO,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_TASK_FRAME_PLAYER,
				INSTANCE_TAG, INSTANCE_ID_TASK_GAME, FACTORY_ID_TASK_CONFIG_GAME,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_TASK_FRAME_PLAYER,
				INSTANCE_TAG, INSTANCE_ID_TASK_GAMECONFIG, FACTORY_ID_TASK_CONFIG_GAMECONFIG,
			FACTORY_END_TAG,
			FACTORY_TAG, FACTORY_ID_TASK_FRAME_PLAYER,
				INSTANCE_TAG, INSTANCE_ID_TASK_SYSTEMCONFIG, FACTORY_ID_TASK_CONFIG_SYSTEMCONFIG,
			FACTORY_END_TAG,
		TYPE_END_TAG,
	COLLECTION_END_TAG,
	END_TAG,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameFactoryImpl_t::GameFactoryImpl_t()
{

}

GameFactoryImpl_t::~GameFactoryImpl_t()
{

}

int * GameFactoryImpl_t::GetCollectionData()
{
	return _aCollData;
}

AVLayer_t * GameFactoryImpl_t::CreateAVLayer( int nFactoryID )
{
	switch( nFactoryID )
	{
	case FACTORY_ID_AVLAYER_INTRO:
		return new AVLayerObj_Intro_t;
	case FACTORY_ID_AVLAYER_STARTMENU:
		return new AVLayerObj_StartMenu_t;
	case FACTORY_ID_AVLAYER_GAMECONFIG:
		return new AVLayerObj_GameConfig_t;
	case FACTORY_ID_AVLAYER_GAME:
		return new AVLayerObj_Game_t;
	case FACTORY_ID_AVLAYER_GAMEHUD:
		return new AVLayerObj_GameHud_t;
	case FACTORY_ID_AVLAYER_GAMEPAUSE:
		return new AVLayerObj_GamePause_t;
	case FACTORY_ID_AVLAYER_SYSTEMCONFIG:
		return new AVLayerObj_SystemConfig_t;
	case FACTORY_ID_AVLAYER_GAMEWIN:
		return new AVLayerObj_GameWin_t;
	case FACTORY_ID_AVLAYER_GAMELOSE:
		return new AVLayerObj_GameLose_t;
	case FACTORY_ID_AVLAYER_GAMEVIEWER:
		return new AVLayerObj_GameViewer_t;
	case FACTORY_ID_AVLAYER_GAMEEDITOR:
		return new AVLayerObj_GameEditor_t;
	}
	return 0;
}

void GameFactoryImpl_t::ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID )
{
	AVLayerConfig_t *pAVLC = 0;

	switch( nConfigID )
	{
	case FACTORY_ID_AVLAYER_CONFIG_INTRO:
		pAVLC = new AVLayerConfigObj_Intro_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_STARTMENU:
		pAVLC = new AVLayerConfigObj_StartMenu_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMECONFIG:
		pAVLC = new AVLayerConfigObj_GameConfig_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAME:
		pAVLC = new AVLayerConfigObj_Game_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMEHUD:
		pAVLC = new AVLayerConfigObj_GameHud_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMEPAUSE:
		pAVLC = new AVLayerConfigObj_GamePause_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_SYSTEMCONFIG:
		pAVLC = new AVLayerConfigObj_SystemConfig_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMEWIN:
		pAVLC = new AVLayerConfigObj_GameWin_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMELOSE:
		pAVLC = new AVLayerConfigObj_GameLose_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMEVIEWER:
		pAVLC = new AVLayerConfigObj_GameViewer_t;
		break;
	case FACTORY_ID_AVLAYER_CONFIG_GAMEEDITOR:
		pAVLC = new AVLayerConfigObj_GameEditor_t;
		break;
	}

	pAVLC->Configure( pAVLayer );
	delete pAVLC;
}

void GameFactoryImpl_t::DeleteAVLayer( AVLayer_t *pAVLayer )
{
	delete pAVLayer;
}

LogicLayer_t * GameFactoryImpl_t::CreateLogicLayer( int nFactoryID )
{
	switch( nFactoryID )
	{
	case FACTORY_ID_LOGICLAYER_INTRO:
		return new LogicLayerObj_Intro_t;
	case FACTORY_ID_LOGICLAYER_STARTMENU:
		return new LogicLayerObj_StartMenu_t;
	case FACTORY_ID_LOGICLAYER_GAMECONFIG:
		return new LogicLayerObj_GameConfig_t;
	case FACTORY_ID_LOGICLAYER_GAME:
		return new LogicLayerObj_Game_t;
	case FACTORY_ID_LOGICLAYER_GAMEPAUSE:
		return new LogicLayerObj_GamePause_t;
	case FACTORY_ID_LOGICLAYER_SYSTEMCONFIG:
		return new LogicLayerObj_SystemConfig_t;
	case FACTORY_ID_LOGICLAYER_GAMEWIN:
		return new LogicLayerObj_GameWin_t;
	case FACTORY_ID_LOGICLAYER_GAMELOSE:
		return new LogicLayerObj_GameLose_t;
	case FACTORY_ID_LOGICLAYER_GAMEVIEWER:
		return new LogicLayerObj_GameViewer_t;
	case FACTORY_ID_LOGICLAYER_GAMEEDITOR:
		return new LogicLayerObj_GameEditor_t;
	}
	return 0;
}

void GameFactoryImpl_t::ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID )
{
	LogicLayerConfig_t *pLLC = 0;

	switch( nConfigID )
	{
	case FACTORY_ID_LOGICLAYER_CONFIG_INTRO:
		pLLC = new LogicLayerConfigObj_Intro_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_STARTMENU:
		pLLC = new LogicLayerConfigObj_StartMenu_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMECONFIG:
		pLLC = new LogicLayerConfigObj_GameConfig_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAME:
		pLLC = new LogicLayerConfigObj_Game_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMEPAUSE:
		pLLC = new LogicLayerConfigObj_GamePause_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_SYSTEMCONFIG:
		pLLC = new LogicLayerConfigObj_SystemConfig_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMEWIN:
		pLLC = new LogicLayerConfigObj_GameWin_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMELOSE:
		pLLC = new LogicLayerConfigObj_GameLose_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMEVIEWER:
		pLLC = new LogicLayerConfigObj_GameViewer_t;
		break;
	case FACTORY_ID_LOGICLAYER_CONFIG_GAMEEDITOR:
		pLLC = new LogicLayerConfigObj_GameEditor_t;
		break;
	}

	pLLC->Configure( pLogicLayer );
	delete pLLC;
}

void GameFactoryImpl_t::DeleteLogicLayer( LogicLayer_t *pLogicLayer )
{
	delete pLogicLayer;
}

Task_t * GameFactoryImpl_t::CreateTask( int nFactoryID )
{
	switch( nFactoryID )
	{
	case FACTORY_ID_TASK_FRAME_PLAYER:
		return new FramePlayer_t;
	case FACTORY_ID_TASK_START_APP:
		return new TaskObj_StartApp_t;
	case FACTORY_ID_TASK_CHANGE_DRIVER:
		return new TaskObj_ChangeDriver_t;
	}
	return 0;
}

void GameFactoryImpl_t::ConfigTask( Task_t *pTask, int nConfigID, Systems_t *pSystems )
{
	TaskConfig_t *pTC = 0;

	switch( nConfigID )
	{
	case FACTORY_ID_TASK_CONFIG_INTRO:
		pTC = new TaskConfigObj_Intro_t;
		break;
	case FACTORY_ID_TASK_CONFIG_GAME:
		pTC = new TaskConfigObj_Game_t;
		break;
	case FACTORY_ID_TASK_CONFIG_GAMECONFIG:
		pTC = new TaskConfigObj_GameConfig_t;
		break;
	case FACTORY_ID_TASK_CONFIG_SYSTEMCONFIG:
		pTC = new TaskConfigObj_SystemConfig_t;
		break;
	}

	if( pTC )
	{
		pTC->Configure( pTask, pSystems );
		delete pTC;
	}
}

void GameFactoryImpl_t::DeleteTask( Task_t *pTask )
{
	delete pTask;
}

