#ifndef _DISPLAY_HPP
#define _DISPLAY_HPP


#include <assert.h>
#include <d3dx9.h>
#include <d3d9.h>


#include <string>
#include <list>


#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")


class Display
{
	public:

		D3DPRESENT_PARAMETERS presentParameters;
		LPDIRECT3DSURFACE9 depthStencil;
		LPDIRECT3DSURFACE9 surface;
		LPDIRECT3DDEVICE9 device;
		D3DCAPS9 capabilities;
		LPDIRECT3D9 direct3d;
		HWND windowHandle;
		DWORD height;
		DWORD width;


	public:

		D3DXMATRIX worldViewProjection;
		D3DXMATRIX worldView;

		LPD3DXMATRIXSTACK projection;
		LPD3DXMATRIXSTACK world;
		LPD3DXMATRIXSTACK view;


	public:

		Display(const char* const name, unsigned width, unsigned height);
		Display();
		virtual ~Display();


	public:

		void open(const char* const name, unsigned width, unsigned height);
		void close();


	public:

		LPDIRECT3DVERTEXSHADER9 getVertexShader(const char* const name);
		LPDIRECT3DPIXELSHADER9 getPixelShader(const char* const name);
		LPDIRECT3DTEXTURE9 getTexture(const char* const name);
		LPD3DXEFFECT getEffect(const char* const name);


	public:

		void flushVertexShaders();
		void flushPixelShaders();
		void flushTextures();
		void flushEffects();
		void flush();


	public:

		bool isValid();


	private:

		struct CachedVertexShader
		{
			LPDIRECT3DVERTEXSHADER9 vertexShader;
			std::string name;
		};


		struct CachedPixelShader
		{
			LPDIRECT3DPIXELSHADER9 pixelShader;
			std::string name;
		};


		struct CachedTexture
		{
			LPDIRECT3DTEXTURE9 texture;
			std::string name;
		};


		struct CachedEffect
		{
			LPD3DXEFFECT effect;
			std::string name;
		};


	private:

		std::list<CachedVertexShader> cachedVertexShaders;
		std::list<CachedPixelShader> cachedPixelShaders;
		std::list<CachedTexture> cachedTextures;
		std::list<CachedEffect> cachedEffects;


	private:

		static LRESULT WINAPI windowHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
};


#endif