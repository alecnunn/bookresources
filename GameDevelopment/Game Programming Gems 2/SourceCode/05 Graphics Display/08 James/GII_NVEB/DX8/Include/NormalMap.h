/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _NORMALMAP_H_
#define _NORMALMAP_H_

#include <assert.h>

#if(DIRECT3D_VERSION >= 0x0800)

#include <d3d8.h>
#include <d3dx8.h>

#else

#include <ddraw.h>
#include <d3d.h>
#include <d3dx.h>

#endif

///////////////////////////////
// Class NormalMap
//
// Author : D. Sim Dietrich Jr.
//          NVIDIA Corporation
//
// Date : 09/27/00
//
//  This class contains utility functions to convert a 32 bit RGBA array of colors into 
// a 32 bit RGBA normal map.
///////////////////////////////
//
// Overview :
//
// Simply call the Convert32BitToNormalMap or Convert24BitToNormalMap function, supplying the appropriate 
// parameters, and a Heightmap will be written into the memory described by pOutput and the outputstride.
//
// You must specify the Heightmap generation method.  The most common formats are UseAverage, UseMaximum
// and UseAlpha.  This chooses which combination of color channels and method is used to determine
// the height of the bumps.
//
//////////////////////////
//
// Implementation Details :
//
//  NormalMap uses both the Singleton and Strategy patterns.  The Singleton pattern is used to
// ensure proper initialization of the smpMethods array.
// 
//  The Strategy pattern is used for the various Methods, which represent different strategies for
// converting a color map into a normal map.  The CalculationMethod enumeration is used to choose
// which method is employed in the Convert..() function.
//
//


class NormalMap
{
	public :

		//-----------------------------------------------------------------------------
		// Name: VectortoRGBA()
		// Desc: Turns a normalized vector into RGBA form. Used to encode vectors into
		//       a height map. 
		//-----------------------------------------------------------------------------
		static DWORD VectortoRGBA( D3DXVECTOR3* v, const DWORD& a )
		{
			DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
			DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
			DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
    
			return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
		}

		enum CalculationMethod
		{
			UseGrayScale = 0,
			UseAverage = 1,
			UseAlpha,
			UseRed,
			UseBlue,
			UseGreen,
			UseMaximum,
			UseLightness,
			UseColorSpace,
			MethodCount
		};

	private :


		class Method
		{
			public :
				virtual float Calculate( const DWORD& theColor ) = 0;
		};

