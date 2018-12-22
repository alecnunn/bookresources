/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _TEXTUREBASIS_H_
#define _TEXTUREBASIS_H_

#pragma pack( push, 1 )

class TextureBasis
{
	private :	

		struct TC
		{
			float s,t;
		};

	public :

		//////////////////////
		// GenerateBases :
		//
		//
		// This function takes an array of position, 2d texture coordinates and an index list
		// It returns a set of 3x3 basis matrices - one for each vertex - that represent
		// model space-> s,t, sxt space appropriate for tangent space bump mapping, per-pixel
		// lighting, etc.
		// 
		// Allocate your own array of ( 3 * sizeof(D3DXVECTOR3) * theVertexCound ) array  
		// as temporary storage for the basis matrix values at each vertex.  This array
		// is formatted as:
		//    float * a;
		//    a[ 0 ] = 
		// Well, code issues with allocation and stuff... 
		//
		// It attempts to smooth out discontinuities by summing all neighboring triangle's
		// basis matrices.  
		//

		static void GenerateBases(	const unsigned int& theVertexCount,
			 						const float* pPositions, 
									const unsigned int& theVertexStride,
									const float* p2DTexCoords,

									const unsigned short* pIndices,
									const unsigned int& theIndexCount,

									float* p3x3BasisMatrices,
									const bool& bNormalizeBases = true,
									const bool& bSmoothBases = true )
		{	
			D3DXVECTOR3* pTriBasis = new D3DXVECTOR3[ theIndexCount ];
			D3DXVECTOR3* pAvgBasis = new D3DXVECTOR3[ theVertexCount * 3 ];

			const D3DXVECTOR3* pPos = (D3DXVECTOR3 * )( pPositions );
			const TC* pTC = ( TC* )(  p2DTexCoords );

			// Zero out average basis for summing
			memset( pAvgBasis, 0x00, sizeof( D3DXVECTOR3 ) * theVertexCount * 3 );

			// first, for each triangle, generate a basis matrix
			for ( unsigned int i = 0; i < theIndexCount; i += 3, ++pPos )
			{
				float x0 = pPos[ pIndices[ i ] ].x;
				float y0 = pPos[ pIndices[ i ] ].y;
				float z0 = pPos[ pIndices[ i ] ].z;

				float s0 = pTC[ pIndices[ i ] ].s;
				float t0 = pTC[ pIndices[ i ] ].t;

				float x1 = pPos[ pIndices[ i + 1 ] ].x;
				float y1 = pPos[ pIndices[ i + 1 ] ].y;
				float z1 = pPos[ pIndices[ i + 1 ] ].z;

				float s1 = pTC[ pIndices[ i + 1 ] ].s;
				float t1 = pTC[ pIndices[ i + 1 ] ].t;

				float x2 = pPos[ pIndices[ i + 2 ] ].x;
				float y2 = pPos[ pIndices[ i + 2 ] ].y;
				float z2 = pPos[ pIndices[ i + 2 ] ].z;

				float s2 = pTC[ pIndices[ i + 2 ] ].s;
				float t2 = pTC[ pIndices[ i + 2 ] ].t;


				float dx = x1 - x0;
				float dy = y1 - y0;
				float dz = y2 - y0;

				float dx2 = x2 - x0;
				float dy2 = y2 - y0;
				float dz2 = z2 - z0;

				float ds = s1 - s0;
				float dt = t1 - t0;

				float ds2 = s2 - s0;
				float dt2 = t2 - t0;

				D3DXVECTOR3 out, out2;

				D3DXVec3Cross( &out, &D3DXVECTOR3( dx, ds, dt ), &D3DXVECTOR3( dx2, ds2, dt2 ) );
				D3DXVec3Normalize( &out2, &out );

				float dsdx = 0.0f;
				if ( out2.y != 0.0f )
				{
					dsdx = - out2.x / out2.y;
				}

				float dtdx = 0.0f;
				if ( out2.z != 0.0f )
				{
					dtdx = - out2.y / out2.z;
				}

				D3DXVec3Cross( &out, &D3DXVECTOR3( dy, ds, dt ), &D3DXVECTOR3( dy2, ds2, dt2 ) );
				D3DXVec3Normalize( &out2, &out );

				float dsdy = 0.0f;
				if ( out2.y != 0.0f )
				{
					dsdy = - out2.x / out2.y;
				}

				float dtdy = 0.0f;
				if ( out2.z != 0.0f )
				{
					dtdy = - out2.y / out2.z;
				}

				D3DXVECTOR3 out3;

				D3DXVec3Cross( &out, &D3DXVECTOR3( dz, ds, dt ), &D3DXVECTOR3( dz2, ds2, dt2 ) );
				D3DXVec3Normalize( &out3, &out );

				float dsdz = 0.0f;
				if ( out2.y != 0.0f )
				{
					dsdy = - out2.x / out2.y;
				}

				float dtdz = 0.0f;
				if ( out2.z != 0.0f )
				{
					dtdz = - out2.y / out2.z;
				}

				D3DXVECTOR3 S( dsdx, dsdy, dsdz );
				D3DXVECTOR3 T( dtdx, dtdy, dtdz );

				D3DXVECTOR3 SxT;

				D3DXVec3Cross( &SxT, &S, &T );

				pTriBasis[ i + 0 ].x =   S.x;
				pTriBasis[ i + 0 ].y =   T.x;
				pTriBasis[ i + 0 ].z = SxT.x;

				pTriBasis[ i + 1 ].x =   S.y;
				pTriBasis[ i + 1 ].y =   T.y;
				pTriBasis[ i + 1 ].z = SxT.y;

				pTriBasis[ i + 2 ].x =   S.z;
				pTriBasis[ i + 2 ].y =   T.z;
				pTriBasis[ i + 2 ].z = SxT.z;

				if ( bSmoothBases )
				{
					pAvgBasis[ pIndices[ i + 0 ] ] += pTriBasis[ i + 0 ];
					pAvgBasis[ pIndices[ i + 1 ] ] += pTriBasis[ i + 1 ];
					pAvgBasis[ pIndices[ i + 2 ] ] += pTriBasis[ i + 2 ];
				}
			}

			if ( bNormalizeBases && bSmoothBases )
			{
				for ( unsigned int j = 0; j < theVertexCount * 3; ++j )
				{
					pAvgBasis[ j ] /= ( D3DXVec3LengthSq( &pAvgBasis[ j ] ) );
				}
				memcpy( p3x3BasisMatrices, pAvgBasis, sizeof( D3DXVECTOR3 ) * 3 * theVertexCount );
			}
			else
			{
				memcpy( p3x3BasisMatrices, pTriBasis, sizeof( D3DXVECTOR3 ) * 3 * theVertexCount );
			}

			delete []pTriBasis;
			delete []pAvgBasis;
		}
			          


};

#pragma pack( pop )


#endif  _TEXTUREBASIS_H_

