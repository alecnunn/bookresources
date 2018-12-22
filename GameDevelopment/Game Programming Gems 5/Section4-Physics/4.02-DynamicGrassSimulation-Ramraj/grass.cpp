//----------------------------------------------------------------------------
//
//grass.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include <olectl.h>
#include <math.h>
#include "grass.h"

#pragma comment( lib, "opengl32.lib" )

//··········································································//
//    Source :: Definitions
//··········································································//

#define TWIST_SCALE 56

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

grass::grass()
{
	m_grid = NULL;
	m_hmap = NULL;
	m_smap = NULL;

	m_active = false;
	m_b_height = false;
	m_b_scale = false;		
}

grass::~grass()
{
	delete[] m_grid;
	delete[] m_hmap;
	delete[] m_smap;
}

bool grass::init(const int width ,const int height, 
		const float damp_factor, const int spread_factor, 
		const float * windvector, float density)

{
	if(!windvector)
		return false;

	delete[] m_grid;
	m_active = false;
	
	//Set up the variables
	if((width <= 0) || (height <= 0) || (damp_factor == 0))
		return false;	
	
	m_spread_fac = spread_factor;
	m_damp_fac = damp_factor;
	m_width = width;
	m_height = height;
	m_density = density;
	m_grid = new float [height * width];
	if(windvector)
	{
		m_wind[0] = windvector[0];
		m_wind[1] = windvector[1];
		m_wind[2] = windvector[2];
	}
	else
	{
		m_wind[0] = windvector[0];
		m_wind[1] = windvector[1];
		m_wind[2] = windvector[2];
	}
	
	if(m_grid == NULL)
		return false;	
	
	//Sets the water heights to zero
	ZeroMemory(m_grid, sizeof(float) * width * height);
	m_active = true;
	m_srand = rand();

	return true;
}

void grass::add_ripple(int x,int y,float height)
{
	if ( m_active && (x >= 0) && (y >= 0) && (x < m_width) && (y < m_height) )
		m_grid[gen_index(x,y)] = height;	
}

void grass::update(int milliseconds)
{
	if(!m_active)
		return;	
	
	float y;
	int x,z,i;
	for(z=1; z<(m_height-1); z++)
	{
		for(x=1; x<(m_width-1); x++)
		{
			i = gen_index(x,z);
			y = ( (m_grid[i + 1] +
				   m_grid[i - 1] +
				   m_grid[i + m_width] +
				   m_grid[i - m_width] ) / 2) -
				  (m_grid[i] );			
			m_grid[i]= y - y*m_damp_fac*(1000.0f - (float)milliseconds)/1000.0f;
		}
	}
}

void grass::draw(void (*draw_stalk)(void))
{
	if(!m_active || !draw_stalk)
		return;
	
	int x, z, i, ran;
	float ran1, ran2;

	//used as a seed for rand after this function
	//otherwise successive calls to rand will have
	//a pattern
	ran = rand();
	//Insures reproducable random numbers
	srand(m_srand);
	
	for(z=0; z<(m_height); z++)
	{
		for(x = 0; x<(m_width); x++)
		{
			i = gen_index(x,z);
			ran1 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;
			ran2 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;

			glPushMatrix();

				glTranslatef(m_density*((float)x + ran1),0.0f,m_density*((float)z + ran2));				
				//apply wind transformations
				glRotatef(m_grid[i] * m_wind[0], 1.0f, 0.0f, 0.0f);
				glRotatef(m_grid[i] * m_wind[2], 0.0f, 0.0f, 1.0f);
				glRotatef(ran2*TWIST_SCALE, 0.0f, 1.0f, 0.0f);
				glScalef(1.0f, 1 + m_grid[i] * m_wind[1], 1.0f);

				draw_stalk();
	
			glPopMatrix();
		}			
	}

	srand(ran);
}

