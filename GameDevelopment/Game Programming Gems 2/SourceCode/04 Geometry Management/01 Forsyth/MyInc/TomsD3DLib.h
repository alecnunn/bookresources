/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
// Useful D3D-related stuff.

#ifndef TOMSD3DLIB_INCLUDED
#define TOMSD3DLIB_INCLUDED



#ifndef DXVERSION
#error Please define DXVERSION to be 7 or 8 or something.
#endif




#if DXVERSION==7
#define DIRECT3D_VERSION 0x0700
#define DIRECTINPUT_VERSION 0x0700
#elif DXVERSION==8
#define DIRECT3D_VERSION 0x0800
// Let the header file tell you what the latest version is.
//#define DIRECTINPUT_VERSION 0x0800
#else
#error Define DXVERSION to be 7 or 8. No other versions supported ATM.
#endif

#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <dinput.h>
//#include "D3DApp.h"
//#include "D3DTextr.h"
#include "DXUtil.h"
//#include "D3DMath.h"
//#include "D3DFile.h"

#if DXVERSION==8
#include "D3D8types.h"
#include "D3dx8core.h"
#else
#include "D3Dtypes.h"
#endif

#include "TomsLib.h"
#include "TomsD3DLib.h"




//#define TRACE sizeof


#if DXVERSION==7

#define LPDIRECTDRAWSURFACEn LPDIRECTDRAWSURFACE7
#define LPDIRECTDRAWn LPDIRECTDRAW7
#define LPDIRECT3DDEVICEn LPDIRECT3DDEVICE7
#define D3DDEVICEDESCn D3DDEVICEDESC7
#define DDSURFACEDESCn DDSURFACEDESC2

#elif DXVERSION==8

#define LPDIRECTDRAWSURFACEn LPDIRECT3DSURFACE8
#define LPDIRECTDRAWn IDontExistAnyMore
#define LPDIRECT3DDEVICEn LPDIRECT3DDEVICE8
#define D3DDEVICEDESCn D3DCAPS8
// DDSURFACEDESC has been split up into stuff like D3DLOCKED_RECT and D3DSURFACE_DESC
#define DDSURFACEDESCn IDontExistAnyMoreInAUsefulWay

#else

#error Define DXVERSION to be 7 or 8. No other versions supported ATM.

#endif



#if DXVERSION==7
// Initialise a D3D-style object
template <class S> void D3DInit ( S &thing )
{
	ZeroMemory ( &thing, sizeof ( S ) );
	thing.dwSize = sizeof ( S );
}
#endif //#if DXVERSION==7


// Check the ref count of a D3D object.
// Only legal with debug runtime.
template <class S> int CheckD3DRefCount ( S &thing )
{
	thing->AddRef();
	return ( thing->Release() );
}





// A struct to handle 4-component texture coords.
struct D3DTEXCOORD
{
	float		fU;
	float		fV;
	float		fW;
	float		fT;
};



// A generalised FVF semi-smart pointer.
class MyFVFPointer
{
private:
	DWORD		dwFVFType;				// The D3D format.
	// These are byte offsets from the start of the vertex.
	// If an offset is -1, that item is not present.
	int			iOffsetPosition;
	int			iOffsetRHW;
	int			iOffsetBlendWeights;
	int			iOffsetNormal;
	int			iOffsetPsize;
	int			iOffsetColour[2];		// AKA "diffuse" and "specular"
	int			iOffsetTexCoord[8];

	int			iNumBlendWeights;		// Number of blend weights.
	int			iNumTexCoords;		// Number of texture sets.
	int			iSizeOfVertex;			// Size in bytes.

	void		*pvCurrentVert;			// The vertex this is currently pointing to.

public:

