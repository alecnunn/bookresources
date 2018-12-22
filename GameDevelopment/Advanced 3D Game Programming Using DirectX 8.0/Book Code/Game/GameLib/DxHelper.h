/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: DDHelper.h
 *    Desc: DirectDraw (and Direct3D) helper classes
 *          constructurs take care of a lot of grunt work
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _DDHELPER_H
#define _DDHELPER_H

#include <memory.h>
#include <d3d8.h>

#include "..\math3d\color4.h"
#include "..\math3d\point3.h"

#define D3DLIGHT_RANGE_MAX      ((float)sqrt(FLT_MAX))
/**
 * This class takes care of the annoying gruntwork
 * of having to zero-out and set the size parameter
 * of our Windows and DirectX structures.
 */
template <class T>
struct sAutoZero : public T
{
	sAutoZero()
	{
		memset( this, 0, sizeof(T) );
		dwSize = sizeof(T);
	}
};


/**
 * The Right Way to release our COM interfaces.
 * If they're still valid, release them, then
 * invalidate them.
 */
template <class T>
inline void SafeRelease( T& iface )
{
	if( iface )
	{
		iface->Release();
		iface = NULL;
	}
}


/**
 * Make sure the release brings our refcount to 0.
 */
template <class T>
inline void ExtraSafeRelease( T& iface )
{
	long refCount = 0;

	if( iface )
	{
		if( 0 < ( refCount = iface->Release() ) )
		{
			DP0("[ExtraSafeRelease]: ERROR: Interface was not released enough!");
		}
		iface = NULL;
	}
}


#define SetColorStage( stage, arg1, arg2, op ) \
{\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_COLORARG1, arg1 );\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_COLORARG2, arg2 );\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_COLOROP, op );\
}

#define SetAlphaStage( stage, arg1, arg2, op ) \
{\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_ALPHAARG1, arg1 );\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_ALPHAARG2, arg2 );\
	Graphics()->GetDevice()->SetTextureStageState( stage, D3DTSS_ALPHAOP, op );\
}






/**
 * sLight helps the D3DLIGHT8 structure.
 * The static constructors make it possible to
 * automatically create certain light types
 */
struct sLight : public D3DLIGHT8
{
	sLight()
	{
		// do nothing
	}

	static sLight Directional( 
		const point3& dir, 
		const color3& diff = color3::White, 
		const color3& spec = color3::Black, 
		const color3& amb = color3::Black )
	{
		sLight out;
		memset(out, 0, sizeof(D3DLIGHT8));
		out.Type = D3DLIGHT_DIRECTIONAL;

		out.Direction = *(D3DVECTOR*)&dir;

		out.Diffuse = *(D3DCOLORVALUE*)&diff;
		out.Specular = *(D3DCOLORVALUE*)&spec;
		out.Ambient = *(D3DCOLORVALUE*)&amb;
		return out;
	}

	static sLight PointSource( 
		const point3& loc, 
		const color3& diff = color3::White, 
		const color3& spec = color3::Black, 
		const color3& amb = color3::Black )
	{
		sLight out;
		memset(out, 0, sizeof(D3DLIGHT8));
		out.Type = D3DLIGHT_POINT;

		out.Range = D3DLIGHT_RANGE_MAX;

		out.Attenuation0 = 0.f;
		out.Attenuation1 = 1.f;
		out.Attenuation2 = 0.f;

		out.Position = *(D3DVECTOR*)&loc;

		out.Diffuse = *(D3DCOLORVALUE*)&diff;
		out.Specular = *(D3DCOLORVALUE*)&spec;
		out.Ambient = *(D3DCOLORVALUE*)&amb;
		return out;
	}

	static sLight Spot( 
		const point3& loc,
		const point3& dir, 
		float theta, float phi,
		const color3& diff = color3::White, 
		const color3& spec = color3::Black, 
		const color3& amb = color3::Black )
	{
		sLight out;
		memset(out, 0, sizeof(D3DLIGHT8));
		out.Type = D3DLIGHT_SPOT;

		out.Range = D3DLIGHT_RANGE_MAX;

		out.Attenuation0 = 0.f;
		out.Attenuation1 = 1.f;
		out.Attenuation2 = 0.f;

		out.Theta = theta;
		out.Phi = phi;
		out.Position = *(D3DVECTOR*)&loc;
		out.Direction = *(D3DVECTOR*)&dir;

		out.Diffuse = *(D3DCOLORVALUE*)&diff;
		out.Specular = *(D3DCOLORVALUE*)&spec;
		out.Ambient = *(D3DCOLORVALUE*)&amb;
		return out;
	}

	operator D3DLIGHT8*()
	{
		return this;
	}
	operator const D3DLIGHT8*() const
	{
		return (const D3DLIGHT8* )this;
	}
};



/**
 * sMaterial wraps the D3DMATERIAL8 structure,
 * providing a nice automatic constructor for the 
 * object.
 */
struct sMaterial : public D3DMATERIAL8
{

	sMaterial( 
		float pow,
		color4 diff = color4( 1.f, 1.f, 1.f, 1.f ), 
		color4 spec = color4( 1.f, 1.f, 1.f, 1.f ),
		color4 amb = color4( 1.f, 1.f, 1.f, 1.f ), 
		color4 emit = color4( 0.f, 0.f, 0.f, 0.f ) )
	{
	    ZeroMemory( this, sizeof(D3DMATERIAL8) );

		/**
		 * We could make an operator for this, but
		 * putting d3d.h everywhere kills compile time.
		 */
		Diffuse = *(D3DCOLORVALUE*)&diff;
		Ambient = *(D3DCOLORVALUE*)&amb;
		Specular = *(D3DCOLORVALUE*)&spec;
		Emissive = *(D3DCOLORVALUE*)&emit;
		Power = pow;
	}

	operator D3DMATERIAL8*()
	{
		return this;
	}
	operator const D3DMATERIAL8*() const
	{
		return (const D3DMATERIAL8* )this;
	}
};



#endif // _DDHELPER_H