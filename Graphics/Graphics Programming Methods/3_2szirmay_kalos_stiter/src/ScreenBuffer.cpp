#include <stdio.h>

#include "screenbuffer.hpp"


ScreenBuffer::ScreenBuffer()
{
	m_width		= 0;
	m_height	= 0;
	m_nPixel	= 0;
	m_radiance			= NULL;
	m_rgb				= NULL;
}

ScreenBuffer::~ScreenBuffer()
{
	if (m_radiance != NULL)
		delete[] m_radiance;
	if (m_rgb != NULL)
		delete[] m_rgb;
}

void ScreenBuffer::Initialize(Camera* pCamera)
{
	m_width		= pCamera->hres;
	m_height	= pCamera->vres;
	m_nPixel	= m_width * m_height;

	m_radiance	= new Color [m_nPixel];
	m_rgb		= new Color [m_nPixel];

	Clear ();
}

void ScreenBuffer::Syncronize()
{
	for(long i=0; i<m_nPixel; i++) {
		m_rgb[i] = m_radiance[i];
		m_rgb[i].ClipToUnitInterval();
	}
}

bool ScreenBuffer::saveImage(const char* fname)
{
	FILE* file = fopen(fname, "w+b");

	if (file) {
		fputc(0,file);
		fputc(0,file);
		fputc(2,file);
		for(int i=3;i<12;i++) 
			fputc(0,file);
		fputc(m_width&0xff,file);
		fputc(m_width/256,file);
		fputc(m_height&0xff,file);
		fputc(m_height/256,file);
		fputc(24,file);
		fputc(32,file);

		for(int y = 0; y < m_nPixel; y++ )
		{
			fputc((unsigned int)(m_rgb[y].b*255),file);
			fputc((unsigned int)(m_rgb[y].g*255),file);
			fputc((unsigned int)(m_rgb[y].r*255),file);
		}
		fclose(file);

		return true;
	} else {
		return false;
	}
}