	// Sets the FVF type
	void		SetFVFType ( DWORD dwType )
	{
		int i;

		// Initialise everything.
		dwFVFType = dwType;
		iOffsetPosition = -1;
		iOffsetRHW = -1;
		iOffsetBlendWeights = -1;
		iOffsetNormal = -1;
		iOffsetPsize = -1;
		iOffsetColour[0] = -1;
		iOffsetColour[1] = -1;
		iSizeOfVertex = 0;
		iNumBlendWeights = 0;
		iNumTexCoords = 0;
		for ( i = 0; i < 8; i++ )
		{
			iOffsetTexCoord[i] = -1;
		}

		if ( dwType == 0 )
		{
			// NULL type - not set up yet.
			return;
		}

		// Now find the components that are present.
		int iOffset = 0;

		// Bits 1,2,3 all code for a position type.
		switch ( dwType & D3DFVF_POSITION_MASK )
		{
		case 0:
			// Invalid.
			ASSERT ( FALSE );
			break;
		case D3DFVF_XYZ:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iNumBlendWeights = 0;
			break;
		case D3DFVF_XYZRHW:
			ASSERT ( ( dwType & D3DFVF_NORMAL ) == 0 );
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetRHW = iOffset;
			iOffset += 1 * sizeof ( float );
			iNumBlendWeights = 0;
			break;
		case D3DFVF_XYZB1:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetBlendWeights = iOffset;
			iNumBlendWeights = 1;
			iOffset += iNumBlendWeights * sizeof ( float );
			break;
		case D3DFVF_XYZB2:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetBlendWeights = iOffset;
			iNumBlendWeights = 2;
			iOffset += iNumBlendWeights * sizeof ( float );
			break;
		case D3DFVF_XYZB3:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetBlendWeights = iOffset;
			iNumBlendWeights = 3;
			iOffset += iNumBlendWeights * sizeof ( float );
			break;
		case D3DFVF_XYZB4:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetBlendWeights = iOffset;
			iNumBlendWeights = 4;
			iOffset += iNumBlendWeights * sizeof ( float );
			break;
		case D3DFVF_XYZB5:
			iOffsetPosition = iOffset;
			iOffset += 3 * sizeof ( float );
			iOffsetBlendWeights = iOffset;
			iNumBlendWeights = 5;
			iOffset += iNumBlendWeights * sizeof ( float );
			break;
		}

#if DXVERSION==7
		if ( dwType & D3DFVF_RESERVED1 )
		{
			// A special-case D3DLVERTEX. Make sure it's correct.
			ASSERT ( dwType == D3DFVF_LVERTEX );
			// No offset to mark, just a DWORD to skip.
			iOffset += sizeof ( float );
		}
#endif

		if ( dwType & D3DFVF_NORMAL )
		{
			iOffsetNormal = iOffset;
			iOffset += 3 * sizeof ( float );
		}
#if DXVERSION==8
		if ( dwType & D3DFVF_PSIZE )
		{
			iOffsetPsize = iOffset;
			iOffset += 1 * sizeof ( float );
		}
#endif
		if ( dwType & D3DFVF_DIFFUSE )
		{
			iOffsetColour[0] = iOffset;
			iOffset += 1 * sizeof ( DWORD );
		}
		if ( dwType & D3DFVF_SPECULAR )
		{
			iOffsetColour[1] = iOffset;
			iOffset += 1 * sizeof ( DWORD );
		}

		// Now do the textures.
		// Slight hardwiring because the D3D headers don't give quite enough info.
#if D3DFVF_TEXCOORDSIZE1(3) != ( 0x3 << ( 3 * 2 + 16 ) )
#error Hardwired FVF format flags do not match headers.
#endif
		iNumTexCoords = ( dwType & D3DFVF_TEXCOUNT_MASK ) >> D3DFVF_TEXCOUNT_SHIFT;
		DWORD dwTemp = dwType >> 16;
		for ( i = 0; i < iNumTexCoords; i++ )
		{
			// Set up the offsets.
			switch ( dwTemp & 0x3 )
			{
			case D3DFVF_TEXTUREFORMAT1:
				// 1 component.
				iOffsetTexCoord[i] = iOffset;
				iOffset += 1 * sizeof ( float );
				break;
			case D3DFVF_TEXTUREFORMAT2:
				// 1 component.
				iOffsetTexCoord[i] = iOffset;
				iOffset += 2 * sizeof ( float );
				break;
			case D3DFVF_TEXTUREFORMAT3:
				// 1 component.
				iOffsetTexCoord[i] = iOffset;
				iOffset += 3 * sizeof ( float );
				break;
			case D3DFVF_TEXTUREFORMAT4:
				// 1 component.
				iOffsetTexCoord[i] = iOffset;
				iOffset += 4 * sizeof ( float );
				break;
			default:
				// The maths fell over somewhere.
				ASSERT ( FALSE );
				break;
			}
			dwTemp >>= 2;
		}

		// The size of the whole vertex.
		iSizeOfVertex = iOffset;
#if DXVERSION==8
		// D3DX is almost useful!
		ASSERT ( (unsigned)iSizeOfVertex == D3DXGetFVFVertexSize ( dwFVFType ) );
#endif
	}

