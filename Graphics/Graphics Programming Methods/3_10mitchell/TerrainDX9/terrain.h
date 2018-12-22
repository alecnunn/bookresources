// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef APPLICATION_H
#define APPLICATION_H

#ifndef DEBUG_H
#include "debug.h"
#endif

#ifndef SCENE_H
#include "scene.h"
#endif

#ifndef INPUT_H
#include "input.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef D3DFONT_H
#include "d3dfont.h"
#endif

class Application
{
public:

	static bool Setup();
	static void Shutdown();

	static void Run();

	static Camera_Class* Get_Camera() { return Camera; }
	static Input_Class*	Get_Input() { return Input; }
	static CD3DFont*		Get_Font() { return Font; }

private:

	static void Process();
	static void Render();

	static void Process_Input_Msg(UINT msg, WPARAM wParam);

	// windows data
	static WNDCLASS			WndClass;

	// d3d object
	static LPDIRECT3D9		D3D;

	static Input_Class*		Input;
	static Camera_Class*		Camera;
	static Scene_Class*		Scene;
	static CD3DFont*			Font;

	// windows message handler
	friend LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif /* APPLICATION_H */