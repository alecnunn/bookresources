// DirectX Mesh

#pragma once

#include <vector>
#include "System/Mesh.h"
#include "System/Vertex.h"

   
namespace DirectX
{
    /// Internal %DirectX vertex structure

	struct Vertex
	{
		float x,y,z;		///< vertex coordinates
		float nx,ny,nz;		///< vertex normal
		float u, v;			///< texture coordinates
	};

	#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

    typedef System::Mesh SystemMesh;

    /// %DirectX implementation of %Mesh

    class Mesh : public System::Mesh
	{
		LPDIRECT3DDEVICE9 _device;
		LPDIRECT3DVERTEXBUFFER9 _vertex_buffer;
		LPDIRECT3DINDEXBUFFER9 _index_buffer;

		int _vertex_count;
		int _index_count;
		int _triangle_count;

	public:

        /// create mesh object.
        /// initializes index and vertex buffers.

		Mesh(LPDIRECT3DDEVICE9 device, const std::vector<System::Vertex> &vertex, const std::vector<int> &index)
		{
			_device = 0;
			_vertex_buffer = 0;
			_index_buffer = 0;
			_vertex_count = 0;
			_index_count = 0;
			_triangle_count = 0;

			assert(device);
			_device = device;

            _vertex_count = (int) vertex.size();
			_index_count = (int) index.size();
			_triangle_count = _index_count / 3;

			assert(_vertex_count>0);
			assert(_index_count>0);
			assert((_index_count % 3)==0);

			if (_device->CreateVertexBuffer(sizeof(Vertex)*_vertex_count, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &_vertex_buffer, NULL) != D3D_OK) throw Error("create vertex buffer failed");

			Vertex *d3d_vertex = 0;
			if (_vertex_buffer->Lock(0,sizeof(Vertex)*_vertex_count,(void**)&d3d_vertex, 0) != D3D_OK) throw Error("vertex buffer lock failed");
			
			assert(d3d_vertex);

			for (int i=0; i<_vertex_count; i++)
			{
				d3d_vertex[i].x = vertex[i].position.x;
				d3d_vertex[i].y = vertex[i].position.y;
				d3d_vertex[i].z = vertex[i].position.z;
				d3d_vertex[i].nx = vertex[i].normal.x;
				d3d_vertex[i].ny = vertex[i].normal.y;
				d3d_vertex[i].nz = vertex[i].normal.z;
				d3d_vertex[i].u = vertex[i].u;
				d3d_vertex[i].v = vertex[i].v;
			}
			
			if (_vertex_buffer->Unlock() != D3D_OK) throw Error("unlock vertex buffer failed");

			// todo: support for 32bit index buffers... ?

			assert(_vertex_count<65536);

			if (_device->CreateIndexBuffer(2*_index_count, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_index_buffer, NULL) != D3D_OK) throw Error("create index buffer failed");
			
			unsigned short *d3d_index = 0;
			if (_index_buffer->Lock(0,2*_index_count,(void**)&d3d_index, 0) != D3D_OK) throw Error("index buffer lock failed");
			
			assert(d3d_index);

			for (i=0; i<_index_count; i++)
			{
				d3d_index[i] = (unsigned short) index[i];
			}
			
			if (_index_buffer->Unlock() != D3D_OK) throw Error("index buffer unlock failed");
        }

        /// destructor.

		~Mesh()
		{
			if (_vertex_buffer) 
			{
				_vertex_buffer->Release();
				_vertex_buffer = 0;
			}

			if (_index_buffer) 
			{
				_index_buffer->Release();
				_index_buffer = 0;
			}
		}

		virtual	void release()
		{
			SystemMesh::release();

			if (!references())
			{
				if (_vertex_buffer) 
				{
					_vertex_buffer->Release();
					_vertex_buffer = 0;
				}

				if (_index_buffer) 
				{
					_index_buffer->Release();
					_index_buffer = 0;
				}
			}
		}

		LPDIRECT3DVERTEXBUFFER9 vertex_buffer() const
		{
			return _vertex_buffer;
		}

		LPDIRECT3DINDEXBUFFER9 index_buffer() const
		{
			return _index_buffer;
		}

		int vertex_count() const
		{
			return _vertex_count;
		}

		int triangle_count() const
		{
			return _triangle_count;
		}
	};
}
