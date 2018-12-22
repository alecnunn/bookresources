/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "time.h"
#include "robot.h"
#include "database.h"
#include "msgroute.h"
#include "debuglog.h"
#include "stdafx.h"


//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	//Create Singletons
	Time* master_time = new Time();
	Database* master_database = new Database();
	MsgRoute* master_msgroute = new MsgRoute();
	DebugLog* master_debuglog = new DebugLog();


	//Create game object
	GameObject myGameObject( g_database.GetNewObjectID() );
	g_database.Store( myGameObject );

	//Give the game object a state machine
	myGameObject.SetStateMachine( new Robot( &myGameObject ) );
	myGameObject.GetStateMachine()->Initialize();


	// Game Loop
	while(1)
	{
		g_time.MarkTimeThisTick();

		myGameObject.GetStateMachine()->Update();

		g_msgroute.DeliverDelayedMessages();
	}

	delete( master_time );
	delete( master_database );
	delete( master_msgroute );
	delete( master_debuglog );


	return(0);
}

