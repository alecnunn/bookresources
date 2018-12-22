#include "standard_headers.hpp"
#include "ai_app.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CAIApp* app = new CAIApp;

	app->Create(hInstance);
	app->Run();
		
	delete app;

	return(0);
}


