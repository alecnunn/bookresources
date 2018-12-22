#ifndef _SCENE_HPP
#define _SCENE_HPP


#include <windows.h>
#include <d3dx9.h>

#include <list>


class Display;
class Light;
class Mesh;


class Scene
{
	public:
		
		std::list<Mesh*> meshes;
		std::list<Light*> lights;
		D3DXVECTOR4 ambient;


	public:

		Scene(Display& display);
		virtual ~Scene();


	public:

		void drawMeshDiffuseAndSpecular(Mesh& mesh, Light& light);
		void drawDebugMeshShadowVolume(Mesh& mesh, Light& light);
		void drawMeshShadowVolume(Mesh& mesh, Light& light);
		void drawMeshAmbientAndEmissive(Mesh& mesh);
		void draw();


	private:

		LPDIRECT3DVERTEXSHADER9 vertexShader[4];
		Display& display;
};


#endif