	// Gets the FVF type
	DWORD		GetFVFType ( void )
	{
		return dwFVFType;
	}

	// Assignment - can only be done between FVF pointers of the same type.
	MyFVFPointer & operator= ( const MyFVFPointer &other )
	{
		ASSERT ( dwFVFType == other.dwFVFType );
		pvCurrentVert = other.pvCurrentVert;
		ASSERT ( 0 == memcmp ( (void *)this, (void *)&other, sizeof ( MyFVFPointer ) ) );
		return ( *this );
	}

	// Assignment from a void*
	MyFVFPointer & operator= ( void *ptr )
	{
		// Can only assign NULL to an untyped vertex.
		ASSERT ( ( dwFVFType != 0 ) || ( ptr == NULL ) );
		pvCurrentVert = ptr;
		return ( *this );
	}

	// Comparison - can only be done between FVF pointers of the same type.
	bool operator== ( const MyFVFPointer &other )
	{
		ASSERT ( dwFVFType == other.dwFVFType );
		if ( pvCurrentVert == other.pvCurrentVert )
		{
			ASSERT ( 0 == memcmp ( (void *)this, (void *)&other, sizeof ( MyFVFPointer ) ) );
			return ( TRUE );
		}
		else
		{
			return ( FALSE );
		}
	}

	// Constructor.
	MyFVFPointer ( DWORD dwType = 0 )
	{
		SetFVFType ( dwType );
	}

	// Constructor from type & pointer.
	MyFVFPointer ( DWORD dwType, void *ptr )
	{
		// Just a straight memberwise copy.
		SetFVFType ( dwType );
		pvCurrentVert = ptr;
	}

	// Copy constructors.
	MyFVFPointer ( MyFVFPointer &other )
	{
		// Just a straight memberwise copy.
		memcpy ( (void *)this, (void *)&other, sizeof ( MyFVFPointer ) );
	}


	// Destruction.
	~MyFVFPointer ( void ) {}


	// Gets the FVF size in bytes
	DWORD		GetFVFSize ( void )
	{
		return ( iSizeOfVertex );
	}

	// Set current vertex base
	void		SetCurVertex ( void *ptr )
	{
		ASSERT ( dwFVFType != 0 );
		pvCurrentVert = ptr;
	}
	// Get current vertex base
	void		*GetCurVertex ( void )
	{
		return ( pvCurrentVert );
	}
	// Move the vertex to the next one.
	void		NextVertex ( void )
	{
		pvCurrentVert = GetVertex ( 1 );
	}
	// Move the vertex to the previous one.
	void		PrevVertex ( void )
	{
		pvCurrentVert = GetVertex ( -1 );
	}
	// Indexed offset. Does not affect the current pointer.
	// iOffset is signed - can be -ve or +ve.
	void		*GetVertex ( int iOffset )
	{
		return ( (void *)( (char *)pvCurrentVert + iSizeOfVertex * iOffset ) );
	}

