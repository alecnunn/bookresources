#ifndef _PIXEL_SHADER_H
#define _PIXEL_SHADER_H


class PixelShader {
public:

	PixelShader() :m_PixelShader(0) {};
	virtual ~PixelShader() {};

	virtual void Apply() = 0;

protected:
	
	void CreatePixelShaderFromFile(char* fileName);
	void CreatePixelShaderFromBuffer(char *shaderStr);

	DWORD	m_PixelShader;


};
#endif
