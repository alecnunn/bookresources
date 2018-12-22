//-----------------------------------------------------------------------------
// Main.h implementation.
// Refer to the Main.h interface for more details.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#include "Main.h"

//-----------------------------------------------------------------------------
// Application specific network message handler.
//-----------------------------------------------------------------------------
void HandleNetworkMessage( ReceivedMessage *msg )
{
	// Only allow network messages to be processed while in the game state.
	if( g_engine->GetCurrentState()->GetID() != STATE_GAME )
		return;

	// Pass control over to the game state's network message handler.
	( (Game*)g_engine->GetCurrentState() )->HandleNetworkMessage( msg );
}

//-----------------------------------------------------------------------------
// Application specific state setup.
//-----------------------------------------------------------------------------
void StateSetup()
{
	g_engine->AddState( new Menu, true );
	g_engine->AddState( new Game, false );
}

//-----------------------------------------------------------------------------
// Application specific material creation.
//-----------------------------------------------------------------------------
void CreateMaterialResource( Material **resource, char *name, char *path = "./" )
{
	*resource = new GameMaterial( name, path );
}

//-----------------------------------------------------------------------------
// Entry point for the application.
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// A unique id for the application so it can be identified on a network.
	GUID guid = { 0xd6c55c78, 0x5030, 0x43b7, { 0x85, 0xa9, 0xc, 0x8b, 0xbe, 0x77, 0x5a, 0x62 } };

	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.guid = guid;
	setup.name = "Cityscape";
	setup.scale = 0.01f;
	setup.totalBackBuffers = 1;
	setup.HandleNetworkMessage = HandleNetworkMessage;
	setup.StateSetup = StateSetup;
	setup.CreateMaterialResource = CreateMaterialResource;
	setup.spawnerPath = "./Assets/Objects/";

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}