	// Allocate some vertices.
	// Return is TRUE on success.
	bool		AllocVertices ( int iNumVertices )
	{
		ASSERT ( dwFVFType != 0 );
		ASSERT ( iSizeOfVertex > 0 );
		pvCurrentVert = malloc ( iNumVertices * iSizeOfVertex );
		if ( pvCurrentVert != NULL )
		{
			return ( TRUE );
		}
		else
		{
			return ( FALSE );
		}
	}

	// Rellocate some vertices.
	// Return is TRUE on success.
	bool		ReallocVertices ( int iNumVertices )
	{
		ASSERT ( dwFVFType != 0 );
		ASSERT ( iSizeOfVertex > 0 );
		pvCurrentVert = realloc ( pvCurrentVert, iNumVertices * iSizeOfVertex );
		if ( pvCurrentVert != NULL )
		{
			return ( TRUE );
		}
		else
		{
			return ( FALSE );
		}
	}

	// Free the vertices.
	void		FreeVertices ( void )
	{
		if ( pvCurrentVert != NULL )
		{
			free ( pvCurrentVert );
			pvCurrentVert = NULL;
		}
	}

	// Individual vertex component pointer getters.
	D3DVECTOR	&Position ( void )
	{
		ASSERT ( iOffsetPosition != -1 );
		return ( *(D3DVECTOR *)( (char *)pvCurrentVert + iOffsetPosition ) );
	}
	float		&X ( void )
	{
		ASSERT ( iOffsetPosition != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetPosition + 0 * sizeof ( float ) ) );
	}
	float		&Y ( void )
	{
		ASSERT ( iOffsetPosition != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetPosition + 1 * sizeof ( float ) ) );
	}
	float		&Z ( void )
	{
		ASSERT ( iOffsetPosition != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetPosition + 2 * sizeof ( float ) ) );
	}
	float		&RHW ( void )
	{
		ASSERT ( iOffsetRHW != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetRHW ) );
	}
	float		&BlendWeight ( int iWeightNum )
	{
		ASSERT ( iOffsetBlendWeights != -1 );
		ASSERT ( iWeightNum < iNumBlendWeights );
#if DXVERSION==8
		// The last weight isn't a weight, it's indices, when LASTBETA_UBYTE4 is set.
		ASSERT ( ( ( dwFVFType & D3DFVF_LASTBETA_UBYTE4 ) == 0 ) || ( iWeightNum < iNumBlendWeights - 1 ) );
#endif
		return ( *(float *)( (char *)pvCurrentVert + iOffsetBlendWeights + iWeightNum * sizeof ( float ) ) );
	}
#if DXVERSION==8
	DWORD		&BlendIndices ( void )
	{
		ASSERT ( iOffsetBlendWeights != -1 );
		ASSERT ( iNumBlendWeights > 0 );
		ASSERT ( ( dwFVFType & D3DFVF_LASTBETA_UBYTE4 ) != 0 );
		// Last "blend weight" is the packed indices.
		return ( *(DWORD *)( (char *)pvCurrentVert + iOffsetBlendWeights + ( iNumBlendWeights - 1 ) * sizeof ( float ) ) );
	}
	BYTE		&BlendIndex ( int iIndexNum )
	{
		ASSERT ( iOffsetBlendWeights != -1 );
		ASSERT ( iNumBlendWeights > 0 );
		ASSERT ( ( dwFVFType & D3DFVF_LASTBETA_UBYTE4 ) != 0 );
		// Last "blend weight" is the packed indices.
		ASSERT ( iIndexNum < 4 );
		return ( *(BYTE *)( (char *)pvCurrentVert + iOffsetBlendWeights + iIndexNum + ( iNumBlendWeights - 1 ) * sizeof ( float ) ) );
	}
