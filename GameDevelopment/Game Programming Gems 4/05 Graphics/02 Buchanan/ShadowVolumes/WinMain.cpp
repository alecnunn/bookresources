#include <windows.h>
#include <math.h>


#include "Display.hpp"
#include "Scene.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Mesh.hpp"


/*
	This program shows how to create shadow volumes for non-closed meshes using
	vertex shader 1.1 graphics hardware.  The mesh first needs to be pre-processed
	which is done in the Mesh class.  The actual drawing of the shadow volumes
	is performed by the Scene class.  A depth fail technique is used in this sample
	but it is not robust in all cases.

	Code of interest is:

	Mesh::processForShadowVolumes()
	Scene::draw()
	ShadowVolumeSides*.vs files

	You may use this code as you wish.

	Warrick Buchanan 21/8/2003


	Special thanks to:

		Nicolas Thibieroz (PowerVR)
		Tobias Barendt (Deibus)
		Alex Vlachos (ATI)
		Tom Forsyth (Mucky Foot)
*/


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showState)
{
	Display display("GPG4::ShadowVolumes", 640, 480);
	Mesh teapot(display.device, "teapot.x", 0.05f);
	Mesh room(display.device, "room.x", 0.05f);
	Scene scene(display);
	Light light[3];

	
	//	Add meshes to the scene
	scene.meshes.push_back(&room);
	scene.meshes.push_back(&teapot);
	
	//	Add lights to the scene
	scene.lights.push_back(&light[0]);
	scene.lights.push_back(&light[1]);
	scene.lights.push_back(&light[2]);
	

	//	Village idiots who set ambient light values of greater than 0.3f will be terminated...
	scene.ambient = D3DXVECTOR4(0.01f, 0.01f, 0.01f, 0.01f);
	

	MSG message;
	memset(&message, 0, sizeof(message));


	while(message.message != WM_QUIT)
	{
		if(PeekMessage(&message, 0, 0, 0, PM_NOREMOVE))
		{
			if(!GetMessage(&message, 0, 0, 0 )) break;
			
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			//	Some values to animate the scene with

			float t = 2.0f*D3DX_PI*float(GetTickCount()%10000)/10000.0f;
			float t2 = 2.0f*D3DX_PI*float(GetTickCount()%50000)/50000.0f;
			

			//	Set the lights up
			
			light[0].position = D3DXVECTOR3(1000.0f*sinf(t), 0.0f, 1000.0f*cosf(t));
			light[0].colour = D3DXVECTOR4(0.4f, 0.4f, 0.4f, 1.0f);
			light[0].range = 15.0f;
			
			light[1].position = D3DXVECTOR3(0.0f, 1000.0f*sinf(t), 1000.0f*cosf(t));
			light[1].colour = D3DXVECTOR4(0.0f, 0.4f, 0.0f, 1.0f);
			light[1].range = 15.0f;
			
			light[2].position = D3DXVECTOR3(1000.0f*cosf(t), 1000.0f*sinf(t), 0.0f);
			light[2].colour = D3DXVECTOR4(0.4f, 0.0f, 0.0f, 1.0f);
			light[2].range = 15.0f;
			

			//	Set transforms
	
			D3DXMATRIX translation, rotation;
			
			D3DXMatrixIdentity(&translation);
			D3DXMatrixIdentity(&rotation);
			
			D3DXMatrixRotationY(&rotation, t2);
			D3DXMatrixTranslation(&translation, 0, 0, 10);
			D3DXMatrixMultiply(display.view->GetTop(), &rotation, &translation);
			
			D3DXMatrixMultiply(&display.worldView, display.world->GetTop(), display.view->GetTop());
			D3DXMatrixMultiply(&display.worldViewProjection, &display.worldView, display.projection->GetTop());
			
			display.device->SetTransform(D3DTS_PROJECTION, display.projection->GetTop());
			display.device->SetTransform(D3DTS_WORLD, display.world->GetTop());
			display.device->SetTransform(D3DTS_VIEW, display.view->GetTop());


			//	Draw the scene

			display.device->BeginScene();
			display.device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x000000ff, 1.0f, 127);

			scene.draw();

			display.device->EndScene();
			display.device->Present(0, 0, 0, 0);
		}
	}


	//	RxOAR!
	return (int)message.wParam;
}