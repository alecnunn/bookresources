// Advanced2D Engine
// Main source code file

//includes
#include "Advanced2D.h"

namespace Advanced2D
{
	Engine::Engine()
	{
		srand((unsigned int)time(NULL));
		p_maximizeProcessor = false;
		p_frameCount_core = 0;
		p_frameRate_core = 0;
		p_frameCount_real = 0;
		p_frameRate_real = 0;
		p_ambientColor = D3DCOLOR_RGBA(255,255,255, 0);
		p_windowHandle = 0;
		p_pauseMode = false;
		p_versionMajor = VERSION_MAJOR;
		p_versionMinor = VERSION_MINOR;
		p_revision = REVISION;
		
		//set default values
		this->setAppTitle("Advanced2D");
		this->setScreenWidth(640);
		this->setScreenHeight(480);
		this->setColorDepth(32);
		this->setFullscreen(false);
		
		//window handle must be set later on for DirectX!
		this->setWindowHandle(0);
	
	}
	
	Engine::~Engine()
	{
	   if (this->p_device) this->p_device->Release();
	   if (this->p_d3d) this->p_d3d->Release();
	}
	
	std::string Engine::getVersionText()
	{
		std::ostringstream s;
		s << "Advanced2D Engine v" << p_versionMajor << "." << p_versionMinor << "." << p_revision;
		return s.str();
	}
	
	void Engine::message(std::string message, std::string title)
	{
		MessageBox(0, message.c_str(), title.c_str(), 0);
	}
	
	void Engine::fatalerror(std::string message, std::string title)
	{
		this->message(message,title);
		Shutdown();
	}
	
	int Engine::Init(int width, int height, int colordepth, bool fullscreen)
	{
	   //initialize Direct3D
		this->p_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (this->p_d3d == NULL) {
			return 0;
		}
	
		//get system desktop color depth
		D3DDISPLAYMODE dm;
		this->p_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
	
		//set configuration options for Direct3D
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = (!fullscreen);
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.BackBufferFormat = dm.Format;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.hDeviceWindow = p_windowHandle;
	
		//create Direct3D device
		this->p_d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			this->p_windowHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp,
			&this->p_device);
	
		if (this->p_device == NULL) return 0;
	
		//clear the backbuffer to black
		this->ClearScene(D3DCOLOR_XRGB(0,0,0));
	
		//create pointer to the back buffer
		this->p_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->p_backbuffer);
	
	    //use ambient lighting and z-buffering
		this->p_device->SetRenderState(D3DRS_ZENABLE, TRUE);
		this->p_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        this->p_device->SetRenderState(D3DRS_LIGHTING, true);
		this->SetAmbient(this->p_ambientColor);
	
		//initialize 2D renderer
		HRESULT result = D3DXCreateSprite(this->p_device, &this->p_sprite_handler);
		if (result != D3D_OK) return 0;
	
		
		//call game initialization extern function
		if (!game_init(this->getWindowHandle()))
			return 0;
	
		//set a default material
		SetDefaultMaterial();
	
		return 1;
	}
	
    void Engine::SetIdentity()
    {
    	D3DXMATRIX matrixWorld;
        D3DXMatrixIdentity(&matrixWorld);
        g_engine->getDevice()->SetTransform(D3DTS_WORLD, &matrixWorld);
    }
	
//***** ADD AMBIENT CODE TO CHAPTER 1 LISTINGS
	void Engine::SetDefaultMaterial()
	{
	    D3DMATERIAL9 mat;
		memset(&mat, 0, sizeof(mat));
		mat.Diffuse.r = mat.Ambient.r = 1.0f;
		mat.Diffuse.g = mat.Ambient.g = 1.0f;
		mat.Diffuse.b = mat.Ambient.b = 1.0f;
		mat.Diffuse.a = mat.Ambient.a = 1.0f;
		p_device->SetMaterial(&mat);
	}
	
	
	void Engine::ClearScene(D3DCOLOR color)
	{
		this->p_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
	}
	
	void Engine::SetAmbient(D3DCOLOR colorvalue)
	{
		this->p_ambientColor = colorvalue;
		this->p_device->SetRenderState(D3DRS_AMBIENT, this->p_ambientColor);
	}
	
	
	int Engine::RenderStart()
	{
		if (!this->p_device) return 0;
		if (this->p_device->BeginScene() != D3D_OK) return 0;
		return 1;
	}
	
	int Engine::RenderStop()
	{
	   if (!this->p_device) return 0;
	   if (this->p_device->EndScene() != D3D_OK) return 0;
	   if (p_device->Present(NULL, NULL, NULL, NULL) != D3D_OK) return 0;
	   return 1;
	}
	

	int Engine::Render2D_Start()
	{
		if (p_sprite_handler->Begin(D3DXSPRITE_ALPHABLEND) != D3D_OK)
			return 0;
		else
			return 1;
	}
	
	int Engine::Render2D_Stop()
	{
	   p_sprite_handler->End();
	   return 1;
	}

	void Engine::Shutdown()
	{
		gameover = true;
	}
	
	void Engine::Update()
	{
		static Timer timedUpdate;
	
		//calculate core framerate
		p_frameCount_core++;
		if (p_coreTimer.stopwatch(999)) {
			p_frameRate_core = p_frameCount_core;
			p_frameCount_core = 0;
		}
	
		//fast update with no timing
		game_update();
	
		//update with 60fps timing
		if (!timedUpdate.stopwatch(14)) {
			if (!this->getMaximizeProcessor()) 
			{
				Sleep(1);
			}
		}
		else {
			//calculate real framerate
			p_frameCount_real++; 
			if (p_realTimer.stopwatch(999)) {
				p_frameRate_real = p_frameCount_real;
				p_frameCount_real = 0;
			}
	
			//begin rendering
			this->RenderStart();

            //let game do it's own 3D
            this->SetIdentity();
            game_render3d();

            //2D rendering
			Render2D_Start();
			game_render2d();
			Render2D_Stop();
        			
			//done rendering
			this->RenderStop();
		}
	
	}

//****** CHANGE BACK IN CHAPTER 1 
	void Engine::Close()
	{
		try {
			gameover = true;
		}
		catch (...) { }
	}
	

} //namespace
