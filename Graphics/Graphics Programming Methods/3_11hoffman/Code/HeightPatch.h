// height patch
// (c) Kenny Mitchell - Westwood Studios EA 2002

#ifndef _HEIGHT_PATCH_H
#define _HEIGHT_PATCH_H

#include <d3d8.h>

class Shader;


class HeightPatchVertex
{
public:
	float x,y,z;
}; 

class HeightPatch
{
public:
	HeightPatch();
	~HeightPatch();
	float GetSize();

	void Render();

private:

	void CreateVertexBuffer();
	void CreateIndexBuffer();

	LPDIRECT3DVERTEXBUFFER8 m_VertexBuffer;
	DWORD					m_NumVertices;

	LPDIRECT3DINDEXBUFFER8	m_IndexBuffer;
	int						m_NumIndices;

	Shader*			m_pShader;
};

#endif
