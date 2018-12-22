//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Tennis.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ball.h"
#include "gamer.h"
#include "aibot.h"
#include "patternbot.h"
#include "soundbuffer.h"
#include "tennisfield.h"

//----------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------

HWND			g_hWnd;
BOOL			g_bActive;
BOOL			g_Freeze;
BOOL			g_Intro;
DWORD			g_FrameRate;
DWORD			g_TargetSpeed;
BOOL			g_bBotTargeting;
LPDDS			imgField;
LPDDS			imgRedCross;
LPDDS			imgIntro;
CTennisBall		ball;
CAiBot			bot;
CTennisField	TennisField;
CSoundBuffer	sndHitBall;
CSoundBuffer	sndBounce;

//----------------------------------------------------------------------------------------------
// The gamer object can be replaced by a pattern bot, which is a bot that can generate
// patterns in its behavior. The pattern bot is excellent for making visible the pattern 
// recognition capabilities of sequential prediction. Just define PATTERNBOT_MOVIE, get some
// popcorn, sit back and relax and watch the show! ;-)
//----------------------------------------------------------------------------------------------

//#define PATTERNBOT_MOVIE

#ifdef PATTERNBOT_MOVIE

CPatternBot		gamer;

#else

CGamer			gamer;

#endif // PATTERNBOT_MOVIE

//----------------------------------------------------------------------------------------------
// SetupSurfaces(): Creates the DirectDraw Surfaces
//----------------------------------------------------------------------------------------------

void SetupSurfaces() {
	CreateSurface(imgField, FIELD_WIDTH, FIELD_HEIGHT);
	CreateSurface(imgRedCross, CROSS_WIDTH, CROSS_HEIGHT);
}

//----------------------------------------------------------------------------------------------
// SetupSurfaces(): Loads bitmaps into the DirectDraw surfaces
//----------------------------------------------------------------------------------------------

void LoadSurfaces() {
	LoadSurface("tennisfield.bmp", 0, 0, imgField);
	LoadSurface("cross.bmp", 0, 0, imgRedCross);	
}

//----------------------------------------------------------------------------------------------
// RestoreAll(): Callback function for my DirectDraw library; restores lost surfaces
//----------------------------------------------------------------------------------------------

BOOL RestoreAll(BOOL reinit) {
	if (reinit) {
		BOOL result;
		ShutdownDD();
		result = InitDD(g_hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 1);
		SetupSurfaces();
		return result;
	}
	LoadSurfaces();
	return TRUE;
}

//----------------------------------------------------------------------------------------------
// LoadSamples(): Loads wave files into DirectSound buffers
//----------------------------------------------------------------------------------------------

void LoadSamples() {
	sndHitBall.CreateWave(lpds, "hitball.wav");
	sndBounce.CreateWave(lpds, "bounce.wav");
}

//----------------------------------------------------------------------------------------------
// GameInit(): Initializes the game
//----------------------------------------------------------------------------------------------

