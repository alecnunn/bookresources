// DirectX Quad Emitter

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Mathematics/Float.h"
#include "Mathematics/Vector.h"


namespace DirectX
{
    using namespace Mathematics;

    /// A simple wrapper around a dynamic vertex buffer.
    /// Emulates glBegin/glQuad/glEnd style semantics.
    /// Please note that this emitter is designed only to work with D3DFVF_XYZ format vertex streams!

    class QuadEmitter
    {
        LPDIRECT3DDEVICE9 _device;                      ///< the d3d device
        LPDIRECT3DVERTEXBUFFER9 _vertexBuffer;          ///< the dynamic vertex buffer
        LPDIRECT3DINDEXBUFFER9 _indexBuffer;            ///< the static vertex buffer with a repeated quad -> two triangles mapping for each quad
            
        Vector *_vertices;                              ///< pointer to locked vertices
        unsigned int _verticesSize;                     ///< size of vertices array (number of vertices)

        unsigned int _quadCount;                        ///< maximum number of quads that will fit into vertex buffer
        
        unsigned int _currentVertex;                    ///< current vertex in vertex buffer
        unsigned int _currentQuad;                      ///< current quad in vertex buffer

    public:

        /// default constructor.
        /// clears everything to defaults.

        QuadEmitter()
        {
            _device = 0;
            _vertexBuffer = 0;
            _indexBuffer = 0;
            _quadCount = 0;
            _vertices = 0;
            _verticesSize = 0;
            _currentQuad = 0;
            _currentVertex = 0;
        }
        
        /// initialize quad emitter.
        /// creates vertex and index buffers and prepares for rendering.
        /// note: you must initialize the quad emitter before you call any other methods!

        void initialize(LPDIRECT3DDEVICE9 device, int quads)
        {
            _device = device;

            _quadCount = quads;

            _verticesSize = quads*4;

            _currentVertex = 0;
            _currentQuad = 0;

            _device->CreateVertexBuffer((UINT)(sizeof(Vector)*_verticesSize), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &_vertexBuffer, 0);

		    _device->CreateIndexBuffer(2 * _quadCount * 3 * sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_indexBuffer, 0);

            // build index buffer

		    unsigned short *index = 0;

		    _indexBuffer->Lock(0, 0, (VOID**)&index, 0);

            int indices = quads*6;

            int vertex = 0;
            
            for (int i=0; i<indices; i+=6)
            {
                index[i] = (unsigned short) vertex;
                index[i+1] = (unsigned short) (vertex + 1);
                index[i+2] = (unsigned short) (vertex + 2);
                index[i+3] = (unsigned short) vertex;
                index[i+4] = (unsigned short) (vertex + 2);
                index[i+5] = (unsigned short) (vertex + 3);

                vertex += 4;
            }

            _indexBuffer->Unlock();
        }

        /// release quad emitter.
        /// releases vertex and index buffers.

	    void release()
	    {
            if (_vertexBuffer)
            {
                _vertexBuffer->Release();
                _vertexBuffer = 0;
            }

            if (_indexBuffer)
            {
                _indexBuffer->Release();
                _indexBuffer = 0;
            }
	    }

        /// destructor.
        /// automatically calls release.

        ~QuadEmitter()
        {
		    release();
        }

        /// begin a stream of quads.

        void begin()
        {
            // start buffered rendering

		    _device->SetIndices(_indexBuffer);

            _device->SetStreamSource(0, _vertexBuffer, 0, sizeof(Vector));

            _vertexBuffer->Lock(0, 0, (void**)&_vertices, D3DLOCK_DISCARD);
        }

        /// emit a quad.
        /// returns a pointer to the current quad vertex for the client to fill in.
        /// you should set the vertices to make up the quad. there are four indices
        /// in the returned pointer to vertices that are valid: [0]..[3].
             
        Vector* quad()
        {
            if (_currentQuad==_quadCount) flush();

            assert(_currentVertex<_verticesSize);
            assert(_currentVertex+1<_verticesSize);
            assert(_currentVertex+2<_verticesSize);
            assert(_currentVertex+3<_verticesSize);
                   
            Vector *pointer = _vertices + _currentVertex;

            _currentVertex += 4;
            _currentQuad ++;

            return pointer;
        }

        /// end quad stream.
        /// you must call this once you have finished emitting quads.

        void end()
        {
            flush(true);

            _device->SetIndices(0);
            _device->SetStreamSource(0,0,0,0);
        }

    protected:

        /// flush quads to the renderer.
        /// this method ensures that all buffered quads are rendered.

        void flush(bool end = false)
        {
            // unlock vertex buffer

            _vertexBuffer->Unlock();

            // safety check for empty buffer

            if (_currentQuad==0) return;

            // draw primitive
            
            _device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _currentVertex, 0, _currentQuad*2);

            // start again ...
            
            _currentVertex = 0;
            _currentQuad = 0;

            if (!end)
            {
                // lock vert buffer (discard)

                _vertexBuffer->Lock(0, 0, (void**)&_vertices, D3DLOCK_DISCARD);
            }
        }
    };
}
