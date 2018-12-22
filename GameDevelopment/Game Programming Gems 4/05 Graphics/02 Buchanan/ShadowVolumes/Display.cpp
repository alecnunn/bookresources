#include "Display.hpp"


Display::Display(const char* const name, unsigned width, unsigned height)

	:	depthStencil(0),
		surface(0),
		device(0),
		direct3d(0),
		windowHandle(0),
		height(0),
		width(0)
{
	open(name, width, height);
}


Display::Display()

	:	depthStencil(0),
		surface(0),
		device(0),
		direct3d(0),
		windowHandle(0),
		height(0),
		width(0)
{
}


Display::~Display()
{
	close();
}


void Display::open(const char* const name, unsigned width, unsigned height)
{
	HRESULT error = D3D_OK;


	WNDCLASSEX windowClass =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		windowHandler,
		0,
		0, 
		GetModuleHandle(0),
		0,
		0,
		0,
		0,
		name,
		0
	};


	RegisterClassEx(&windowClass);


	windowHandle = CreateWindow(name,
								name, 
								WS_OVERLAPPEDWINDOW,
								100,
								100,
								width + 8,
								height + 34,
								GetDesktopWindow(),
								0,
								windowClass.hInstance,
								0);
	assert(windowHandle);


	direct3d = Direct3DCreate9(D3D_SDK_VERSION);
	assert(direct3d);


	ZeroMemory(&presentParameters, sizeof(presentParameters));
	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;


	error = direct3d->CreateDevice(	D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									windowHandle,
									D3DCREATE_HARDWARE_VERTEXPROCESSING,
									&presentParameters,
									&device);

	if(error != D3D_OK)
	{
		error = direct3d->CreateDevice(	D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										windowHandle,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										&presentParameters,
										&device);


		assert(error == D3D_OK);
	}


	ZeroMemory(&capabilities, sizeof(capabilities));
	device->GetDeviceCaps(&capabilities);


	ShowWindow(windowHandle, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle);


	error = D3DXCreateMatrixStack(0, &projection);
	assert(error == D3D_OK);


	error = D3DXCreateMatrixStack(0, &world);
	assert(error == D3D_OK);


	error = D3DXCreateMatrixStack(0, &view);
	assert(error == D3D_OK);


	projection->LoadIdentity();
	world->LoadIdentity();
	view->LoadIdentity();


	D3DXMatrixPerspectiveFovLH(projection->GetTop(), D3DX_PI/4, float(height)/float(width), 1.0f, 100.f);


	device->SetTransform(D3DTS_PROJECTION, projection->GetTop());
	device->SetTransform(D3DTS_WORLD, projection->GetTop());
	device->SetTransform(D3DTS_VIEW, projection->GetTop());


	device->GetDepthStencilSurface(&depthStencil);
	device->GetRenderTarget(0, &surface);


	for(DWORD stage = 0; stage < capabilities.MaxSimultaneousTextures; stage++)
	{
		device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); 
		device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}


	device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);


	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	device->SetRenderState(D3DRS_LIGHTING, FALSE);


	Display::height = height;
	Display::width = width;
}


void Display::close()
{
	flush();


	if(device)
	{
		if(depthStencil)
		{
			depthStencil->Release();
			depthStencil = 0;
		}


		if(surface)
		{
			surface->Release();
			surface = 0;
		}


		for(unsigned index = 0; index < capabilities.MaxSimultaneousTextures; index++)
		{
			device->SetTexture(index, 0);
		}


		for(index = 0; index < capabilities.MaxStreams; index++)
		{
			device->SetStreamSource(index, 0, 0, 0);
		}


		device->SetIndices(0);


		device->Release();
		device = 0;
	}


	if(direct3d)
	{
		direct3d->Release();
		device = 0;
	}


	UnregisterClass("Display", GetModuleHandle(0));
}


LPDIRECT3DVERTEXSHADER9 Display::getVertexShader(const char* const name)
{
	std::list<CachedVertexShader>::iterator cachedVertexShader;
	HRESULT error = D3D_OK;


	for(cachedVertexShader = cachedVertexShaders.begin(); cachedVertexShader != cachedVertexShaders.end(); cachedVertexShader++)
	{
		if(cachedVertexShader->name == std::string(name))
		{
			return cachedVertexShader->vertexShader;
		}
	}


	LPDIRECT3DVERTEXSHADER9 vertexShader = 0;
	LPD3DXBUFFER errors = 0;
	LPD3DXBUFFER code = 0;


	error = D3DXAssembleShaderFromFile(name, 0, 0, 0, &code, &errors);


	if(error != D3D_OK)
	{
		if(errors)
		{
			MessageBox(0, (char*)errors->GetBufferPointer(), "Error assembling vertex shader", MB_OK);
			errors->Release();
		}


		return 0;
	}


	error = device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertexShader);
	assert(error == D3D_OK);


	if(errors) errors->Release();
	code->Release();
	

	cachedVertexShaders.push_back(CachedVertexShader());

	cachedVertexShaders.back().name = std::string(name);
	cachedVertexShaders.back().vertexShader = vertexShader;
	
	vertexShader->AddRef();


	return vertexShader;
}


