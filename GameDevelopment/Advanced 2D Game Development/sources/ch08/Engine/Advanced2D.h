// Advanced2D Engine
// Main header file

#pragma once

#define WIN32_EXTRA_LEAN
#include <windows.h>

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define DIRECTINPUT_VERSION 0x0800
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <dinput.h>

#include "Timer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Light.h"
#include "Vector3.h"
#include "Sprite.h"
#include "ParticleEmitter.h"
#include "Input.h"
#include "fmod.hpp"
#include "Audio.h"
#include "Entity.h"
#include "Font.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define REVISION 0

//external variables and functions
extern bool gameover;

extern bool game_preload();
extern bool game_init(HWND);
extern void game_update();
extern void game_end();
extern void game_render3d();
extern void game_render2d();

extern void game_keyPress(int);
extern void game_keyRelease(int);
extern void game_mouseButton(int);
extern void game_mouseMotion(int,int);
extern void game_mouseMove(int,int);
extern void game_mouseWheel(int);

extern void game_entityUpdate(Advanced2D::Entity*);
extern void game_entityRender(Advanced2D::Entity*);

//macro to read the keyboard asynchronously
#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)


namespace Advanced2D 
{
	class Engine {
	private:
		int p_versionMajor, p_versionMinor, p_revision;
		HWND p_windowHandle;
		LPDIRECT3D9 p_d3d;
		LPDIRECT3DDEVICE9 p_device;
		LPDIRECT3DSURFACE9 p_backbuffer;
		LPD3DXSPRITE p_sprite_handler;
		std::string p_apptitle;
		bool p_fullscreen;
		int p_screenwidth;
		int p_screenheight;
		int p_colordepth;
		bool p_pauseMode;
		D3DCOLOR p_ambientColor;
		bool p_maximizeProcessor;
		Timer p_coreTimer;
		long p_frameCount_core;
		long p_frameRate_core;
		Timer p_realTimer;
		long p_frameCount_real;
		long p_frameRate_real;

		Input *p_input;
		void UpdateKeyboard();
		void UpdateMouse();

		void TestForCollisions();
		void UpdateEntities();
		void Draw2DEntities();
		void Draw3DEntities();
		void BuryEntities();

		std::list<Entity*> p_entities;

	public:
		Engine();
		virtual ~Engine();
		int Init(int width, int height, int colordepth, bool fullscreen);
		void Close();
		void Update();
		void message(std::string message, std::string title = "ADVANCED 2D");
		void fatalerror(std::string message, std::string title = "FATAL ERROR");
		void Shutdown();
		void ClearScene(D3DCOLOR color);
		void SetDefaultMaterial();
		void SetAmbient(D3DCOLOR colorvalue);
		void SetIdentity();		
		int RenderStart();
		int RenderStop();
		int Render2D_Start();
		int Render2D_Stop();
		int Release();

		//simplified public Audio object
		Audio *audio;

		//accessor/mutator functions expose the private variables
		bool isPaused() { return this->p_pauseMode; }
		void setPaused(bool value) { this->p_pauseMode = value; }

		LPDIRECT3DDEVICE9 getDevice() { return this->p_device; }

		LPDIRECT3DSURFACE9 getBackBuffer() { return this->p_backbuffer; }

		LPD3DXSPRITE getSpriteHandler() { return this->p_sprite_handler; }

		void setWindowHandle(HWND hwnd) { this->p_windowHandle = hwnd; }
		HWND getWindowHandle() { return this->p_windowHandle; }

		std::string getAppTitle() { return this->p_apptitle; }
		void setAppTitle(std::string value) { this->p_apptitle = value; }

		int getVersionMajor() { return this->p_versionMajor; }
		int getVersionMinor() { return this->p_versionMinor; }
		int getRevision() { return this->p_revision; }
		std::string getVersionText();

		long getFrameRate_core() { return this->p_frameRate_core; };
		long getFrameRate_real() { return this->p_frameRate_real; };

		int getScreenWidth() { return this->p_screenwidth; }
		void setScreenWidth(int value) { this->p_screenwidth = value; }
		int getScreenHeight() { return this->p_screenheight; }
		void setScreenHeight(int value) { this->p_screenheight = value; }

		int getColorDepth() { return this->p_colordepth; }
		void setColorDepth(int value) { this->p_colordepth = value; }

		bool getFullscreen() { return this->p_fullscreen; }
		void setFullscreen(bool value) { this->p_fullscreen = value; }

		bool getMaximizeProcessor() { return this->p_maximizeProcessor; }
		void setMaximizeProcessor(bool value) { this->p_maximizeProcessor = value; }


		std::list<Entity*> getEntityList() { return p_entities; }
		long getEntityCount() { return (long)p_entities.size(); }
		void addEntity(Entity *entity);
		Entity *findEntity(std::string name);
		Entity *findEntity(int objectType);

		
	}; //class


}; //namespace

 
//define the global engine object (visible everywhere!)
extern Advanced2D::Engine *g_engine;

