/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
//
//	GPGIterators
//
//	DESCRIPTION
//    This file defines iterators that let you access the individual 
//    attributes of a mesh or vertex.  The current implementation is
//    based on strided pointers.
//
// They behave much like conventional pointers in C:
//   ptr.Next() is like ptr++;
//   ptr.Index(i) is like ptr[i];
//   x = ptr.Get() is like  x = ptr;
//   ptr.PointAt(foo) is like ptr = foo;  //Note: ptr and foo must have 
//                                        //      have been initialized by the
//                                        //      same GPGMesh (must have 
//                                        //      same stride).
//	NOTES
//    The entire implementation is in this header file 
//    so that all the functions will inline properly.
//    We have verified that the code generated is very efficient.
//    Future version will likely use a C++ template.
//

#ifndef GPGIterators_h
#define GPGIterators_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"
#include "GPGVector4.h"
#include "GPGVector3.h"
#include "GPGVector2.h"
#include "GPGFace.h"
#include "GPGVertexAttributes.h"

//
// Iterators (Strided Pointers)
//

class GPGVector3Iter
{
public:
    GPGVector3Iter() : m_stride(0), m_ptr(NULL) {}

	// NOTE:  This method should only be called from GPGMesh
	//        to initialize the stride.
    void SetStride(/*in*/ U32 stride) { m_stride = stride; }

	// Make this iterator point the location specified by "ptr" argument.
	// NOTE:  Both iterators must have been initialized by the same
	//        GPGMesh, Cloned, or guaranteed to have the same stride.
    void PointAt(/*in*/ GPGVector3* ptr) { m_ptr = (U8 *)ptr; }

	// Get value at current location
    GPGVector3* Get() { return (GPGVector3*)m_ptr; }

	// Get value at offset from the current location
    GPGVector3* Index(/*in*/ U32 index) { return (GPGVector3*)(m_ptr + index * m_stride); }

	// Get current value and set current to next location
    GPGVector3* Next() { GPGVector3* ptr = (GPGVector3*)m_ptr; m_ptr += m_stride; return ptr; }

	//	Make this iterator like pSrcIter
	void Clone(GPGVector3Iter* pSrcIter) { *this = *pSrcIter; }

private:
    U32 m_stride;	// Byte offset to get at the next element
    U8* m_ptr;		// Current element
};

class GPGVector2Iter
{
public:
    GPGVector2Iter() : m_stride(0), m_ptr(NULL) {}
    void SetStride(/*in*/ U32 stride) { m_stride = stride; }

    void PointAt(/*in*/ GPGVector2* ptr) { m_ptr = (U8 *)ptr; }
    GPGVector2* Get() { return (GPGVector2*)m_ptr; }
    GPGVector2* Index(/*in*/ U32 index) { return (GPGVector2*)(m_ptr + index * m_stride); }
    GPGVector2* Next() { GPGVector2* ptr = (GPGVector2*)m_ptr; m_ptr += m_stride; return ptr; }
	void Clone(GPGVector2Iter* pSrcIter) { *this = *pSrcIter; }

private:
    U32 m_stride;
    U8* m_ptr;
};

class GPGFaceIter
{
public:
    GPGFaceIter() : m_stride(0), m_ptr(NULL) {}
    void SetStride(/*in*/ U32 stride) { m_stride = stride; }

    void PointAt(/*in*/ GPGFace* ptr) { m_ptr = (U8 *)ptr; }
    GPGFace* Get() { return (GPGFace*)m_ptr; }
    GPGFace* Index(/*in*/ U32 index) { return (GPGFace*)(m_ptr + index * m_stride); }
    GPGFace* Next() { GPGFace* ptr = (GPGFace*)m_ptr; m_ptr += m_stride; return ptr; }
	void Clone(GPGFaceIter* pSrcIter) { *this = *pSrcIter; }

private:
    U32 m_stride;
    U8* m_ptr;
};


// For use with pencil sketch algorithm
class GPGFacePoolIter
{
public:
    GPGFacePoolIter() : m_stride(0), m_ptr(NULL) {}
    void SetStride(/*in*/ U32 stride) { m_stride = stride; }

