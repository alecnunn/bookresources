#ifndef _MESH_HPP
#define _MESH_HPP


#include <d3dx9.h>


#include <vector>


class Mesh
{
	public:
		
		typedef unsigned short Index;


		struct Vertex
		{
			D3DXVECTOR3 position;
			D3DXVECTOR3 normal;
		};


		struct Edge
		{
			Vertex vertex[2];
			Index index[2];


			Edge(const Vertex& vertex0, const Vertex& vertex1, const Index& index0, const Index& index1);
			bool operator==(const Edge& edge) const;
		};


	public:

		LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
		LPDIRECT3DINDEXBUFFER9 indexBuffer;
		LPDIRECT3DDEVICE9 device;

		std::vector<Vertex> vertices;
		std::vector<Index> indices;

		bool closed2ManifoldMesh;
		DWORD triangles;
		bool debug;


	public:

		Mesh(LPDIRECT3DDEVICE9 device, const char* const fileName, float scale = 1.0f);
		virtual ~Mesh();


	public:

		void drawEdgeQuads();
		void draw();


	private:

		void loadFromX(const char* const fileName, float scale = 1.0f);
		void processForShadowVolumes();
		void calculateFaceNormals();
		void deallocate();
		void allocate();
		void expand();
};


inline Mesh::Edge::Edge(const Vertex& vertex0, const Vertex& vertex1, const Index& index0, const Index& index1)
{
		vertex[0] = vertex0;
		vertex[1] = vertex1;


		index[0] = index0;
		index[1] = index1;
}


inline bool Mesh::Edge::operator==(const Edge& edge) const
{
	return edge.vertex[1].position == vertex[0].position && edge.vertex[0].position == vertex[1].position;
}


#endif