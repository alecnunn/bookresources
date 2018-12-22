#ifndef _VERTEX_SHADER_H
#define _VERTEX_SHADER_H


class VertexShader {
public:
	VertexShader() :m_VertexShader(0) {};
	virtual ~VertexShader() {};

	virtual void Apply() = 0;
	virtual int GetVertexStride() const = 0;
	virtual DWORD GetVertexFormat() const = 0;

protected:
	
	void CreateVertexShader(char* fileName, DWORD* vertexShaderDeclaration);

	DWORD	m_VertexShader;


};
#endif




