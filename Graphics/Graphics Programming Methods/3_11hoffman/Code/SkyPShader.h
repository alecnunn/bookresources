#ifndef _SKY_PSHADER_H
#define _SKY_PSHADER_H

#include "PixelShader.h"

class SkyPShader : public PixelShader
{

public:
	SkyPShader();
	~SkyPShader();

	virtual void Apply();

private:

	void SetupPixelShaderConstants();

};


#endif