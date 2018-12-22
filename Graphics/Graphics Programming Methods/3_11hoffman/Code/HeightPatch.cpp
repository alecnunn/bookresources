// height patch
// (c) Kenny Mitchell - Westwood Studios EA 2002

#include "stdafx.h"

#include "HeightPatch.h"
#include "TerrainShader.h"
#include "MyDebug.h"
#include "Defines.h"

extern LPDIRECT3DDEVICE8 g_Device;
extern int g_ScatterVS;
HeightPatch::HeightPatch()
{
	// create shader
	m_pShader=new TerrainShader();

	CreateVertexBuffer();
	CreateIndexBuffer();
}

HeightPatch::~HeightPatch()
{
	SAFE_RELEASE(m_IndexBuffer);
	SAFE_RELEASE(m_VertexBuffer);

	SAFE_DELETE(m_pShader);
}

float HeightPatch::GetSize()
{
	return PATCH_SIZE * PATCH_SCALE;
}



void HeightPatch::Render()
{
	g_ScatterVS = 1;


	// Choose shader and set up constants.
	m_pShader->Apply();

	// set stream source
	g_Device->SetStreamSource(0, m_VertexBuffer, sizeof(HeightPatchVertex));

	// set indices
	g_Device->SetIndices(m_IndexBuffer,0);

	// draw patch
	g_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, m_NumVertices, 0, m_NumIndices);

}

void HeightPatch::CreateVertexBuffer()
{
	m_NumVertices=NUM_PATCH_VERTICES;
	HRESULT result;

	result=g_Device->CreateVertexBuffer
	(
		m_NumVertices*sizeof(HeightPatchVertex),
		D3DUSAGE_WRITEONLY, 
		D3DFVF_XYZ,
		D3DPOOL_MANAGED, 
		&m_VertexBuffer
	);

	FAILMSG("Failed to create vertex buffer");

	// create vertices
	HeightPatchVertex* vertices;

	// load height field into buffer
	result=m_VertexBuffer->Lock
	(
		0,
		m_NumVertices*sizeof(HeightPatchVertex), 
		(BYTE**)&vertices,
		0
	);
	FAILMSG("Failed to lock vertex buffer");

	// read height field
	unsigned char map[NUM_PATCH_VERTICES];
	FILE* map_file=fopen("hf129_3.raw","rb");
	if (!map_file)
	{
		Fail_Message("Failed to open height field",__FILE__,__LINE__);
	}
	fread(map,NUM_PATCH_VERTICES,sizeof(unsigned char),map_file);
	fclose(map_file);

	int x,z;
	int size=PATCH_SIZE;
	float fx, fz;
	float scale=PATCH_SCALE;
	unsigned char* map_ptr=map;

	for (z=0; z<size; z++)
	{
		for (x=0; x<size; x++)
		{
			float fy;

			fx=((float)x)*scale;
			fz=((float)z)*scale;
			fy=((float)(*map_ptr))*HEIGHT_SCALE;

			vertices->x=fx;
			vertices->y=fy;
			vertices->z=fz;

			vertices++;
			map_ptr++;


		}
	}

	m_VertexBuffer->Unlock();
}


void HeightPatch::CreateIndexBuffer()
{
	// create index buffer
	int size=PATCH_SIZE;
	int step=1;
	int psize=(size-1)/step;

	// calculate number of indices
	m_NumIndices=((psize))*(((psize+1))*2)+(psize*2)-2;

	// Create index buffer
	HRESULT result=g_Device->CreateIndexBuffer
	(
		(m_NumIndices+2)*sizeof(WORD),
		D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, 
		D3DPOOL_MANAGED,
		&m_IndexBuffer
	);
	FAILMSG("Failed to create index buffer");

	// create indices
	WORD* indices;

	result=m_IndexBuffer->Lock(0, 0/*m_NumIndices*sizeof(WORD) */, (BYTE**)&indices, 0);
	FAILMSG("Failed to lock index buffer");

	// generates indices to process patch in resonably cache friendly 'S' format
	int x,z,s_toggle=1;
	for (z=step; z<size; z+=step)
	{
		if (s_toggle)
		{
			for (x=0; x<(size-step); x+=step)
			{
				*indices++=(WORD)((z-0)*size+(x));
				*indices++=(WORD)((z-step)*size+(x));
			}

			*indices++=(WORD)((z-0)*size+(size-1));
			*indices++=(WORD)((z-step)*size+(size-1));

			// insert additional degenerates to start next row
			*indices++=indices[-2];
			*indices++=indices[-1];
		}
		else
		{
			*indices++=(WORD)((z-step)*size+(size-1));
			*indices++=(WORD)((z-0)*size+(size-1));

			for (x=size-1; x>0; x-=step)
			{
				*indices++=(WORD)((z-step)*size+(x-step));
				*indices++=(WORD)((z-0)*size+(x-step));
			}

			// insert additional degenerates to start next row
			*indices++=indices[-1];
			*indices++=indices[-1];
		}

		s_toggle=!s_toggle;
	}

	m_IndexBuffer->Unlock();
}