    void PointAt(/*in*/ GPGFace** ptr) { m_ptr = (U8 *)ptr; }
    GPGFace** Get() { return (GPGFace**)m_ptr; }
    GPGFace** Next() { GPGFace** ptr = (GPGFace**)m_ptr; m_ptr += m_stride; return ptr; }
	void Clone(GPGFacePoolIter* pSrcIter) { *this = *pSrcIter; }

private:
    U32 m_stride;
    U8* m_ptr;
};


class U32Iter
{
public:
    U32Iter() : m_stride(0), m_ptr(NULL) {}
    void SetStride(/*in*/ U32 stride) { m_stride = stride; }

    void PointAt(/*in*/ U32* ptr) { m_ptr = (U8 *)ptr; }
    U32* Get() { return (U32*)m_ptr; }
    U32* Index(/*in*/ U32 index) { return (U32*)(m_ptr + index * m_stride); }
    U32* Next() { U32* ptr = (U32*)m_ptr; m_ptr += m_stride; return ptr; }
	void Clone(U32Iter* pSrcIter) { *this = *pSrcIter; }

private:
    U32 m_stride;
    U8* m_ptr;
};

// NOTE:
// GPGVertexIter is now an anchronism and is here for backward compatibility.
// Please use GPGVector3Iter, GPGVector2Iter, etc.  Thus you build a
// unique iterator for each attribute you wish to iterate over.

// This is a special iterator/pointer that abstracts access
// to the fields of a vertex record.
class GPGVertexIter
{
public:
    //
    // General interfaces
    //
    void PointAt(void* ptr) { m_ptr = (U8 *)ptr; }
    void* Get() { return (void*)m_ptr; }
    void* Index(U32 index) { return m_ptr + index * m_stride; }
    void* Next() { void* ptr = m_ptr; m_ptr += m_stride; return ptr; }
	void Clone(GPGVertexIter* pSrcIter) { *this = *pSrcIter; }

    // These methods are used in combination with GetPostion(),
    // GetNormal(), GetDiffuseColor(), GetTexCoord(), ...
    // to determine if the results are real (valid).
    GPGVertexAttributes GetAttributes() { return m_attributes; }
    U32 GetNumTextureLayers() { return m_attributes.numTextureLayers; }

    // Developer must check GetAttributeLayout for hasPosition
    GPGVector3* GetPosition()
    {
        GPGASSERT(m_attributes.hasPosition);
        return (GPGVector3*)(m_ptr + m_offsets.positionOffset);
    }

    // Developer must check GetAttributeLayout for hasNormal
    GPGVector3* GetNormal()
    {
        GPGASSERT(m_attributes.hasNormal);
        return (GPGVector3*)(m_ptr + m_offsets.normalOffset);
    }

    // Developer must check GetAttributeLayout for hasDiffuseColor
    U32* GetDiffuseColor()
    {
        GPGASSERT(m_attributes.hasDiffuseColor);
        return (U32*)(m_ptr + m_offsets.diffuseColorOffset);
    }

    // Developer must check GetAttributeLayout for hasTexCoords
    // and GetNumTextureLayers for number of texture layers.
    GPGVector2* GetTexCoord(U8 layer = 0)
    {
        GPGASSERT(m_attributes.numTextureLayers);
        GPGASSERT(layer < m_attributes.numTextureLayers);
        return (GPGVector2*)(m_ptr + m_offsets.textureOffset + sizeof(GPGVector2) * layer);
    }


    //
    // Interfaces for GPGMesh only!
    //
    struct AttributeOffsets
    {
        U8 positionOffset;
        U8 reciprocalHomogenousWOffset;
        U8 normalOffset;
        U8 diffuseColorOffset;
        U8 specularColorOffset;
        U8 textureOffset;
		U8 total;
        U8 padToEnd[1];
    };
    
    void SetStride(/*in*/ U32 stride,
                   /*in*/ GPGVertexAttributes* pAttributes,
                   /*in*/ AttributeOffsets* pOffsets)
    {
        m_stride = stride;
        m_attributes = *pAttributes;  // we copy for performance reasons
        m_offsets = *pOffsets;  // we copy for performance reasons
    }

protected:
    U32 m_stride;
    U8* m_ptr;
    GPGVertexAttributes m_attributes;
    AttributeOffsets m_offsets;
};


#endif
