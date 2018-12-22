#include "Scene.hpp"


#include <vector>


#include "D3DCheck.hpp"
#include "Display.hpp"
#include "Light.hpp"
#include "Mesh.hpp"


Scene::Scene(Display& display)

	:	display(display)
{
	display.device->AddRef();


	vertexShader[0] = display.getVertexShader("ShadowVolumeFrontCaps.vs");
	vertexShader[1] = display.getVertexShader("ShadowVolumeBackCaps.vs");
	vertexShader[2] = display.getVertexShader("ShadowVolumeSides.vs");
	vertexShader[3] = display.getVertexShader("Standard.vs");
}


Scene::~Scene()
{
	if(display.device)
	{
		if(vertexShader[0]) vertexShader[0]->Release();
		if(vertexShader[1]) vertexShader[1]->Release();
		if(vertexShader[2]) vertexShader[2]->Release();
		if(vertexShader[3]) vertexShader[3]->Release();


		display.device->Release();
	}
}


void Scene::drawMeshDiffuseAndSpecular(Mesh& mesh, Light& light)
{
	mesh.draw();
}


void Scene::drawDebugMeshShadowVolume(Mesh& mesh, Light& light)
{
	//	Draw front cap
	display.device->SetVertexShader(vertexShader[0]);
	mesh.draw();
	

	//	Draw back cap
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	display.device->SetVertexShader(vertexShader[1]);
	mesh.draw();
	
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	//	Draw sides
	display.device->SetVertexShader(vertexShader[2]);
	mesh.drawEdgeQuads();
}


void Scene::drawMeshShadowVolume(Mesh& mesh, Light& light)
{
	//	Draw back of shadow volume

	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	display.device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	
	//	Draw front cap
	display.device->SetVertexShader(vertexShader[0]);
	mesh.draw();
	
	//	Draw back cap
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	display.device->SetVertexShader(vertexShader[1]);
	mesh.draw();
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	//	Draw sides
	display.device->SetVertexShader(vertexShader[2]);
	mesh.drawEdgeQuads();

	
	//	Draw front of shadow volume

	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	display.device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
	
	//	Draw front cap
	display.device->SetVertexShader(vertexShader[0]);
	mesh.draw();
	
	//	Draw back cap
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	display.device->SetVertexShader(vertexShader[1]);
	mesh.draw();
	
	display.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	//	Draw sides
	display.device->SetVertexShader(vertexShader[2]);
	mesh.drawEdgeQuads();
}


void Scene::drawMeshAmbientAndEmissive(Mesh& mesh)
{
	mesh.draw();
}


void Scene::draw()
{
	D3DXMATRIX transposedWorldViewProjection;
	D3DXMATRIX transposedWorldView;
	
	
	//	We need these for the vertex shader

	D3DXMatrixTranspose(&transposedWorldViewProjection, &display.worldViewProjection);
	D3DXMatrixTranspose(&transposedWorldView, &display.worldView);
	

	//	Ambient and emissive pass

	display.device->SetVertexShaderConstantF(0, (FLOAT*)&D3DXVECTOR4(0.0, 0.5, 1.0, 2.0), 1);
	display.device->SetVertexShaderConstantF(1, (FLOAT*)&transposedWorldViewProjection, 4);
	display.device->SetVertexShaderConstantF(5, (FLOAT*)&D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	display.device->SetVertexShaderConstantF(6, (FLOAT*)&D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	display.device->SetVertexShaderConstantF(7, (FLOAT*)&ambient, 1);


	display.device->SetVertexShader(vertexShader[3]);
	
	for(std::list<Mesh*>::iterator mesh = meshes.begin(); mesh != meshes.end(); mesh++)
	{
		drawMeshAmbientAndEmissive(*(*mesh));
	}


	//	Diffuse and specular pass for each light
	
	for(std::list<Light*>::iterator light = lights.begin(); light != lights.end(); light++)
	{
		display.device->SetVertexShaderConstantF(5, (FLOAT*)&D3DXVECTOR4((*light)->position.x, (*light)->position.y, (*light)->position.z, (*light)->range), 1);
		display.device->SetVertexShaderConstantF(6, (FLOAT*)&(*light)->colour, 1);
		display.device->SetVertexShaderConstantF(7, (FLOAT*)&D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);


		//	Stencil must be cleared as each light has different shadow volumes
		display.device->Clear(0, 0, D3DCLEAR_STENCIL, 0xff000000, 1.0f, 127);
		

		//	Render state goodness

		display.device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		display.device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		display.device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		display.device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

		display.device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		display.device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		display.device->SetRenderState(D3DRS_STENCILREF, 0xffffffff);
		display.device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		display.device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		display.device->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);	
		

		//	Draw each meshes shadow volume with respect to the current light

		for(mesh = meshes.begin(); mesh != meshes.end(); mesh++)
		{
			drawMeshShadowVolume(*(*mesh), *(*light));
		}
		

		//	Set more render state goodness

		display.device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);

		display.device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		display.device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		display.device->SetRenderState(D3DRS_CULLMODE,  D3DCULL_CCW);
		display.device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		display.device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		display.device->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		display.device->SetRenderState(D3DRS_STENCILREF, 127);
		display.device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		display.device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		display.device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		display.device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		
		display.device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		display.device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		display.device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		display.device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		display.device->SetVertexShader(vertexShader[3]);


		//	Draw each mesh lit according to the current light

		for(mesh = meshes.begin(); mesh != meshes.end(); mesh++)
		{
			drawMeshDiffuseAndSpecular(*(*mesh), *(*light));
		}
		
		display.device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		display.device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		

		//	Optional debug visualisation pass

		for(mesh = meshes.begin(); mesh != meshes.end(); mesh++)
		{
			if((*mesh)->debug) drawDebugMeshShadowVolume(*(*mesh), *(*light));
		}
	}
}