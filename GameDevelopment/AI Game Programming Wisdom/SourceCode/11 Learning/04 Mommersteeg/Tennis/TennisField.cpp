//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    TennisPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "tennisfield.h"
#include "soundbuffer.h"

//----------------------------------------------------------------------------------------------
// External declarations
//----------------------------------------------------------------------------------------------

extern LPDDS			imgField;
extern CSoundBuffer		sndHitBall;
extern CSoundBuffer		sndBounce;

//----------------------------------------------------------------------------------------------
// NetCollision(): checks for collisions between the ball and the tennis net (disabled here
//				   by setting the NET_HEIGHT to 0)	
//----------------------------------------------------------------------------------------------

void CTennisField::NetCollision() {
	RECT net;
	net.left = field.left;
	net.right = field.right;
	net.top = (field.top+field.bottom)/2 - NET_WIDTH;
	net.bottom = (field.top+field.bottom)/2 + NET_WIDTH;

	POINT pt;
	ball->GetPosition(pt);
	if (ball->GetHeight()<NET_HEIGHT && PtInRect(&net, pt)) {
		ball->SetVelocity(0,0,0);
	}
}

//----------------------------------------------------------------------------------------------
// SetupField(): sets up the tennis field and registers the game objects
//----------------------------------------------------------------------------------------------

void CTennisField::SetupField(LPRECT pField, CPaddle * Player1, CPaddle * Player2, CTennisBall * Ball) {
	// copy information into private structs
	CopyRect(&field, pField);
	player[0] = Player1;
	player[1] = Player2;
	ball = Ball;
	
	// set up bounce information
	player[0]->bounce = 0;
	player[1]->bounce = 0;

	// set up scores
	player[0]->score = 0;
	player[1]->score = 0;

	// register to subjects
	player[0]->RegisterObserver(this);
	player[1]->RegisterObserver(this);
	ball->RegisterObserver(this);

	// set up service
	SetupService(PLAYER1);
}

//----------------------------------------------------------------------------------------------
// ShowPlayerScore(): Displays the player score on the screen
//----------------------------------------------------------------------------------------------

void CTennisField::ShowPlayerScore(HDC dc, int x, int y, int PlayerId) {
	// displayed player score at (x, y)
	char s[20];	
	ltoa(player[PlayerId]->score,s,10);
    int length = strlen(s);
	SetTextColor(dc, player[PlayerId]->GetColor());
	TextOut(dc, x, y, s, length);
}

//----------------------------------------------------------------------------------------------
// ShowFPS(): Calculates the frames per second and displays the framerate on the screen.
//			  Note: all game speeds are relative to this framerate
//----------------------------------------------------------------------------------------------

void CTennisField::ShowFPS(HDC dc, int x, int y) {
	// calculate the new framerate
	nFrameCount++;
	g_FrameRate = (1000*nFrameCount) / (timeGetTime()-StartTime);
	
	// display the framerate on the screen
	char s[20] = "FPS: ";	
	ltoa(g_FrameRate,s+5,10);
    int length = strlen(s);
	TextOut(dc, x, y, s, length);	
}

//----------------------------------------------------------------------------------------------
// Update(): Updates the tennis field and all game objects registered with it
//----------------------------------------------------------------------------------------------

void CTennisField::Update() {

	Fill(NULL, 0, lpddss);
	Blit(imgField, lpddss, (SCREEN_WIDTH-FIELD_WIDTH) / 2, (SCREEN_HEIGHT-FIELD_HEIGHT) / 2);

	// update ball position
	ball->Update();

	// check for collisions between paddles and the ball
	player[0]->BallCollision(ball);
	player[1]->BallCollision(ball);

	// check for collisions between ball and net
	NetCollision();
	
	// update paddle positions
	player[0]->Update();
	player[1]->Update();
	
	// paint game objects
	player[0]->Paint(lpddss);
	player[1]->Paint(lpddss);
	ball->Paint(lpddss);

	HDC dc;
	lpddss->GetDC(&dc);

	// show scores
	SetBkColor(dc, RGB(0,0,0));
	SetTextColor(dc, RGB(255,255,255));
	TextOut(dc, 10, 140, "Score:", 6);

	// show frame rate
	ShowFPS(dc, SCREEN_WIDTH-80, SCREEN_HEIGHT-20);
	
	// show footer text
	TextOut(dc, 10, SCREEN_HEIGHT-20, "ESC - quit  F1 - (un)freeze game  F2 - toggle bot targeting  F3 - stop ball", 75);
	
	// show individual scores
	ShowPlayerScore(dc, 10, 160, 0);
	ShowPlayerScore(dc, 10, 175, 1);

	lpddss->ReleaseDC(dc);

	if (!Flip()){
		MessageBox(NULL,"flip failed","",MB_OK);
		exit(1);
	}
}

//----------------------------------------------------------------------------------------------
// BallInField(): Checks if the ball is in the play field of the specified player
//----------------------------------------------------------------------------------------------

BOOL CTennisField::BallInField(int index, POINT pt) {
	int opponent = (index+1) % 2;
	LPRECT lpRect = player[index]->GetField();

	if (PtInRect(lpRect, pt)) {
		if (ball->GetOwner() != OWNER_SERVICE) {
			player[index]->bounce++;
				
			if (player[index]->GetId()==ball->GetOwner()) {
				PlayerScores(opponent);
			} else {
				if (player[index]->bounce>1) {
					PlayerScores(opponent);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
// Notify(): Observer notification handler; the tennis field can receive notifications from
//			 the ball and the players
//----------------------------------------------------------------------------------------------

BOOL CTennisField::Notify(int msg, DWORD param) {
	if (msg == HIT_GROUND_EVENT || msg == BALL_ROLLING_EVENT) {
		POINT pt;
		ball->GetPosition(pt);

		if (!BallInField(0, pt) && !BallInField(1, pt)) {

			for (int i=0; i<2; i++) {
				int opponent = (i+1) % 2;
				if (player[i]->GetId()==ball->GetOwner()) {
					if (player[opponent]->bounce>0) {
						PlayerScores(i); break;
					} else {
						PlayerScores(opponent); break;
					}
				}
			}

		}
		if (msg == HIT_GROUND_EVENT) {
			sndBounce.Play();
		}
	}

	if (msg == HIT_BALL_EVENT) {
		player[0]->bounce = 0;
		player[1]->bounce = 0;
		sndHitBall.Play();
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------
// SetupForService(): sets up the game for the service
//----------------------------------------------------------------------------------------------

void CTennisField::SetupService(int ServicePlayer) {
	// set player positions
	for (int i=0; i<2; i++) {
		player[i]->ResetForService(ServicePlayer == i);
		player[i]->bounce = 0;
	}

	// set ball position
	RECT field;
	CopyRect(&field, player[ServicePlayer]->GetField());
	ball->SetPosition((float)(field.right+field.left) / 2, (float)(field.top+field.bottom) / 2, 0);
	ball->SetVelocity(0,0,0);
	ball->SetOwner(OWNER_SERVICE);
}

//----------------------------------------------------------------------------------------------
// PlayerScores(): called when a player scores a point
//----------------------------------------------------------------------------------------------

void CTennisField::PlayerScores(int index) {
	player[index]->score++;
	SetupService(index);
}