#endif
	D3DVECTOR	&Normal ( void )
	{
		ASSERT ( iOffsetNormal != -1 );
		return ( *(D3DVECTOR *)( (char *)pvCurrentVert + iOffsetNormal ) );
	}
	float		&NX ( void )
	{
		ASSERT ( iOffsetNormal != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetNormal + 0 * sizeof ( float ) ) );
	}
	float		&NY ( void )
	{
		ASSERT ( iOffsetNormal != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetNormal + 1 * sizeof ( float ) ) );
	}
	float		&NZ ( void )
	{
		ASSERT ( iOffsetNormal != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetNormal + 2 * sizeof ( float ) ) );
	}
	float		&Psize ( void )
	{
		ASSERT ( iOffsetPsize != -1 );
		return ( *(float *)( (char *)pvCurrentVert + iOffsetPsize ) );
	}
	DWORD		&Colour ( int iColourNum )		// Counted from 0.
	{
		// Only two colours at max ATM.
		ASSERT ( iColourNum < 2 );
		ASSERT ( iOffsetColour[iColourNum] != -1 );
		return ( *(DWORD *)( (char *)pvCurrentVert + iOffsetColour[iColourNum] ) );
	}
	DWORD		&Diff ( void )
	{
		return ( Colour ( 0 ) );
	}
	DWORD		&Spec ( void )
	{
		return ( Colour ( 1 ) );
	}
	D3DTEXCOORD	&TexCoord ( int iTexCoordNum )	// Counted from 0.
	{
		ASSERT ( iTexCoordNum < iNumTexCoords );
		ASSERT ( iOffsetTexCoord[iTexCoordNum] != -1 );
		return ( *(D3DTEXCOORD *)( (char *)pvCurrentVert + iOffsetTexCoord[iTexCoordNum] ) );
	}
	float		&U0 ( void )
	{
		return ( TexCoord(0).fU );
	}
	float		&V0 ( void )
	{
		return ( TexCoord(0).fV );
	}
	float		&U1 ( void )
	{
		return ( TexCoord(1).fU );
	}
	float		&V1 ( void )
	{
		return ( TexCoord(1).fV );
	}


	// Now some more nice overloaded operators.

	// Conversion to void*
	operator void*()
	{
		return ( (void *)pvCurrentVert );
	}

	// Subscript - may be positive or negative.
	// NOTE! This is awesomely inefficient unless the compiler is smart. :-(
	MyFVFPointer operator[] ( const int iOffset )
	{
		MyFVFPointer ret = *this;
		ret.pvCurrentVert = GetVertex ( iOffset );
		return ret;
	}

	// Addition.
	MyFVFPointer &operator+= ( const int iOffset )
	{
		pvCurrentVert = GetVertex ( iOffset );
		return ( *this );
	}

	// Subtraction.
	MyFVFPointer &operator-= ( const int iOffset )
	{
		pvCurrentVert = GetVertex ( -iOffset );
		return ( *this );
	}

	// Next (prefix).
	MyFVFPointer &operator++ ( void )
	{
		pvCurrentVert = GetVertex ( 1 );
		return ( *this );
	}

	// Prev (prefix).
	MyFVFPointer &operator-- ( void )
	{
		pvCurrentVert = GetVertex ( -1 );
		return ( *this );
	}

};





// Fill the matViewport matrix from the
// viewport data in vpViewport.
// Very useful for concatenating the whole
// vertex transform pipeline.
// TomF.
#if DXVERSION==8
static void MakeMatrixFromViewport ( D3DMATRIX &matViewport, D3DVIEWPORT8 &vpViewport )
#else
static void MakeMatrixFromViewport ( D3DMATRIX &matViewport, D3DVIEWPORT7 &vpViewport )
#endif
{

#if DXVERSION==8
	float fHalfWidth = vpViewport.Width * 0.5f;
	float fHalfHeight = vpViewport.Height * -0.5f;
#else
	float fHalfWidth = vpViewport.dwWidth * 0.5f;
	float fHalfHeight = vpViewport.dwHeight * -0.5f;
#endif

	matViewport._11 = fHalfWidth;
	matViewport._12 = 0.0f;
	matViewport._13 = 0.0f;
	matViewport._14 = 0.0f;

	matViewport._21 = 0.0f;
	matViewport._22 = fHalfHeight;
	matViewport._23 = 0.0f;
	matViewport._24 = 0.0f;

#if DXVERSION==8
	matViewport._31 = 0.0f;
	matViewport._32 = 0.0f;
	matViewport._33 = vpViewport.MaxZ - vpViewport.MinZ;
	matViewport._34 = 0.0f;

	matViewport._41 = vpViewport.X + fHalfWidth;
	matViewport._42 = vpViewport.Y - fHalfHeight;
	matViewport._43 = vpViewport.MinZ;
	matViewport._44 = 1.0f;
#else
	matViewport._31 = 0.0f;
	matViewport._32 = 0.0f;
	matViewport._33 = vpViewport.dvMaxZ - vpViewport.dvMinZ;
	matViewport._34 = 0.0f;

	matViewport._41 = vpViewport.dwX + fHalfWidth;
	matViewport._42 = vpViewport.dwY - fHalfHeight;
	matViewport._43 = vpViewport.dvMinZ;
	matViewport._44 = 1.0f;
#endif
}




