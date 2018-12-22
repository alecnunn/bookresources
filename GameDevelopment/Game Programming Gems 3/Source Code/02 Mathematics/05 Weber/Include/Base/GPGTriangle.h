
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

#ifndef __GPGTriangle_h__
#define __GPGTriangle_h__




/******************************************************************************
class GPGTriangle

	References:
		Graphics Gems I, pg 390

******************************************************************************/
class GPGTriangle
	{
	public:
				GPGTriangle(void)
					{
					m_set=false;
					};

		void	SetVertexPointers(GPGVector3 *v1,GPGVector3 *v2,GPGVector3 *v3)
					{
					m_vertex[0]=v1;
					m_vertex[1]=v2;
					m_vertex[2]=v3;
					m_set=true;
					};

		void	SetFaceNormalPointer(GPGVector3 *set)		{ m_normal=set; };

		void	PrepareForIntersections(void);
		long	TriTriIntersection(const GPGTriangle *other);

	private:

		float	RayPlaneIntersection(const GPGVector3 &start,
													const GPGVector3 &extent);

		bool	ContainsPoint(const GPGVector3 &intersection);

		GPGVector3	*m_vertex[3];
		GPGVector3	*m_normal;
		bool		m_set;

					// prepared values
		float		m_normal_dot_point;
		long		i1,i2;
	};



GPGINLINE float GPGTriangle::RayPlaneIntersection(const GPGVector3 &start,
													const GPGVector3 &extent)
	{
	float denom;

	if( (denom=m_normal->DotProduct(extent)) == 0.0f)
		return 0.0f;

	return ( (m_normal_dot_point - m_normal->DotProduct(start)) / denom);
	}



#endif /* __GPGTriangle_h__ */
