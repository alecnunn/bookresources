/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 7.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: mathD3D.h
 *    Desc: structs that look quite a bit like D3D structs
 *          but have our operators defined.
 *          
 * (C) 1999 by Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MATHD3D_H
#define _MATHD3D_H

// notice the striking similarity to D3DVERTEX
struct sVertex
{
	point3 loc;
	point3 norm;
	float u, v; // Texture coords

	sVertex(){}

	sVertex( const point3& inLoc, const point3& inNorm, float inU = 0, float inV = 0 ) :
			loc( inLoc ), norm( inNorm ), u( inU ), v( inV )
	{
	}

};

// and this fella could be mistaken for D3DLVERTEX
struct sLitVertex
{
	point3 loc;
	unsigned long reserved; // D3D needs this to be zero
	unsigned long diffuse; // The diffuse color of this vertex
	unsigned long specular; // the specular color
	float u, v; // Texture coords

	sLitVertex(){}

	sLitVertex( 
		const point3& inLoc, 
		unsigned long inDiff = 0, 
		unsigned long inSpec = 0, 
		float inU = 0, 
		float inV = 0) :
		loc( inLoc ), reserved(0), diffuse( inDiff ), specular( inSpec ),
		u( inU ), v( inV ) 
	{
	}

};

// D3DTLVERTEX?  no, this is his cousin.
struct sTLVertex
{
	point3 sLoc; // location in screenspace (hence the s)
	float rhw; // inverse w coordinate (from projection matrix mult)
	unsigned long diffuse; // The diffuse color of this vertex
	unsigned long specular; // the specular color

	float u, v; // Texture coordinates
};

#endif //_MATHD3D_H
