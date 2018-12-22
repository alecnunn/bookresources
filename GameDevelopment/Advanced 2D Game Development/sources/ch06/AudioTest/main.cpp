/*
  Name: AudioTest
  Date: 03/2008
  Description: Demonstrates the FMOD sound system
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

//independent sample
Sample *wobble;

bool game_preload() 
{
	g_engine->setAppTitle("AUDIO TEST");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(640);
    g_engine->setScreenHeight(480);
    g_engine->setColorDepth(32);
    return true;
}

bool game_init(HWND) 
{
    g_engine->message("Press mouse buttons to hear sound clips!");

    //load sample into audio manager
    if (!g_engine->audio->Load("gong.ogg", "gong")) {
        g_engine->message("Error loading gong.ogg");
        return false;
    }

    //load sample into audio manager
    if (!g_engine->audio->Load("explosion.wav", "explosion")) {
        g_engine->message("Error loading explosion.wav");
        return false;
    }

    //load independent sample
    wobble = new Sample();
    wobble = g_engine->audio->Load("wobble.wav");
    if (!wobble) {
        g_engine->message("Error loading wobble.wav");
        return false;
    }

    return true;
}

void game_keyRelease(int key)
{
    if (key == DIK_ESCAPE) g_engine->Close();
}

void game_mouseButton(int button)
{
    switch(button) {
        case 0: 
            //play gong sample stored in audio manager
            g_engine->audio->Play("gong");            
            break;

        case 1:
            //play explosion sample stored in audio manager
            g_engine->audio->Play("explosion");
            break;

        case 2:
            //play woggle sample stored independently
            g_engine->audio->Play(wobble);
            break;
    }
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(100,0,0));
}

void game_end() 
{
    g_engine->audio->StopAll();
    delete wobble;
}

//unused game events
void game_update() { }
void game_keyPress(int key) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_render2d() { }

