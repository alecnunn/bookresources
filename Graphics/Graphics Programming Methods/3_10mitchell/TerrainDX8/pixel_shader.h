// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef PIXEL_SHADER_H
#define PIXEL_SHADER_H

#ifndef SHADER_H
#include "shader.h"
#endif

class Pixel_Shader_Class : public Shader_Class
{
public:
	Pixel_Shader_Class();
	virtual ~Pixel_Shader_Class();

	virtual void Apply()=0;

protected:
	
	void Create_Pixel_Shader(char* file_name);

	DWORD							Pixel_Shader;
};

#endif