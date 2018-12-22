//-----------------------------------------------------------------------------
// The derived game state where all the game's processing happens.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#ifndef GAME_H
#define GAME_H

//-----------------------------------------------------------------------------
// State ID Define
//-----------------------------------------------------------------------------
#define STATE_GAME 1

//-----------------------------------------------------------------------------
// Game Class
//-----------------------------------------------------------------------------
class Game : public State
{
public:
	Game();

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer( ViewerSetup *viewer );
	virtual void Update( float elapsed );
	virtual void Render();

	void HandleNetworkMessage( ReceivedMessage *msg );

private:
	Material *m_crosshair; // Material used to render the crosshair.

	char m_scoreBoardNames[MAX_PATH]; // Text for displaying the names of all the connected players.
	char m_scoreBoardFrags[MAX_PATH]; // Text for displaying each player's frag count.
	char m_scoreBoardDeaths[MAX_PATH]; // Text for displaying each player's death tally.
	Font *m_scoreBoardFont; // Font used to render the score board.
};

//-----------------------------------------------------------------------------
// Externals
//-----------------------------------------------------------------------------
extern Game *g_game;

#endif