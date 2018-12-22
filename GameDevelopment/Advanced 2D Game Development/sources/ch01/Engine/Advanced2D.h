// Advanced2D Engine
// Main header file

#ifndef _ADVANCED2D_H
#define _ADVANCED2D_H 1

#include <iostream>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include "Timer.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define REVISION 0


//external variables and functions
extern bool gameover;
extern int game_preload();
extern int game_init(HWND);
extern void game_update();
extern void game_end();

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
		int RenderStart();
		int RenderStop();
		int Release();

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
		
	}; //class


}; //namespace

 
//define the global engine object (visible everywhere!)
extern Advanced2D::Engine *g_engine;

#endif

