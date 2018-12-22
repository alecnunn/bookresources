#include "Mesh.hpp"


#include <assert.h>
#include <list>


#include "D3DCheck.hpp"


Mesh::Mesh(LPDIRECT3DDEVICE9 device, const char* const fileName, float scale)

	:	closed2ManifoldMesh(false),
		vertexBuffer(0),
		indexBuffer(0),
		device(device),
		triangles(0),
		debug(false)
{
	device->AddRef();

	loadFromX(fileName, scale);
}


Mesh::~Mesh()
{
	deallocate();


	if(device)
	{
		device->Release();
		device = 0;
	}
}


void Mesh::drawEdgeQuads()
{
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	device->SetIndices(indexBuffer);
	device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
	device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (UINT)vertices.size(), 0, (UINT)indices.size()/3);
}


void Mesh::draw()
{
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, triangles);
}


void Mesh::loadFromX(const char* const fileName, float scale)
{
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	BYTE* vertexData;
	BYTE* indexData;
	LPD3DXMESH mesh;


	D3DCheck() = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM, device, 0, 0, 0, 0, &mesh);
	D3DCheck() = mesh->GetVertexBuffer(&vertexBuffer);
	D3DCheck() = mesh->GetIndexBuffer(&indexBuffer);


	vertices.resize(mesh->GetNumVertices());
	indices.resize(mesh->GetNumFaces()*3);


	vertexBuffer->Lock(0, mesh->GetNumVertices()*D3DXGetFVFVertexSize(mesh->GetFVF()), (void**)&vertexData, D3DLOCK_READONLY);
	indexBuffer->Lock(0, mesh->GetNumFaces()*2, (void**)&indexData, D3DLOCK_READONLY);


	memcpy(&indices[0], indexData, sizeof(Index)*indices.size());


	for(unsigned vertex = 0; vertex < mesh->GetNumVertices(); vertex++)
	{
		vertices[vertex].position = *((D3DXVECTOR3*)(vertexData + D3DXGetFVFVertexSize(mesh->GetFVF())*vertex));
		vertices[vertex].position *= scale;
	}


	D3DCheck() = indexBuffer->Unlock();
	D3DCheck() = vertexBuffer->Unlock();


	indexBuffer->Release();
	vertexBuffer->Release();


	mesh->Release();


	triangles = (DWORD)indices.size()/3;


	expand();
	calculateFaceNormals();
	processForShadowVolumes();
	allocate();
}


