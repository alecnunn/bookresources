// DirectX Line Emitter

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Mathematics/Float.h"
#include "Mathematics/Vector.h"


namespace DirectX
{
    using namespace Mathematics;

    /// A simple wrapper around a dynamic vertex buffer.
    /// Emulates glBegin/glLine/glEnd style semantics.
    /// Please note that this emitter is designed only to work with D3DFVF_XYZ format vertex streams!

    class LineEmitter
    {
        LPDIRECT3DDEVICE9 _device;                      ///< the d3d device
        LPDIRECT3DVERTEXBUFFER9 _vertexBuffer;          ///< the dynamic vertex buffer
            
        Vector *_vertices;                              ///< pointer to locked vertices
        unsigned int _verticesSize;                     ///< size of vertices array (number of vertices)

        unsigned int _lineCount;                        ///< maximum number of lines that will fit into vertex buffer
        
        unsigned int _currentVertex;                    ///< current vertex in vertex buffer
        unsigned int _currentLine;                      ///< current line in vertex buffer

    public:

        /// default constructor.
        /// clears everything to defaults.

        LineEmitter()
        {
            _device = 0;
            _vertexBuffer = 0;
            _lineCount = 0;
            _vertices = 0;
            _verticesSize = 0;
            _currentLine = 0;
            _currentVertex = 0;
        }
        
        /// initialize line emitter.
        /// creates vertex and index buffers and prepares for rendering.
        /// note: you must initialize the quad emitter before you call any other methods!

        void initialize(LPDIRECT3DDEVICE9 device, int lines)
        {
            _device = device;

            _lineCount = lines;

            _verticesSize = lines*2;

            _currentVertex = 0;
            _currentLine = 0;

            _device->CreateVertexBuffer((UINT)(sizeof(Vector)*_verticesSize), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_XYZ, D3DPOOL_DEFAULT, &_vertexBuffer, 0);
        }

        /// release line emitter.
        /// releases vertex buffer.

	    void release()
	    {
            if (_vertexBuffer)
            {
                _vertexBuffer->Release();
                _vertexBuffer = 0;
            }
	    }

        /// destructor.
        /// automatically calls release.

        ~LineEmitter()
        {
		    release();
        }

        /// begin a stream of lines.

        void begin()
        {
            // start buffered rendering

            _device->SetStreamSource(0, _vertexBuffer, 0, sizeof(Vector));

            _vertexBuffer->Lock(0, 0, (void**)&_vertices, D3DLOCK_DISCARD);
        }

        /// emit a line.
        /// returns a pointer to the current line vertex for the client to fill in.
        /// you should set the vertices to make up the line. there are two indices
        /// in the returned pointer to vertices that are valid: [0],[1].
             
        Vector* line()
        {
            if (_currentLine==_lineCount) flush();

            assert(_currentVertex<_verticesSize);
            assert(_currentVertex+1<_verticesSize);
                   
            Vector *pointer = _vertices + _currentVertex;

            _currentVertex += 2;
            _currentLine ++;

            return pointer;
        }

        /// end line stream.
        /// you must call this once you have finished emitting lines.

        void end()
        {
            flush(true);

            _device->SetStreamSource(0,0,0,0);
        }

    protected:

        /// flush lines to the renderer.
        /// this method ensures that all buffered lines are rendered.

        void flush(bool end = false)
        {
            // unlock vertex buffer

            _vertexBuffer->Unlock();

            // safety check for empty buffer

            if (_currentLine==0) return;

            // draw primitive
            
            _device->DrawPrimitive(D3DPT_LINELIST, 0, _currentLine);

            // start again ...
            
            _currentVertex = 0;
            _currentLine = 0;

            if (!end)
            {
                // lock vert buffer (discard)

                _vertexBuffer->Lock(0, 0, (void**)&_vertices, D3DLOCK_DISCARD);
            }
        }
    };
}