// Multi-format support. Not very fast, but useful. TomF.

struct FloatColour
{
	float fA, fR, fG, fB;

	FloatColour operator+( FloatColour fc )
	{
		fc.fA += this->fA;
		fc.fR += this->fR;
		fc.fG += this->fG;
		fc.fB += this->fB;
		return fc;
	}

	FloatColour operator*( float fScale )
	{
		FloatColour fc;
		fc.fA = this->fA * fScale;
		fc.fR = this->fR * fScale;
		fc.fG = this->fG * fScale;
		fc.fB = this->fB * fScale;
		return fc;
	}
};



#if DXVERSION>=8

// Fake up the essential DDPIXELFORMAT data from DX8 FORMAT info.
struct DX8PIXELFORMAT
{
	DWORD dwFlags;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
};

// Make these up, as they are not in DX8.
#define DDPF_RGB 0x1
#define DDPF_ALPHAPIXELS 0x2

#endif //#if DXVERSION==8



#if DXVERSION>=8
static FloatColour ConvertPixelDataToFloat ( DWORD dwSrc, const DX8PIXELFORMAT &ddpfSrc )
#else
static FloatColour ConvertPixelDataToFloat ( DWORD dwSrc, const DDPIXELFORMAT &ddpfSrc )
#endif
{
	FloatColour fc;
	ASSERT ( ( ddpfSrc.dwFlags & DDPF_RGB ) != 0 );
	fc.fR = ( (float)( dwSrc & ddpfSrc.dwRBitMask        ) / (float)ddpfSrc.dwRBitMask );
	fc.fG = ( (float)( dwSrc & ddpfSrc.dwGBitMask        ) / (float)ddpfSrc.dwGBitMask );
	fc.fB = ( (float)( dwSrc & ddpfSrc.dwBBitMask        ) / (float)ddpfSrc.dwBBitMask );
	fc.fA = ( (float)( dwSrc & ddpfSrc.dwRGBAlphaBitMask ) / (float)ddpfSrc.dwRGBAlphaBitMask );
	return fc;
}

#if DXVERSION>=8
static DWORD ConvertFloatToPixelData ( const FloatColour &fc, const DX8PIXELFORMAT &ddpfDst )
#else
static DWORD ConvertFloatToPixelData ( const FloatColour &fc, const DDPIXELFORMAT &ddpfDst )
#endif
{
	ASSERT ( ( ddpfDst.dwFlags & DDPF_RGB ) != 0 );
	DWORD dwRes = 0;
	dwRes |= ( (DWORD)( fc.fR * (float)( ddpfDst.dwRBitMask        ) ) & ddpfDst.dwRBitMask );
	dwRes |= ( (DWORD)( fc.fG * (float)( ddpfDst.dwGBitMask        ) ) & ddpfDst.dwGBitMask );
	dwRes |= ( (DWORD)( fc.fB * (float)( ddpfDst.dwBBitMask        ) ) & ddpfDst.dwBBitMask );
	dwRes |= ( (DWORD)( fc.fA * (float)( ddpfDst.dwRGBAlphaBitMask ) ) & ddpfDst.dwRGBAlphaBitMask );
	return dwRes;
}