void Mesh::processForShadowVolumes()
{
	std::list<Edge> edges;


	//	Be optimistic to start with
	closed2ManifoldMesh = true;


	//	From now on the indices will only be used for the degenerate edge quads

	indices.clear();
	indices.reserve(vertices.size()*4);


	//	Make a list of all the edges in the mesh

	for(Index triangle = 0; triangle < vertices.size()/3; triangle++)
	{
		edges.push_back(Edge(vertices[triangle*3 + 0], vertices[triangle*3 + 1], triangle*3 + 0, triangle*3 + 1));
		edges.push_back(Edge(vertices[triangle*3 + 1], vertices[triangle*3 + 2], triangle*3 + 1, triangle*3 + 2));
		edges.push_back(Edge(vertices[triangle*3 + 2], vertices[triangle*3 + 0], triangle*3 + 2, triangle*3 + 0));
	}


	//	Try and pair up each edge

	while(edges.size())
	{
		//	Get the first edge in the list and remove it from the list

		Edge edgeToMatch = edges.front();
		bool matched = false;

		edges.pop_front();


		//	See if any of the edges in the list match this one

		std::list<Edge>::iterator edge = edges.begin();


		while(edge != edges.end())
		{
			if(*edge == edgeToMatch)
			{
				matched = true;
				break;
			}


			edge++;
		}


		if(matched)
		{
			//	We ignore edges between faces with identical normals as they can't cast shadows

			if(edge->vertex[0].normal != edgeToMatch.vertex[0].normal)
			{
				//	Add degenerate edge quad
				indices.push_back(edgeToMatch.index[0]);
				indices.push_back(edge->index[1]);
				indices.push_back(edgeToMatch.index[1]);

				indices.push_back(edge->index[0]);
				indices.push_back(edgeToMatch.index[1]);
				indices.push_back(edge->index[1]);
			}

			//	Remove the matched edge
			edges.erase(edge);
		}
		else
		{
			/*
				No matching edge was found so either its an open edge or a matching
				edge was already previously matched up with another matching edge!
				In either case we just treat it as if it was an open edge and add two
				new vertices for the degenerate quad which are flagged with a non-unit
				length normal so we can detect it as an 'open edge' quad in the vertex
				shader.
			*/

			vertices.push_back(edgeToMatch.vertex[0]);
			vertices.back().normal = -vertices.back().normal*(float)sqrt(2.2f/D3DXVec3Length(&vertices.back().normal));

			vertices.push_back(edgeToMatch.vertex[1]);
			vertices.back().normal = -vertices.back().normal*(float)sqrt(2.2f/D3DXVec3Length(&vertices.back().normal));

			indices.push_back(edgeToMatch.index[0]);
			indices.push_back((Mesh::Index)vertices.size() - 2);
			indices.push_back(edgeToMatch.index[1]);

			indices.push_back((Mesh::Index)vertices.size() - 2);
			indices.push_back((Mesh::Index)vertices.size() - 1);
			indices.push_back(edgeToMatch.index[1]);


			//	It's not a 'nice' mesh :)
			closed2ManifoldMesh = false;
		}
	}
}


void Mesh::calculateFaceNormals()
{
	for(Index triangle = 0; triangle < triangles; triangle++)
	{
		D3DXVECTOR3 edge0 = vertices[triangle*3 + 1].position - vertices[triangle*3 + 0].position;
		D3DXVECTOR3 edge1 = vertices[triangle*3 + 2].position - vertices[triangle*3 + 0].position;
		D3DXVECTOR3 normal;


		D3DXVec3Cross(&normal, &edge0, &edge1);
		D3DXVec3Normalize(&normal, &normal);


		vertices[triangle*3 + 0].normal = normal;
		vertices[triangle*3 + 1].normal = normal;
		vertices[triangle*3 + 2].normal = normal;
	}
}


void Mesh::deallocate()
{
	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}


	if(indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}
}


void Mesh::allocate()
{
	BYTE* vertexData;
	BYTE* indexData;


	D3DCheck() = device->CreateIndexBuffer(	(UINT)indices.size()*sizeof(Index),
											D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16,
											D3DPOOL_MANAGED,
											&indexBuffer,
											0);

	D3DCheck() = device->CreateVertexBuffer((UINT)vertices.size()*sizeof(Vertex),
											D3DUSAGE_WRITEONLY,
											0,
											D3DPOOL_MANAGED,
											&vertexBuffer,
											0);


	D3DCheck() = indexBuffer->Lock(0, (UINT)indices.size()*sizeof(Index), (void**)&indexData, 0);
	D3DCheck() = vertexBuffer->Lock(0, (UINT)vertices.size()*sizeof(Vertex), (void**)&vertexData, 0);


	memcpy(indexData, &indices[0], indices.size()*sizeof(Index));
	memcpy(vertexData, &vertices[0], vertices.size()*sizeof(Vertex));


	D3DCheck() = vertexBuffer->Unlock();
	D3DCheck() = indexBuffer->Unlock();
}


void Mesh::expand()
{
	std::vector<Vertex> vertices;


	vertices.resize(indices.size());


	//	Un-index the mesh

	for(Index index = 0; index < indices.size(); index++)
	{
		vertices[index] = this->vertices[indices[index]];
	}


	this->vertices = vertices;


	//	Indices are now irrelevant
	indices.clear();
}