void grass::draw_scaled(void (*draw_stalk)(void))
{
	if(!m_active || !draw_stalk || !m_b_scale)
		return;
	
	int x, z, i, ran;
	float ran1, ran2;

	ran = rand();
	srand(m_srand);
	
	for(z=0; z<(m_height); z++)
	{
		for(x = 0; x<(m_width); x++)
		{
			i = gen_index(x,z);
			ran1 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;
			ran2 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;

			glPushMatrix();

				glTranslatef(m_density*((float)x + ran1),0.0f,m_density*((float)z + ran2));
				glScalef(m_smap[i],m_smap[i],m_smap[i]);
				//apply wind transformations
				glRotatef(m_grid[i] * m_wind[0], 1.0f, 0.0f, 0.0f);
				glRotatef(m_grid[i] * m_wind[2], 0.0f, 0.0f, 1.0f);
				glRotatef(ran2*TWIST_SCALE, 0.0f, 1.0f, 0.0f);
				glScalef(1.0f, 1 + m_grid[i] * m_wind[1], 1.0f);

				draw_stalk();
	
			glPopMatrix();
		}			
	}

	srand(ran);
}
void grass::draw_height(void (*draw_stalk)(void))
{
	if(!m_active || !draw_stalk || !m_b_height)
		return;
	
	int x, z, i, ran;
	float ran1, ran2;

	ran = rand();
	srand(m_srand);
	
	for(z=0; z<(m_height); z++)
	{
		for(x = 0; x<(m_width); x++)
		{
			i = gen_index(x,z);
			ran1 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;
			ran2 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;

			glPushMatrix();

				glTranslatef(m_density*((float)x + ran1),m_hmap[i],m_density*((float)z + ran2));
				//apply wind transformations
				glRotatef(m_grid[i] * m_wind[0], 1.0f, 0.0f, 0.0f);
				glRotatef(m_grid[i] * m_wind[2], 0.0f, 0.0f, 1.0f);
				glRotatef(ran2*TWIST_SCALE, 0.0f, 1.0f, 0.0f);
				glScalef(1.0f, 1 + m_grid[i] * m_wind[1], 1.0f);

				draw_stalk();
	
			glPopMatrix();
		}			
	}

	srand(ran);
}
void grass::draw_scaled_height(void (*draw_stalk)(void))
{
	if(!m_active || !draw_stalk || !m_b_scale || !m_b_height)
		return;
	
	int x, z, i, ran;
	float ran1, ran2;

	ran = rand();
	srand(m_srand);
	
	for(z=0; z<(m_height); z++)
	{
		for(x = 0; x<(m_width); x++)
		{
			i = gen_index(x,z);
			ran1 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;
			ran2 = (((float)rand()/(float)RAND_MAX) * 2 - 1) * m_spread_fac;

			glPushMatrix();

				glTranslatef(m_density*((float)x + ran1),m_hmap[i],m_density*((float)z + ran2));
				glScalef(m_smap[i],m_smap[i],m_smap[i]);
				//apply wind transformations
				glRotatef(m_grid[i] * m_wind[0], 1.0f, 0.0f, 0.0f);
				glRotatef(m_grid[i] * m_wind[2], 0.0f, 0.0f, 1.0f);
				glRotatef(ran2*TWIST_SCALE, 0.0f, 1.0f, 0.0f);
				glScalef(1.0f, 1 + m_grid[i] * m_wind[1], 1.0f);

				draw_stalk();
	
			glPopMatrix();
		}			
	}

	srand(ran);
}

bool grass::load_height_map( const char * path, float height_scale)
{
	if(!m_active)
		return false;

	delete[] m_hmap;
	m_b_height = false;

	//load map
	if(!load_map(path,&m_hmap))
		return false;

	//scale map
	int x,z;
	for(z=0; z < m_height ; z++)
	{
		for(x=0; x < m_width ; x++)
		{
			m_hmap[gen_index(x,z)] *= height_scale;
		}
	}
	m_b_height = true;

	return true;
}