// Convert between two different formats. Both must be RGB.
#if DXVERSION>=8
static DWORD ConvertPixelData ( const DX8PIXELFORMAT &ddpfDst, DWORD dwSrc, const DX8PIXELFORMAT &ddpfSrc )
#else
static DWORD ConvertPixelData ( const DDPIXELFORMAT &ddpfDst, DWORD dwSrc, const DDPIXELFORMAT &ddpfSrc )
#endif
{
	FloatColour fc = ConvertPixelDataToFloat ( dwSrc, ddpfSrc );
	return ConvertFloatToPixelData ( fc, ddpfDst );
}


#if DXVERSION>=8
static const DX8PIXELFORMAT ddpfR8G8B8A8 = {
	DDPF_ALPHAPIXELS | DDPF_RGB,	// dwFlags
	0x00FF0000,						// dwRBitMask,
	0x0000FF00,						// dwGBitMask,
	0x000000FF,						// dwBBitMask,
	0xFF000000						// dwRGBAlphaBitMask
};
#else
static const DDPIXELFORMAT ddpfR8G8B8A8 = {
	sizeof ( DDPIXELFORMAT ),		// dwSize
	DDPF_ALPHAPIXELS | DDPF_RGB,	// dwFlags
	0,								// dwFourCC
	32,								// dwRGBBitCount,
	0x00FF0000,						// dwRBitMask,
	0x0000FF00,						// dwGBitMask,
	0x000000FF,						// dwBBitMask,
	0xFF000000						// dwRGBAlphaBitMask
};
#endif


// Convert from the given pixel format to A8R8G8B8
#if DXVERSION>=8
static DWORD ConvertPixelToA8R8G8B8 ( const DX8PIXELFORMAT &ddpf, DWORD dwSrc )
#else
static DWORD ConvertPixelToA8R8G8B8 ( const DDPIXELFORMAT &ddpf, DWORD dwSrc )
#endif
{
	return ConvertPixelData ( ddpfR8G8B8A8, dwSrc, ddpf );
}

// Convert from A8R8G8B8 to the given pixel format.
#if DXVERSION>=8
static DWORD ConvertPixelFromA8R8G8B8 ( const DX8PIXELFORMAT &ddpf, DWORD dwSrc )
#else
static DWORD ConvertPixelFromA8R8G8B8 ( const DDPIXELFORMAT &ddpf, DWORD dwSrc )
#endif
{
	return ConvertPixelData ( ddpf, dwSrc, ddpfR8G8B8A8 );
}



// An attempt at a "universal" blitter that handles any texture formats,
// and handles different filtering modes. Adapt as needed.

// Filter types.
enum UblitFilter
{
	UBLIT_FILTER_POINT,
	UBLIT_FILTER_BILINEAR,
};



#if DXVERSION==7
// Returns TRUE if successful, FALSE otherwise.
BOOL UniversalBlit ( LPDIRECTDRAWSURFACEn pddsSrc, LPDIRECTDRAWSURFACEn pddsDst,
							UblitFilter ufMin = UBLIT_FILTER_POINT,
							UblitFilter ufMag = UBLIT_FILTER_POINT );
#endif



#if DXVERSION==7
// Given a locked DD surface and its surface desc, returns the pixel data at the given coords.
// Clamps to the surface's size in X and Y.
DWORD SamplePoint ( int iX, int iY, DDSURFACEDESC2 &ddsdSrc );
#else
// Given a locked DD surface and its surface desc, returns the pixel data at the given coords.
// Clamps to the surface's size in X and Y.
DWORD SamplePoint ( int iX, int iY, int iBitsPerPixel, D3DSURFACE_DESC &ddsdSrc, D3DLOCKED_RECT &locked_rect );
#endif




#endif //#ifndef TOMSD3DLIB_INCLUDED



