//=============================================================================
// d3dUtil.h by Frank Luna (C) 2005 All Rights Reserved.
//
// Contains various utility code for DirectX applications, such as, clean up
// and debugging code.
//=============================================================================

#ifndef D3DUTIL_H
#define D3DUTIL_H

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <string>
#include <sstream>
#include <vector>

//===============================================================
// Globals for convenient access.
class D3DApp;
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;

//===============================================================
// Clean up

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

//===============================================================
// Geometry generation.

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices);

//===============================================================
// Colors and Materials

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);

struct Mtrl
{
	Mtrl()
		:ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(8.0f){}
	Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, 
		 const D3DXCOLOR& s, float power)
		:ambient(a), diffuse(d), spec(s), specPower(power){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
};

struct DirLight
{
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXVECTOR3 dirW;
};

//===============================================================
// .X Files

void LoadXFile(
	const std::string& filename, 
	ID3DXMesh** meshOut, 
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs);

//===============================================================
// Math Constants

const float INFINITY = FLT_MAX;
const float EPSILON  = 0.001f;

//===============================================================
// Debug

inline void HandleHR(HRESULT hr, const std::string file, int lineNum)
{                                                           
	if(FAILED(hr))                                                 
	{        
		std::stringstream ss;
		std::string strLineNum;
		ss << lineNum;
		ss >> strLineNum;

		std::string s = std::string("File = ") + file;
		s += std::string("\nLine = ") + strLineNum;
		s += std::string("\nDesc = ") + DXGetErrorDescription9(hr);
		s += std::string("\nCode = ") + DXGetErrorString9(hr);
		s += std::string("\n\nDo you want to debug?");
		
		if( MessageBox(0, s.c_str(), 0, MB_YESNO) == IDYES )
			DebugBreak();
	}   
}

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(hr) {HandleHR(hr, __FILE__, __LINE__);}
	#endif
#else
	#ifndef HR
	#define HR(hr) {hr;}
	#endif
#endif 

#endif // D3DUTIL_H