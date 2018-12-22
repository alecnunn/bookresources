// Useful D3D-related stuff.

#ifndef TOMSD3DLIB_INCLUDED
#define TOMSD3DLIB_INCLUDED



//#include "Strings.h"


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

#ifndef STRICT
#define STRICT 1
#endif
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

// and some stuff that seems to have been left out of D3D8types.h
#define D3DFVF_RESERVED1        0x020
#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )
#define D3DFVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )


#else
#include "D3Dtypes.h"
#endif

#include "TomsLib.h"




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



struct Vec3;
struct Mat44;
struct Mat44Trans;




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
	int			iSizeTexCoord[8];		// How many components (UVTW) that texture coord has.

	int			iNumBlendWeights;		// Number of blend weights.
	int			iNumTexCoords;			// Number of texture sets.
	int			iSizeOfVertex;			// Size in bytes.

	void		*pvCurrentVert;			// The vertex this is currently pointing to.

public:

	// Sets the FVF type
	// Explicitely not inlined, because it's a comparitively expensive op, and
	// we don't want it to vanish from Vtune's view.
	void		SetFVFType ( DWORD dwType );

	// Sets the type and pointer from another FVF.
	// fvfOne=fvfTwo only works if they are already the same type,
	// this is the equivalent if you want to force the types to be the same.
	void Replicate ( MyFVFPointer &fvfpOther )
	{
		memcpy ( (void *)this, (void *)&fvfpOther, sizeof ( MyFVFPointer ) );
		ASSERT ( pvCurrentVert == fvfpOther.pvCurrentVert );
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
		SetFVFType ( dwType );
		pvCurrentVert = ptr;
	}

	// Copy constructor.
	MyFVFPointer ( MyFVFPointer &other )
	{
		this->Replicate ( other );
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
	Vec3		&Position ( void )
	{
		ASSERT ( iOffsetPosition != -1 );
		return ( *(Vec3 *)( (char *)pvCurrentVert + iOffsetPosition ) );
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
		ASSERT ( iSizeTexCoord[0] >= 1 );
		return ( TexCoord(0).fU );
	}
	float		&V0 ( void )
	{
		ASSERT ( iSizeTexCoord[0] >= 2 );
		return ( TexCoord(0).fV );
	}
	float		&U1 ( void )
	{
		ASSERT ( iSizeTexCoord[1] >= 1 );
		return ( TexCoord(1).fU );
	}
	float		&V1 ( void )
	{
		ASSERT ( iSizeTexCoord[1] >= 2 );
		return ( TexCoord(1).fV );
	}


	// Now some more nice overloaded operators.

	// Conversion to void*
	operator void*()
	{
		return ( (void *)pvCurrentVert );
	}

	// Indexing.
	void *operator[] ( const int iOffset )
	{
		return GetVertex ( iOffset );
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

#ifndef EXTERNAL_PROGS
	// Returns a string describing the FVF.
	// Not cached - obviously this is very slow.
	String GetTextName ( void );
#endif

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



struct Vec3
{
	float x,y,z;

	Vec3 ( void ) {};

	Vec3 ( float ax, float ay, float az )
	{
		x = ax;
		y = ay;
		z = az;
	}

	Vec3 ( const Vec3 &a )
	{
		x = a.x;
		y = a.y;
		z = a.z;
	}

	Vec3 &operator+= ( const Vec3 &a )
	{
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Vec3 &operator-= ( const Vec3 &a )
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Vec3 operator+ ( const Vec3 &a ) const
	{
		Vec3 res;
		res.x = x + a.x;
		res.y = y + a.y;
		res.z = z + a.z;
		return res;
	}

	Vec3 operator- ( const Vec3 &a ) const
	{
		Vec3 res;
		res.x = x - a.x;
		res.y = y - a.y;
		res.z = z - a.z;
		return res;
	}

	Vec3 operator- ( void ) const
	{
		Vec3 res;
		res.x = -x;
		res.y = -y;
		res.z = -z;
		return res;
	}

	// Dot product.
	float operator* ( const Vec3 &a ) const
	{
		return x * a.x + y * a.y + z * a.z;
	}

	// Cross product.
	Vec3 operator^ ( const Vec3 &a ) const
	{
		Vec3 res;
		// XYZZY!
		res.x = y * a.z - z * a.y;
		res.y = z * a.x - x * a.z;
		res.z = x * a.y - y * a.x;
		return res;
	}

	// Scale
	Vec3 operator* ( const float f ) const
	{
		Vec3 res;
		res.x = x * f;
		res.y = y * f;
		res.z = z * f;
		return res;
	}

	Vec3 operator/ ( const float f ) const
	{
		Vec3 res;
		float f2 = 1.0f / f;
		res.x = x * f2;
		res.y = y * f2;
		res.z = z * f2;
		return res;
	}

	Vec3 &operator*= ( const float f )
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	Vec3 &operator/= ( const float f )
	{
		float f2 = 1.0f / f;
		x *= f2;
		y *= f2;
		z *= f2;
		return *this;
	}

#if 0
	Vec3 operator* ( const D3DXMATRIX mat ) const
	{
		Vec3 res;
		D3DXVECTOR4 vec;
		D3DXVec3Transform ( &vec, (D3DXVECTOR3 *)(this), &mat );
		// Just drop vec.w
		res.x = vec.x;
		res.y = vec.y;
		res.z = vec.z;
		return res;
	}
#else

	// Ignore the W offset.
	Vec3 TransNorm ( const D3DXMATRIX &mat ) const
	{
		Vec3 res;
		D3DXVec3TransformNormal ( (D3DXVECTOR3 *)&res, (D3DXVECTOR3 *)(this), &mat );
		return res;
	}

	// Transform back to the w=1 plane.
	Vec3 TransCoord ( const D3DXMATRIX &mat ) const
	{
		Vec3 res;
		D3DXVec3TransformCoord ( (D3DXVECTOR3 *)&res, (D3DXVECTOR3 *)(this), &mat );
		return res;
	}

	// Just ditch the w result.
	Vec3 Trans ( const D3DXMATRIX &mat ) const
	{
		Vec3 res;
		D3DXVECTOR4 vec;
		D3DXVec3Transform ( &vec, (D3DXVECTOR3 *)(this), &mat );
		res.x = vec.x;
		res.y = vec.y;
		res.z = vec.z;
		return res;
	}

	// Ignore the W offset.
	Vec3 TransNorm ( const Mat44 &mat ) const;
	Vec3 TransNorm ( const Mat44Trans &mat ) const;
	// Transform back to the w=1 plane.
	Vec3 TransCoord ( const Mat44 &mat ) const;
	// Just ditch the w result.
	Vec3 Trans ( const Mat44 &mat ) const;
	Vec3 Trans ( const Mat44Trans &mat ) const;

#endif

	float LengthSq ( void ) const
	{
		return x * x + y * y + z * z;
	}

	float Length ( void ) const
	{
		return sqrtf ( x * x + y * y + z * z );
	}

	Vec3 Norm ( void ) const
	{
		float fRecipLen = 1.0f / this->Length();
		Vec3 res;
		res.x = x * fRecipLen;
		res.y = y * fRecipLen;
		res.z = z * fRecipLen;
		return res;
	}

	// Cast to a D3DXVECTOR3
	operator D3DXVECTOR3 ( void )
	{
		D3DXVECTOR3 res;
		res.x = x;
		res.y = y;
		res.z = z;
		return res;
	}

	// Cast to a D3DXVECTOR3*. This is a massive cheat, but the internals are the same. Honest.
	operator D3DXVECTOR3* ( void )
	{
		return (D3DXVECTOR3*)this;
	}

	operator const D3DXVECTOR3* ( void ) const
	{
		return (const D3DXVECTOR3*)this;
	}

	// Cast to a D3DVECTOR
	operator const D3DVECTOR ( void )
	{
		D3DVECTOR res;
		res.x = x;
		res.y = y;
		res.z = z;
		return res;
	}

	// Cast to a D3DVECTOR
	operator const D3DVECTOR ( void ) const 
	{
		D3DVECTOR res;
		res.x = x;
		res.y = y;
		res.z = z;
		return res;
	}

	// Conversion from D3DXVECTOR3
	Vec3 ( const D3DXVECTOR3 &src )
	{
		x = src.x;
		y = src.y;
		z = src.z;
	}

	// Conversion from D3DVECTOR3
	Vec3 ( const D3DVECTOR &src )
	{
		x = src.x;
		y = src.y;
		z = src.z;
	}

};




// Not very featureful at the moment.
struct Vec4
{
	float x,y,z,w;

	Vec4 ( void ) {};

	Vec4 ( float ax, float ay, float az, float aw = 1.0f )
	{
		x = ax;
		y = ay;
		z = az;
		w = aw;
	}

	Vec4 ( const Vec4 &a )
	{
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
	}

	Vec4 ( const Vec3 &a )
	{
		x = a.x;
		y = a.y;
		z = a.z;
		w = 1.0f;
	}

	operator D3DXVECTOR4 ( void )
	{
		D3DXVECTOR4 res;
		res.x = x;
		res.y = y;
		res.z = z;
		res.w = w;
		return res;
	}

	// Cast to a D3DXVECTOR4*. This is a massive cheat, but the internals are the same. Honest.
	operator D3DXVECTOR4* ( void )
	{
		return (D3DXVECTOR4*)this;
	}
	operator const D3DXVECTOR4* ( void ) const
	{
		return (const D3DXVECTOR4*)this;
	}

	// Cast to a Vec3*. This is an even worse cheat, but the x,y,z bits agree.
	// For heaven's sake don't go freeing() the block though, or relying on the size!!!
	// This is basically just so you can pass it to the D3DX functions.
	operator Vec3* ( void )
	{
		return (Vec3*)this;
	}
	operator const Vec3* ( void ) const
	{
		return (const Vec3*)this;
	}

	operator Vec3 ( void )
	{
		Vec3 res;
		res.x = x;
		res.y = y;
		res.z = z;
		return res;
	}

	// Conversion from D3DXVECTOR4
	Vec4 ( const D3DXVECTOR4 &src )
	{
		x = src.x;
		y = src.y;
		z = src.z;
		w = src.w;
	}


};



// A Mat44 that has been transposed. NOTE! This stores the non-transposed version of the matrix!
struct Mat44Trans
{
	Vec4 x, y, z, w;

	// And this actually does the work of transposing it.
	operator Mat44 ( void );
};



struct Mat44
{
	union {
		// This is what a D3DMATRIX looks like.
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		// This is what a D3DMATRIX looks like.
		float m[4][4];
		// This is what I like using.
		struct {
			Vec4		x, y, z, w;
		};
	};

	Mat44 ( void ) {};

	Mat44 ( const Vec4 &a, const Vec4 &b, const Vec4 &c, const Vec4 &d )
	{
		x = a;
		y = b;
		z = c;
		w = d;
	}

	Mat44 ( const Vec3 &xin, const Vec3 &yin, const Vec3 &zin, const Vec3 &pos )
	{
		x.x = xin.x; x.y = xin.y; x.z = xin.z; x.w = 0.0f;
		y.x = yin.x; y.y = yin.y; y.z = yin.z; y.w = 0.0f;
		z.x = zin.x; z.y = zin.y; z.z = zin.z; z.w = 0.0f;
		w.x = pos.x; w.y = pos.y; w.z = pos.z; w.w = 1.0f;
	}

	Mat44 ( const Vec3 &xin, const Vec3 &yin, const Vec3 &zin )
	{
		x.x = xin.x; x.y = xin.y; x.z = xin.z; x.w = 0.0f;
		y.x = yin.x; y.y = yin.y; y.z = yin.z; y.w = 0.0f;
		z.x = zin.x; z.y = zin.y; z.z = zin.z; z.w = 0.0f;
		w.x =  0.0f; w.y =  0.0f; w.z =  0.0f; w.w = 1.0f;
	}

	Mat44 ( float xinx, float xiny, float xinz, float xinw,
			float yinx, float yiny, float yinz, float yinw,
			float zinx, float ziny, float zinz, float zinw,
			float posx, float posy, float posz, float posw )
	{
		x.x = xinx; x.y = xiny; x.z = xinz; x.w = xinw;
		y.x = yinx; y.y = yiny; y.z = yinz; y.w = yinw;
		z.x = zinx; z.y = ziny; z.z = zinz; z.w = zinw;
		w.x = posx; w.y = posy; w.z = posz; w.w = posw;
	}

	Mat44 ( const Mat44 &a )
	{
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
	}

	operator D3DXMATRIX ( void )
	{
		D3DXMATRIX res;
		res._11 = x.x; res._12 = x.y; res._13 = x.z; res._14 = x.w;
		res._21 = y.x; res._22 = y.y; res._23 = y.z; res._24 = y.w;
		res._31 = z.x; res._32 = z.y; res._33 = z.z; res._34 = z.w;
		res._41 = w.x; res._42 = w.y; res._43 = w.z; res._44 = w.w;
		return res;
	}

	// Cast to a D3DXMATRIX*. This is a massive cheat, but the internals are the same. Honest.
	operator D3DXMATRIX* ( void )
	{
		return (D3DXMATRIX*)this;
	}
	operator const D3DXMATRIX* ( void ) const
	{
		return (const D3DXMATRIX*)this;
	}

	// Conversion from D3DXMATRIX
	Mat44 ( const D3DXMATRIX &src )
	{
		x.x = src._11; x.y = src._12; x.z = src._13; x.w = src._14;
		y.x = src._21; y.y = src._22; y.z = src._23; y.w = src._24;
		z.x = src._31; z.y = src._32; z.z = src._33; z.w = src._34;
		w.x = src._41; w.y = src._42; w.z = src._43; w.w = src._44;
	}

	// Return the transpose.
	Mat44Trans &T ( void )
	{
		// Except it just returns "this" renamed to a Mat44. Sneaky...
		return *(Mat44Trans*)this;
	}

	Mat44 operator* ( const Mat44 &a ) const
	{
		Mat44 res;
#define DO_COMP(p,q) res.p.q = x.q * a.p.x + y.q * a.p.y + z.q * a.p.z + w.q * a.p.w
		DO_COMP(x,x);
		DO_COMP(x,y);
		DO_COMP(x,z);
		DO_COMP(x,w);
		DO_COMP(y,x);
		DO_COMP(y,y);
		DO_COMP(y,z);
		DO_COMP(y,w);
		DO_COMP(z,x);
		DO_COMP(z,y);
		DO_COMP(z,z);
		DO_COMP(z,w);
		DO_COMP(w,x);
		DO_COMP(w,y);
		DO_COMP(w,z);
		DO_COMP(w,w);
#undef DO_COMP
		return res;
	}

	// Multiply with a transpose.
	Mat44 operator* ( const Mat44Trans &a ) const
	{
		Mat44 res;
	#define DO_COMP(p,q) res.p.q = x.q * a.x.p + y.q * a.y.p + z.q * a.z.p + w.q * a.w.p
		DO_COMP(x,x);
		DO_COMP(x,y);
		DO_COMP(x,z);
		DO_COMP(x,w);
		DO_COMP(y,x);
		DO_COMP(y,y);
		DO_COMP(y,z);
		DO_COMP(y,w);
		DO_COMP(z,x);
		DO_COMP(z,y);
		DO_COMP(z,z);
		DO_COMP(z,w);
		DO_COMP(w,x);
		DO_COMP(w,y);
		DO_COMP(w,z);
		DO_COMP(w,w);
	#undef DO_COMP
		return res;
	}

};


__inline Mat44Trans::operator Mat44 ( void )
{
	// Only when it is converted back to a Mat44 does it actually do any work.
	Mat44 res;
	res.x.x = x.x;
	res.x.y = y.x;
	res.x.z = z.x;
	res.x.w = w.x;
	res.y.x = x.y;
	res.y.y = y.y;
	res.y.z = z.y;
	res.y.w = w.y;
	res.z.x = x.z;
	res.z.y = y.z;
	res.z.z = z.z;
	res.z.w = w.z;
	res.w.x = x.w;
	res.w.y = y.w;
	res.w.z = z.w;
	res.w.w = w.w;
	return res;
}



// The vector*float scale done the other way around.
static Vec3 operator* ( const float f, const Vec3 &that )
{
	Vec3 res;
	res.x = that.x * f;
	res.y = that.y * f;
	res.z = that.z * f;
	return res;
}


// Vec4 * Mat44
static Vec4 operator* ( const Vec4 &v, const Mat44 &mat )
{
	Vec4 res;
	res.x = v.x * mat.x.x + v.y * mat.x.y + v.z * mat.x.z + v.w * mat.x.w;
	res.y = v.x * mat.y.x + v.y * mat.y.y + v.z * mat.y.z + v.w * mat.y.w;
	res.z = v.x * mat.z.x + v.y * mat.z.y + v.z * mat.z.z + v.w * mat.z.w;
	res.w = v.x * mat.w.x + v.y * mat.w.y + v.z * mat.w.z + v.w * mat.w.w;
	return res;
}

// Transpose version...
static Vec4 operator* ( const Vec4 &v, const Mat44Trans &mat )
{
	Vec4 res;
	res.x = v.x * mat.x.x + v.y * mat.y.x + v.z * mat.z.x + v.w * mat.w.x;
	res.y = v.x * mat.x.y + v.y * mat.y.y + v.z * mat.z.y + v.w * mat.w.y;
	res.z = v.x * mat.x.z + v.y * mat.y.z + v.z * mat.z.z + v.w * mat.w.z;
	res.w = v.x * mat.x.w + v.y * mat.y.w + v.z * mat.z.w + v.w * mat.w.w;
	return res;
}



// Ignore the W offset.
__inline Vec3 Vec3::TransNorm ( const Mat44 &mat ) const
{
	Vec3 res;
	res.x = x * mat.x.x + y * mat.y.x + z * mat.z.x;
	res.y = x * mat.x.y + y * mat.y.y + z * mat.z.y;
	res.z = x * mat.x.z + y * mat.y.z + z * mat.z.z;
	return res;
}


__inline Vec3 Vec3::TransNorm ( const Mat44Trans &mat ) const
{
	Vec3 res;
	res.x = x * mat.x.x + y * mat.x.y + z * mat.x.z;
	res.y = x * mat.y.x + y * mat.y.y + z * mat.y.z;
	res.z = x * mat.z.x + y * mat.z.y + z * mat.z.z;
	return res;
}

// Transform back to the w=1 plane.
__inline Vec3 Vec3::TransCoord ( const Mat44 &mat ) const
{
	Vec3 res;
	res.x   = x * mat.x.x + y * mat.y.x + z * mat.z.x + mat.w.x;
	res.y   = x * mat.x.y + y * mat.y.y + z * mat.z.y + mat.w.y;
	res.z   = x * mat.x.z + y * mat.y.z + z * mat.z.z + mat.w.z;
	float w = x * mat.x.w + y * mat.y.w + z * mat.z.w + mat.w.w;
	res *= 1.0f / w;
	return res;
}

// Just ditch the w result.
__inline Vec3 Vec3::Trans ( const Mat44 &mat ) const
{
	Vec3 res;
	res.x   = x * mat.x.x + y * mat.y.x + z * mat.z.x + mat.w.x;
	res.y   = x * mat.x.y + y * mat.y.y + z * mat.z.y + mat.w.y;
	res.z   = x * mat.x.z + y * mat.y.z + z * mat.z.z + mat.w.z;
	return res;
}

__inline Vec3 Vec3::Trans ( const Mat44Trans &mat ) const
{
	Vec3 res;
	res.x   = x * mat.x.x + y * mat.x.y + z * mat.x.z + mat.x.w;
	res.y   = x * mat.y.x + y * mat.y.y + z * mat.y.z + mat.y.w;
	res.z   = x * mat.z.x + y * mat.z.y + z * mat.z.z + mat.z.w;
	return res;
}




#endif //#ifndef TOMSD3DLIB_INCLUDED