LPDIRECT3DPIXELSHADER9 Display::getPixelShader(const char* const name)
{
	std::list<CachedPixelShader>::iterator cachedPixelShader;
	HRESULT error = D3D_OK;


	for(cachedPixelShader = cachedPixelShaders.begin(); cachedPixelShader != cachedPixelShaders.end(); cachedPixelShader++)
	{
		if(cachedPixelShader->name == std::string(name))
		{
			cachedPixelShader->pixelShader->AddRef();


			return cachedPixelShader->pixelShader;
		}
	}


	LPDIRECT3DPIXELSHADER9 pixelShader = 0;
	LPD3DXBUFFER errors = 0;
	LPD3DXBUFFER code = 0;


	error = D3DXAssembleShaderFromFile(name, 0, 0, 0, &code, &errors);


	if(error != D3D_OK)
	{
		if(errors)
		{
			MessageBox(0, (char*)errors->GetBufferPointer(), "Error assembling pixel shader", MB_OK);
			errors->Release();
		}


		return 0;
	}


	error = device->CreatePixelShader((DWORD*)code->GetBufferPointer(), &pixelShader);
	assert(error == D3D_OK);


	if(errors) errors->Release();
	code->Release();


	cachedPixelShaders.push_back(CachedPixelShader());

	cachedPixelShaders.back().name = std::string(name);
	cachedPixelShaders.back().pixelShader = pixelShader;
	
	pixelShader->AddRef();


	return pixelShader;
}


LPDIRECT3DTEXTURE9 Display::getTexture(const char* const name)
{
	std::list<CachedTexture>::iterator cachedTexture;
	LPDIRECT3DTEXTURE9 texture = 0;
	HRESULT error = D3D_OK;


	for(cachedTexture = cachedTextures.begin(); cachedTexture != cachedTextures.end(); cachedTexture++)
	{
		if(cachedTexture->name == std::string(name))
		{
			cachedTexture->texture->AddRef();


			return cachedTexture->texture;
		}
	}


	error = D3DXCreateTextureFromFile(device, name, &texture);
	if(error != D3D_OK) return 0;


	cachedTextures.push_back(CachedTexture());

	cachedTextures.back().name = std::string(name);
	cachedTextures.back().texture = texture;
	
	texture->AddRef();


	return texture;
}


LPD3DXEFFECT Display::getEffect(const char* const name)
{
	std::list<CachedEffect>::iterator cachedEffect;
	LPD3DXEFFECT effect = 0;
	HRESULT error = D3D_OK;


	for(cachedEffect = cachedEffects.begin(); cachedEffect != cachedEffects.end(); cachedEffect++)
	{
		if(cachedEffect->name == std::string(name))
		{
			cachedEffect->effect->AddRef();


			return cachedEffect->effect;
		}
	}


	error = D3DXCreateEffectFromFile(device, name, 0, 0, 0, 0, &effect, 0);
	if(error != D3D_OK) return 0;


	cachedEffects.push_back(CachedEffect());

	cachedEffects.back().name = std::string(name);
	cachedEffects.back().effect = effect;
	
	effect->AddRef();


	return effect;
}


void Display::flushVertexShaders()
{
	std::list<CachedVertexShader>::iterator cachedVertexShader;


	for(cachedVertexShader = cachedVertexShaders.begin(); cachedVertexShader != cachedVertexShaders.end(); cachedVertexShader++)
	{
		cachedVertexShader->vertexShader->Release();
	}


	cachedVertexShaders.clear();
}


void Display::flushPixelShaders()
{
	std::list<CachedPixelShader>::iterator cachedPixelShader;


	for(cachedPixelShader = cachedPixelShaders.begin(); cachedPixelShader != cachedPixelShaders.end(); cachedPixelShader++)
	{
		cachedPixelShader->pixelShader->Release();
	}


	cachedPixelShaders.clear();
}


void Display::flushTextures()
{
	std::list<CachedTexture>::iterator cachedTexture;


	for(cachedTexture = cachedTextures.begin(); cachedTexture != cachedTextures.end(); cachedTexture++)
	{
		cachedTexture->texture->Release();
	}


	cachedTextures.clear();
}


void Display::flushEffects()
{
	std::list<CachedEffect>::iterator cachedEffect;


	for(cachedEffect = cachedEffects.begin(); cachedEffect != cachedEffects.end(); cachedEffect++)
	{
		cachedEffect->effect->Release();
	}


	cachedEffects.clear();
}


void Display::flush()
{
	flushVertexShaders();
	flushPixelShaders();
	flushTextures();
	flushEffects();
}


bool Display::isValid()
{
	return windowHandle && direct3d && device;
}


LRESULT WINAPI Display::windowHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
				case WM_DESTROY:	PostQuitMessage(0);
									return 0;
					

				case WM_PAINT:		//	TODO
									ValidateRect(windowHandle, 0);
									return 0;
	}
	
	
	return DefWindowProc(windowHandle, message, wParam, lParam);
}