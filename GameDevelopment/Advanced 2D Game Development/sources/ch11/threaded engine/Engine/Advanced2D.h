// Advanced2D Engine
// Main header file

#pragma once
//#ifndef _ADVANCED2D_H
//#define _ADVANCED2D_H 1

//*****ADD TO CHAPTER 1 LISTING
#define WIN32_EXTRA_LEAN
#include <windows.h>

//****CHAPTER 7 NOTE: winmain.h was removed
//#include "winmain.h"

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
#include "Console.h"
#include "Rect.h"
#include "Math.h"
#include <pthread.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define REVISION 0

//external variables and functions
extern bool gameover;

//*****CHANGE INT TO BOOL IN CHAPTER 1 LISTING
extern bool game_preload();
extern bool game_init(HWND);
extern void game_update();
extern void game_end();
extern void game_render3d();
extern void game_render2d();

//****CHAPTER 5
extern void game_keyPress(int);
extern void game_keyRelease(int);
extern void game_mouseButton(int);
extern void game_mouseMotion(int,int);
extern void game_mouseMove(int,int);
extern void game_mouseWheel(int);

//****CHAPTER 7
extern void game_entityUpdate(Advanced2D::Entity*);
extern void game_entityRender(Advanced2D::Entity*);

//add this one in chapter 9
extern void game_entityCollision(Advanced2D::Entity*,Advanced2D::Entity*);


//*****ADD TO CHAPTER 1
//macro to read the keyboard asynchronously
#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)


namespace Advanced2D 
{
//****CHAPTER 11
	//thread stuff
	//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	void* thread_function_bury_entities(void* data);
	

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
//****MOVED HERE FROM CPP FILE IN CHAPTER 9
		Timer timedUpdate;
//*****ADDED IN CHAPTER 9
		Timer collisionTimer;


//****ADDED IN CHAPTER 5
		Input *p_input;
		void UpdateKeyboard();
		void UpdateMouse();


//*****ADDED IN CHAPTER 7
		void UpdateEntities();
		void Draw2DEntities();
		void Draw3DEntities();
		void BuryEntities();

//****ADDED IN CHAPTER 9
		bool collision(Sprite *sprite1, Sprite *sprite2);
		bool collisionBR(Sprite *sprite1, Sprite *sprite2);
		bool collisionD(Sprite *sprite1, Sprite *sprite2);
		void TestForCollisions();

		std::list<Entity*>* p_entities;


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

		//public Audio object
		Audio *audio;

//CHAPTER 10
		//public Math object
		Math *math;		


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


//******NEED TO ADD
//**CHAPTER 11 -- the entity list is now created on heap! update this in ch7
		std::list<Entity*>* getEntityList() { return p_entities; }
		long getEntityCount() { return (long)p_entities->size(); }
		void addEntity(Entity *entity);
		Entity *findEntity(std::string name);
		Entity *findEntity(int objectType);
//****ADDED IN CHAPTER 10
		int getEntityCount(int objectType);

		//exposed to public to simplify code
		pthread_mutex_t mutex;
		
	}; //class


}; //namespace

 
//define the global engine object (visible everywhere!)
extern Advanced2D::Engine *g_engine;

//#endif