void GameInit(HWND hWnd, HINSTANCE hInstance) {	

	// initialize global variables
	g_hWnd = hWnd;
	g_Freeze = FALSE;
	g_Intro = TRUE;
	g_FrameRate = DEFAULT_SPEED;
	g_TargetSpeed = DEFAULT_SPEED;
	g_bBotTargeting = FALSE;

	// initialize DirectDraw
	if (!InitDD(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 1)) {
		MessageBox(NULL, "Failed to initialize DirectDraw!","Error", 0);
		return;
	}

	// set up DirectDraw surfaces and display the intro screen
	SetupSurfaces();
	LoadSurfaces();
	LoadSurface("intro.bmp", 0, 0, lpddss);	
	Flip();

	// initialize DirectSound
	if (!InitDS()) {
		MessageBox(NULL, "Failed to initialize DirectSound!", "Error", 0);
		return;
	}

	// set up DirectSound buffers
	LoadSamples();
			
	RECT field;

	// set up bot play field
	field.left = (SCREEN_WIDTH-FIELD_WIDTH) / 2 + OUTBOX_WIDTH;
	field.right = SCREEN_WIDTH - (SCREEN_WIDTH-FIELD_WIDTH) / 2 - OUTBOX_WIDTH;
	field.top = (SCREEN_HEIGHT-FIELD_HEIGHT) / 2;
	field.bottom = SCREEN_HEIGHT / 2;

	// set up bot settings
	bot.SetPlayField(&field);
	bot.SetColor(RGB(0,0,255));
	bot.SetDirection(PADDLE_DOWN);
	bot.SetupAI(&ball, g_bBotTargeting);

	// set up gamer play field
	field.left = (SCREEN_WIDTH-FIELD_WIDTH) / 2 + OUTBOX_WIDTH;
	field.right = SCREEN_WIDTH - (SCREEN_WIDTH-FIELD_WIDTH) / 2 - OUTBOX_WIDTH;
	field.top = SCREEN_HEIGHT / 2; 
	field.bottom = SCREEN_HEIGHT-(SCREEN_HEIGHT-FIELD_HEIGHT) / 2;

	// set up gamer settings
	gamer.SetPlayField(&field);
	gamer.SetColor(RGB(255,0,0));	
	gamer.SetDirection(PADDLE_UP);	

#ifdef PATTERNBOT_MOVIE

	gamer.SetupAI(&ball, TRUE);
	gamer.SetPatternSize(3);

#endif

	// set up entire tennis field
	field.left = (SCREEN_WIDTH-FIELD_WIDTH) / 2;
	field.right = SCREEN_WIDTH - (SCREEN_WIDTH-FIELD_WIDTH) / 2;
	field.top = (SCREEN_HEIGHT-FIELD_HEIGHT) / 2;
	field.bottom = SCREEN_HEIGHT-(SCREEN_HEIGHT-FIELD_HEIGHT) / 2;

	// plug objects into "manager" object
	TennisField.SetupField(&field, &gamer, &bot, &ball);	
}

//----------------------------------------------------------------------------------------------
// GameMain(): Main game loop (called from the message loop)
//----------------------------------------------------------------------------------------------

void GameMain() {	
	if (!g_Freeze && !g_Intro) {
		TennisField.Update();
	}
}

//----------------------------------------------------------------------------------------------
// GameShutDown(): Destroys the game objects and releases allocated memory
//----------------------------------------------------------------------------------------------

void GameShutdown() {
	sndHitBall.Release();
	sndBounce.Release();
	ShutdownDS();
	imgField->Release();
	imgRedCross->Release();
	ShutdownDD();
}

//----------------------------------------------------------------------------------------------
// WindowProc(): Window handler 
//----------------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hWnd, 
						    UINT Msg, 
                            WPARAM wParam, 
                            LPARAM lParam) 
{
	switch(Msg) {	

        case WM_ACTIVATEAPP:
            g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
            break;

		case WM_SETCURSOR:
            SetCursor(NULL);
            break;

		case WM_PAINT:
			break;

		case WM_KEYDOWN:
			if (g_Intro) {
				g_Intro = FALSE;
				TennisField.Start();
			} else {
				int nVirtKey = (int)wParam;
				switch (nVirtKey) {
				case VK_F1: 
					g_Freeze = !g_Freeze;
					if (!g_Freeze) {
						TennisField.Start(); // restart frame-rate counter
					}
					break;
				case VK_F2:
					g_bBotTargeting = !g_bBotTargeting;
					bot.EnableTargeting(g_bBotTargeting);
					break;
				case VK_F3:
					ball.SetVelocity(0,0,0);
					break;
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
				}
			}
			break;

		case WM_DESTROY: 
			PostQuitMessage(0);			
			break;

		default:
			break;
    }

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//----------------------------------------------------------------------------------------------
// WinMain(): Entry point for the application
//----------------------------------------------------------------------------------------------

int APIENTRY WinMain(	HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPSTR lpCmdLine,
						int nCmdShow)
{

	WNDCLASS	wc;		
	MSG			msg;
	HWND		hWnd;

	// set up the window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass(&wc);

	// create the window
    hWnd = CreateWindowEx(WS_EX_TOPMOST,
                          NAME,
                          TITLE,
                          WS_POPUP,
                          0,
                          0,
                          SCREEN_WIDTH,
                          SCREEN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    if (!hWnd)
        return FALSE;

	// do some other window stuff
	ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);
	ShowCursor(FALSE);

	// initialize the game
	GameInit(hWnd, hInstance);
	
	// run the message loop
	while (TRUE) {
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { 
			if (msg.message == WM_QUIT)
				break;
	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    
		if (g_bActive) {
			GameMain();
		} else {
			WaitMessage();
		}
	}


	// shut down the game
	GameShutdown();

	// release the window
	DestroyWindow(hWnd);

	return (msg.wParam);
}
