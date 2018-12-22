// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "displacement_patch.h"
#include "terrain.h"
#include "analytic_shader.h"

Displacement_Patch::Displacement_Patch()
: Vertex_Buffer(NULL),
  Index_Buffer(NULL),
  Shader(NULL)
{
	Create_Displacement_Map();

	Create_Shader();

	Create_Vertex_Buffer();
	Create_Index_Buffer();
}

Displacement_Patch::~Displacement_Patch()
{
	// ensure null state before clean up
	DEV->SetPixelShader(NULL);
	DEV->SetIndices(0,NULL);
	DEV->SetStreamSource(0,NULL,0);

	Safe_Release(Index_Buffer);
	Safe_Release(Vertex_Buffer);

	Safe_Delete(Shader);

	Safe_Delete(Displacement_Map);
}

void Displacement_Patch::Render()
{
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	DEV->SetTransform(D3DTS_WORLD, &world);

	Shader->Apply();
	Shader->Update_Instance();

    // Displacement map control keys for realtime interactive manipulation of the different parameters
	Input_Class* input=Application::Get_Input();

	if (input->Get_Key('I'))	Displacement_Scale  +=1.0f; // Increase Scale
	if (input->Get_Key('K'))	Displacement_Scale  -=1.0f; // Decrease Scale

	// set stream source
	DEV->SetStreamSource
	(
		0, 
		Vertex_Buffer, 
		sizeof(DISPLACEMENT_PATCH_VERTEX)
	);

	// set indices
	DEV->SetIndices(Index_Buffer,0);

	// draw patch
	DEV->DrawIndexedPrimitive
	(
		D3DPT_TRIANGLESTRIP, 
		0, 
		Num_Vertices,
		0, 
		Num_Indices/3
	);

	// show displacement patch stats
	char string[255];
	CD3DFont* font=Application::Get_Font();

	sprintf(string,"I/K - Displacement Scale %4.3f", Displacement_Scale/256.0f);
	font->DrawText(10,60,string);
}

void Displacement_Patch::Render_Tiled()
{
	char string[255];
	CD3DFont* font=Application::Get_Font();

	Shader->Apply();

    // Displacement map control keys for realtime interactive manipulation of the different parameters
	Input_Class* input=Application::Get_Input();

	if (input->Get_Key('I'))	Displacement_Scale  +=1.0f; // Increase Scale
	if (input->Get_Key('K'))	Displacement_Scale  -=1.0f; // Decrease Scale

	// set stream source
	DEV->SetStreamSource
	(
		0, 
		Vertex_Buffer, 
		sizeof(DISPLACEMENT_PATCH_VERTEX)
	);

	// set indices
	DEV->SetIndices(Index_Buffer,0);

	D3DXVECTOR4 corner(0,0,0,1);
	D3DXVECTOR3 delta(PATCH_WORLD_WIDTH,Displacement_Scale,PATCH_WORLD_WIDTH);

	D3DXVECTOR4 pcorner(0,0,0,1);
	D3DXVECTOR3 pdelta(PATCH_SCALE,Displacement_Scale,PATCH_SCALE);

	// draw tile grid
	for (int tx=0; tx<TILE_GRID_WIDTH; tx++)
	{
		for (int tz=0; tz<TILE_GRID_WIDTH; tz++)
		{
			D3DXMATRIX world;
			D3DXMatrixIdentity(&world);
			world._41=tx*PATCH_WORLD_WIDTH;
			world._43=tz*PATCH_WORLD_WIDTH;
			world._22=Displacement_Scale*HEIGHT_SCALE;

			// frustum cull tile
			corner.x=world._41;
			corner.z=world._43;

			if (!Application::Get_Camera()->Test_AAPatch_Against_Frustum(corner,delta))
			{
				DEV->SetTransform(D3DTS_WORLD, &world);

				Shader->Update_Instance();

				// draw patch
				DEV->DrawIndexedPrimitive
				(
					D3DPT_TRIANGLESTRIP, 
					0, 
					Num_Vertices,
					0, 
					Num_Indices
				);
			}
		}
	}

	sprintf(string,"I/K - Displacement Scale %4.3f", Displacement_Scale/256.0f);
	font->DrawText(10,60,string);
}

void Displacement_Patch::Create_Vertex_Buffer()
{
	// Create vertex buffer
	Num_Vertices=NUM_PATCH_VERTICES;
	
	HRESULT result=DEV->CreateVertexBuffer
	(
		Num_Vertices*sizeof(DISPLACEMENT_PATCH_VERTEX),
		D3DUSAGE_WRITEONLY, 
		0,
		D3DPOOL_DEFAULT, 
		&Vertex_Buffer
	);
	FAILMSG("Failed to create vertex buffer");

	// create vertices
	DISPLACEMENT_PATCH_VERTEX* vertices;

	// load height field into buffer
	result=Vertex_Buffer->Lock
	(
		0,
		Num_Vertices*sizeof(DISPLACEMENT_PATCH_VERTEX), 
		(BYTE**)&vertices,
		0
	);
	FAILMSG("Failed to lock vertex buffer");

	int x,z;
	int size=PATCH_SIZE;
	float fx, fz;
	float scale=PATCH_SCALE;
	float fsize=((float)(size-1));
	float offset=256.0f/1024.0f;
	float border=512.0f/1024.0f;

	for (z=0; z<size; z++)
	{
		unsigned char* map=&Displacement_Map[((z%PATCH_WIDTH)<<MAP_VERTEX_STRIDE_POWER_OF_2)<<MAP_WIDTH_POWER_OF_2];

		for (x=0; x<size; x++)
		{
			fx=((float)x)*scale;
			fz=((float)z)*scale;

			vertices->x=fx;
			vertices->y=map[(x%PATCH_WIDTH)<<MAP_VERTEX_STRIDE_POWER_OF_2];
			vertices->z=fz;

			vertices++;
		}
	}

	Vertex_Buffer->Unlock();
}

void Displacement_Patch::Create_Index_Buffer()
{
	// create index buffer
	int size=PATCH_SIZE;
	int step=1;
	int psize=(size-1)/step;

	// calculate number of indices
	Num_Indices=((psize))*(((psize+1))*2)+(psize*2)-2;

	// Create index buffer
	HRESULT result=DEV->CreateIndexBuffer
	(
		(Num_Indices+2)*sizeof(WORD),
		D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, 
		D3DPOOL_MANAGED,
		&Index_Buffer
	);
	FAILMSG("Failed to create index buffer");

	// create indices
	WORD* indices;

	result=Index_Buffer->Lock(0, 0, (BYTE**)&indices, 0);
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

	Index_Buffer->Unlock();
}

void Displacement_Patch::Create_Displacement_Map()
{
	Displacement_Scale=256.0f;

	// read height field
	Displacement_Map=new unsigned char[MAP_SIZE];
	FILE* map_file=fopen("hf512.raw","rb");
	if (!map_file)
	{
		Fail_Message("Failed to open height field",__FILE__,__LINE__);
	}
	fread(Displacement_Map,MAP_SIZE,sizeof(unsigned char),map_file);
	fclose(map_file);
}

void Displacement_Patch::Create_Shader()
{
	// create shader
	Analytic_Shader* shader=new Analytic_Shader();

	Shader=shader;
}