		class UseLightness: public Method
		{
			public :
				virtual float Calculate( const DWORD& theColor )
				{
					float theMax = max( max( ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) / 255.0f ),
											 ( (float)( ( theColor & 0x0000ff00 ) >>  8 ) / 255.0f ) ),
											 ( (float)( ( theColor & 0x000000ff ) >>  0 ) / 255.0f ) );
					float theMin = min( min( ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) / 255.0f ),
											 ( (float)( ( theColor & 0x0000ff00 ) >>  8 ) / 255.0f ) ),
											 ( (float)( ( theColor & 0x000000ff ) >>  0 ) / 255.0f ) );


					return ( ( theMax + theMin ) / 2.0f );
				}
		};

		class ColorSpace : public Method
		{
			public :
				virtual float Calculate( const DWORD& theColor )
				{
					float fr = (float) ( ( theColor >> 16 ) & 0xFF );
					float fg = (float) ( ( theColor >> 8 ) & 0xFF );
					float fb = (float) ( ( theColor & 0xFF ) );
                
					fr /= 255.0f;
					fg /= 255.0f;
					fb /= 255.0f;
                
					float fa = ( 1.0f - fr ) * ( 1.0f - fg ) * ( 1.0f - fb );

					fa = 1.0f - fa;
                
					return fa;
				}
		};


		class GrayScale : public Method
		{
			public :
				virtual float Calculate( const DWORD& theColor )
				{
					return ( ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) * 0.30f / 255.0f ) +
						     ( (float)( ( theColor & 0x0000ff00 ) >>  8 ) * 0.59f / 255.0f ) +
					         ( (float)( ( theColor & 0x000000ff ) >>  0 ) * 0.11f / 255.0f ) ); 
				}
		};

		class Maximum : public Method
		{
			public :
				virtual float Calculate( const DWORD& theColor )
				{
					return max( max( ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) / 255.0f ),
						             ( (float)( ( theColor & 0x0000ff00 ) >>  8 ) / 255.0f ) ),
					               ( (float)( ( theColor & 0x000000ff ) >>  0 ) / 255.0f ) );
				}
		};

		class Average: public Method
		{
			public :
				virtual float Calculate( const DWORD& theColor )
				{
					return ( ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) / 255.0f ) + 
						     ( (float)( ( theColor & 0x0000ff00 ) >>  8 ) / 255.0f ) +
					         ( (float)( ( theColor & 0x000000ff ) >>  0 ) / 255.0f ) ) / 3.0f; 
				}
		};

		class Alpha: public Method
		{
			public :
			virtual float Calculate( const DWORD& theColor )
			{
				return ( (float)( ( theColor & 0xFF000000 ) >> 24 ) / 255.0f );
			}
		};
		class Red: public Method
		{
			public : 
			virtual float Calculate( const DWORD& theColor )
			{
				return ( (float)( ( theColor & 0x00ff0000 ) >> 16 ) / 255.0f );
			}
		};

		class Green: public Method
		{
			public : 
			virtual float Calculate( const DWORD& theColor )
			{
				return ( (float)( ( theColor & 0x0000ff00 ) >> 8 ) / 255.0f );
			}
		};
		class Blue: public Method
		{
			public : 
			virtual float Calculate( const DWORD& theColor )
			{
				return ( (float)( ( theColor & 0x000000FF ) >> 0 ) / 255.0f );
			}
		};

		static Method* smpMethods[ MethodCount ];

		static void Init()
		{
			if ( smpMethods[ UseGrayScale ] == 0 )
			{
				smpMethods[ UseGrayScale ] = new GrayScale;
				smpMethods[ UseAverage ]   = new Average;
				smpMethods[ UseRed ] = new Red;
				smpMethods[ UseAlpha ] = new Alpha;
				smpMethods[ UseGreen ] = new Green;
				smpMethods[ UseBlue ] = new Blue;
				smpMethods[ UseMaximum ] = new Maximum;
				smpMethods[ UseColorSpace ] = new ColorSpace;
			}
		}

		~NormalMap()
		{
			for ( int i = 0; i < MethodCount; ++i )
			{
				delete smpMethods[ i ], smpMethods[ i ] = 0;
			}
		};

	public :


		static bool Convert32BitToNormalMap( const unsigned int* pInput, 
											 const unsigned int& theInputStrideInBytes,
											 const unsigned int& theWidthInPixels,
											 const unsigned int& theHeightInPixels,
											       unsigned int* pOutput, 
											 const unsigned int& theOutputStrideInBytes,
											 const float& theHeightScaleFactor,
											 const CalculationMethod& theMethod )
		{
			Init();

			if ( ( !pInput ) || ( !pOutput ) || ( theInputStrideInBytes == 0 ) || ( theOutputStrideInBytes == 0 ) ) 
			{
				assert( false );
				return false;
			}


			unsigned int theColors[ 3 ];
			D3DXVECTOR3  theVectors[ 3 ];

			D3DXVECTOR3  theBasis[ 3 ];

			D3DXVECTOR3  theNormal;

			for ( unsigned int y = 0; y < theHeightInPixels; ++y )
			{
				unsigned int theNextY = y + 1;

				if ( theNextY == theHeightInPixels ) theNextY = 0;

				for ( unsigned int x = 0; x < theWidthInPixels; ++x )
				{
					theColors[ 0 ] = pInput[ ( y * theInputStrideInBytes / sizeof( unsigned int ) + x ) ];

					theVectors[ 0 ].x = 0;
					theVectors[ 0 ].y = 0;
					theVectors[ 0 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 0 ] ) * theHeightScaleFactor;

					DWORD theHeight = (DWORD)( theVectors[ 0 ].z * 255.0f );

					unsigned int theNextX = x + 1;

					if ( theNextX == theWidthInPixels ) theNextX = 0;

					theColors[ 1 ] = pInput[ ( y * theInputStrideInBytes / sizeof( unsigned int ) + theNextX ) ];

					theVectors[ 1 ].x = 1;
					theVectors[ 1 ].y = 0;
					theVectors[ 1 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 1 ] ) * theHeightScaleFactor;

					theColors[ 2 ] = pInput[ ( theNextY * theInputStrideInBytes / sizeof( unsigned int ) + x ) ];

					theVectors[ 2 ].x = 0;
					theVectors[ 2 ].y = 1;
					theVectors[ 2 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 2 ] ) * theHeightScaleFactor;


					// Now I can generate two vectors from two adjoining edges, representing the x and y axes

					theBasis[ 0 ] = theVectors[ 1 ] - theVectors[ 0 ]; 
					theBasis[ 1 ] = theVectors[ 2 ] - theVectors[ 0 ]; 

					// Take the Cross product to generate the surface normal at this point

					D3DXVec3Cross( &theBasis[ 2 ], &theBasis[ 0 ], &theBasis[ 1 ] );

					// now Normalize the vector
					D3DXVec3Normalize( &theNormal, &theBasis[ 2 ] );

					// Now convert vector to an RGBA value
					pOutput[ ( y * theOutputStrideInBytes / sizeof( unsigned int ) + x ) ] = 
						VectortoRGBA( &theNormal, theHeight );
				}


			}
			return true;
		}

		
		static bool Convert24BitToNormalMap( const unsigned char* pInput, 
											 const unsigned int& theInputStrideInBytes,
											 const unsigned int& theWidthInPixels,
											 const unsigned int& theHeightInPixels,
											       unsigned int* pOutput, 
											 const unsigned int& theOutputStrideInBytes,
											 const float& theHeightScaleFactor,
											 const CalculationMethod& theMethod )
		{
			Init();

			if ( ( !pInput ) || ( !pOutput ) || ( theInputStrideInBytes == 0 ) || ( theOutputStrideInBytes == 0 ) ) 
			{
				assert( false );
				return false;
			}

			if ( theMethod == UseAlpha ) 
			{
				assert( theMethod != UseAlpha );
				return false;
			}

			union Color
			{
				struct
				{
					char a,r,g,b;
				};
				DWORD m;
			};

			Color theColors[ 3 ];

			D3DXVECTOR3  theVectors[ 3 ];
			D3DXVECTOR3  theBasis[ 3 ];
			D3DXVECTOR3  theNormal;


			for ( unsigned int y = 0; y < theHeightInPixels; ++y )
			{
				unsigned int theNextY = y + 1;

				if ( theNextY == theHeightInPixels ) theNextY = 0;

				for ( unsigned int x = 0; x < theWidthInPixels; ++x )
				{
					
					theColors[ 0 ].r = pInput[ ( y * theInputStrideInBytes + x * 3 ) + 0 ];
					theColors[ 0 ].g = pInput[ ( y * theInputStrideInBytes + x * 3 ) + 1 ];
					theColors[ 0 ].b = pInput[ ( y * theInputStrideInBytes + x * 3 ) + 2 ];

					theVectors[ 0 ].x = 0;
					theVectors[ 0 ].y = 0;
					theVectors[ 0 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 0 ].m ) * theHeightScaleFactor;

					DWORD theHeight = (DWORD)( theVectors[ 0 ].z * 255.0f );

					unsigned int theNextX = x + 1;

					if ( theNextX == theWidthInPixels ) theNextX = 0;

					theColors[ 1 ].r = pInput[ ( y * theInputStrideInBytes + theNextX * 3 ) + 0 ];
					theColors[ 1 ].g = pInput[ ( y * theInputStrideInBytes + theNextX * 3 ) + 1 ];
					theColors[ 1 ].b = pInput[ ( y * theInputStrideInBytes + theNextX * 3 ) + 2 ];

					theVectors[ 1 ].x = 1;
					theVectors[ 1 ].y = 0;
					theVectors[ 1 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 1 ].m ) * theHeightScaleFactor;

					theColors[ 2 ].r = pInput[ ( theNextY * theInputStrideInBytes + x * 3 ) + 0 ];
					theColors[ 2 ].g = pInput[ ( theNextY * theInputStrideInBytes + x * 3 ) + 1 ];
					theColors[ 2 ].b = pInput[ ( theNextY * theInputStrideInBytes + x * 3 ) + 2 ];

					theVectors[ 2 ].x = 0;
					theVectors[ 2 ].y = 1;
					theVectors[ 2 ].z = smpMethods[ theMethod ]->Calculate( theColors[ 2 ].m ) * theHeightScaleFactor;


					// Now I can generate two vectors from two adjoining edges, representing the x and y axes

					theBasis[ 0 ] = theVectors[ 1 ] - theVectors[ 0 ]; 
					theBasis[ 1 ] = theVectors[ 2 ] - theVectors[ 0 ]; 

					// Take the Cross product to generate the surface normal at this point

					D3DXVec3Cross( &theBasis[ 2 ], &theBasis[ 0 ], &theBasis[ 1 ] );

					// now Normalize the vector
					D3DXVec3Normalize( &theNormal, &theBasis[ 2 ] );

					// Now convert vector to an RGBA value
					pOutput[ ( y * theOutputStrideInBytes / sizeof( unsigned int ) + x ) ] = 
						VectortoRGBA( &theNormal, theHeight );
				}
			}
			return true;
		}


		// If we are performing per-pixel lighting with a tangent space basis per vertex,
		// we are setting up a coordinate system in which we can represent a slice through a sphere
		// as a location on a disc and a planar distance from the disc.

		// Since in local tangent space the Z direction is perpendicular to the triangle,
		// it can be represented with a single scalar value across the triangle.  In practice,
		// Normals aren't identical and triangles are lit as if they were not planar, so we
		// compensate for this two ways :

		// a) We recalculate a new tangent space at each vertex ( doing this already for bump mapping )
		// b) We recalculate a new planar distance at each vertex, so its not strictly constsant, but
		//     should be very close

		static bool CreateXYDiscMap( const unsigned int& theWidthInPixels,
							  unsigned int* pOutput, 
							  const unsigned int& theOutputStrideInBytes )
		{
			Init();

			if ( ( !pOutput ) || ( theOutputStrideInBytes == 0 ) || ( theWidthInPixels <= 2 ) ) 
			{
				assert( false );
				return false;
			}

			D3DXVECTOR3  theVector, theNormal;

			// We are simply placing normals into the disc

			const unsigned int theDiameter = theWidthInPixels - 2;
			const float theRadius = (float)theDiameter / 2.0f;

			for ( unsigned int y = 0; y < theWidthInPixels; ++y )
			{
				theVector.y = ( y / ( .5f * ( theDiameter ) ) ) - 1.0f;

				for ( unsigned int x = 0; x < theWidthInPixels; ++x )
				{
					theVector.x = ( x / ( .5f * ( theDiameter ) ) ) - 1.0f;

					theVector.z = 0.0f;

					theNormal.x = 0.0f;
					theNormal.y = 0.0f;
					theNormal.z = 0.0f;
					DWORD XsqPlusYSq = 0x0;

					float theLengthSquared = 0.0f;

					// Zero out the borders and the areas outside the disc

					if ( ( x == 0 ) || ( x == ( theWidthInPixels - 1 ) ) ||
						 ( y == 0 ) || ( y == ( theWidthInPixels - 1 ) ) ||
						 ( D3DXVec3LengthSq( &theVector ) >= ( theRadius ) ) )
					{
						pOutput[ ( y * theOutputStrideInBytes / sizeof( unsigned int ) + x ) ] = 
							VectortoRGBA( &theNormal, XsqPlusYSq );
					}
					else
					{

						// now Normalize the vector
						D3DXVec3Normalize( &theNormal, &theVector );

						// Force the Z to be one
						theNormal.z = 1.0f;

						XsqPlusYSq = min( 0xFF, (DWORD)( 255.0f * ( theNormal.x * theNormal.x + theNormal.y * theNormal.y ) ) );

						// Now convert vector to an RGBA value
						pOutput[ ( y * theOutputStrideInBytes / sizeof( unsigned int ) + x ) ] = 
							VectortoRGBA( &theNormal, XsqPlusYSq );
					}

				}
			}
			return true;
		}



};

#endif  _NORMALMAP_H_

