
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

#ifndef __GPGInfoRender_h__
#define __GPGInfoRender_h__


#include "Base/GPGDataTypes.h"
#include "Base/GPGVector3.h"



/******************************************************************************
class GPGInfoRender

******************************************************************************/
class GPGInfoRender
	{
	public:

				// Set size of all following drawn points
virtual void	SetPointSize(float pixels)									=0;

				// draw array of points vertex[vertices]
				// if multicolor, points are independently colored with
				//		color[vertices], else only uses color[0]
				// if color==NULL, no colors are specified (uses current state)
virtual void	DrawPointArray(const GPGVector3 *vertex,long vertices,
								bool multicolor,const GPGVector3 *color)	=0;

				// vertices and colors as above
				// if strip, vertices are contiguous with vertices-1 segments
				// else vertices are independent pairs with vertices/2 segments
virtual	void	DrawLineArray(const GPGVector3 *vertex,long vertices,
						bool strip,bool multicolor,const GPGVector3 *color,
						F32 linewidth = 1,bool antialias=false)				=0;

				// vertices and colors as above
				// normals are treated the same as vertices
				// if strip, vertices are tristriped with vertices-2 triangles
				// else vertices are independent triplet with vertices/3
				//		triangles
virtual void	DrawTriangleArray(const GPGVector3 *vertex,
						const GPGVector3 *normal,long vertices,
						bool strip,bool multicolor,const GPGVector3 *color,
						F32 alpha = 1.0f)									=0;

				// draw screen-align single color text at the projected location
virtual	void	DrawAlignedText(const GPGVector3 &location,const char *text,
												const GPGVector3 &color)	=0;
	};

#endif /* __GPGInfoRender_h__ */