bool grass::load_scale_map( const char * path, float mul_fac, float add_fac)
{
	if(!m_active)
		return false;

	delete[] m_smap;
	m_b_scale = false;

	//load map
	if(!load_map(path,&m_smap))
		return false;

	//scale/increment map
	int x,z,i;
	for(z=0; z < m_height ; z++)
	{
		for(x=0; x < m_width ; x++)
		{
			i = gen_index(x,z);
			m_smap[i] *= mul_fac;
			m_smap[i] += add_fac;

			if(m_smap[i] < 0.0f)
				m_smap[i] = 0.0f;
		}
	}
	m_b_scale = true;

	return true;	
}

bool grass::load_map( const char * path, float **buf)
{
	if(!m_active)
		return false;
	
	//generate path
	char abs_path[MAX_PATH+1];

	if (strstr(path, "http://"))
		strcpy(abs_path, path);	
	else
	{
		GetCurrentDirectory(MAX_PATH, abs_path);
		char * end_char = abs_path + strlen(abs_path) - 1;
		if(*end_char != '\\')
			strcat(abs_path, "\\");
		strcat(abs_path, path);
	}
	OLECHAR ole_path[MAX_PATH+1];
	MultiByteToWideChar(CP_ACP, 0, abs_path, -1, ole_path, MAX_PATH);

	//Load ipicture

	IPicture * ipic;
	HRESULT hr = OleLoadPicturePath(ole_path, 0, 0, 0, IID_IPicture, (void**)&ipic);

	if(FAILED(hr))
		return false;

	//calculate picture size
	HDC	hDC = CreateCompatibleDC(GetDC(0));
	if(!hDC)
	{
		ipic->Release();
		return false;
	}
	

	long w_ipic;
	long h_ipic;
	long w_pixs;
	long h_pixs;
	
	ipic->get_Width(&w_ipic);
	w_pixs = MulDiv(w_ipic, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
	ipic->get_Height(&h_ipic);
	h_pixs = MulDiv(h_ipic, GetDeviceCaps(hDC, LOGPIXELSY), 2540);	

	{
		GLint max_dim;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_dim);	

		if (w_pixs <= max_dim)
			w_pixs = 1 << (int)floor((log((double)w_pixs)/log(2.0f)) + 0.5f); 
		else
			w_pixs = max_dim;
 
		if (h_pixs <= max_dim)
			h_pixs = 1 << (int)floor((log((double)h_pixs)/log(2.0f)) + 0.5f);
		else 
			h_pixs = max_dim;
	}

	HBITMAP		hBMP;	
	BITMAPINFO	bi = {0};
	DWORD		*pBits = 0;

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount		= 32;
	bi.bmiHeader.biWidth		= w_pixs;
	bi.bmiHeader.biHeight		= h_pixs;
	bi.bmiHeader.biCompression	= BI_RGB;
	bi.bmiHeader.biPlanes		= 1;

	hBMP = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hBMP)
	{
		DeleteDC(hDC);
		ipic->Release();
		return false;
	}

	SelectObject(hDC, hBMP);
	ipic->Render(hDC, 0, 0, w_pixs, h_pixs, 0, h_ipic, w_ipic, -h_ipic, 0);	

	// average byte values
	BYTE* pPixel;
	int i;
	for(i = 0; i < w_pixs * h_pixs; i++)
	{
		pPixel = (BYTE*)(&pBits[i]);
		pPixel[0] += pPixel[1];
		pPixel[0] += pPixel[2];
		pPixel[0] /= 3;
	}

	//map to buf values
	(*buf) = new float [m_width * m_height];	
	if(!(*buf))
		return false;

	int index_x, index_z;
	int x,z;
	for(z=0; z < m_height ; z++)
	{
		index_z = (int)( ((float)z / (float)m_height) * (float)(h_pixs) );
		for(x=0; x < m_width ; x++)
		{
			index_x = (int)( ((float)x / (float)m_width) * (float)(w_pixs) );
			pPixel	= (BYTE*)(&pBits[(index_z) * w_pixs + index_x]);
			i = gen_index(x,z);
			(*buf)[i] = ((float)pPixel[0]) / (255.0f);			
		}
	}
	
	DeleteObject(hBMP);
	DeleteDC(hDC);
	ipic->Release();	

	return true;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//grass.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
