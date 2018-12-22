#include "sgx/core/types/types.hpp"
#include "sgx/core/sgxlib/sgxlib.hpp"
#include "sgx/domains/graphics/color/colortypes.hpp"


//
// RGB (Red, Green, Blue)
//
tUINT32 
sgxGetRGB32(const tREAL32 r, const tREAL32 g, const tREAL32 b) 
{
tUINT32 col;

	col = sgxRange<tUINT32>(tUINT32(r*255), 0, 255) << 16;
	col|= sgxRange<tUINT32>(tUINT32(g*255), 0, 255) <<  8;
	col|= sgxRange<tUINT32>(tUINT32(b*255), 0, 255) <<  0;

	return col;
}

void	
sgxColorRGB::Darken(const tREAL32 pc)
{
	Lighten(-pc);
}

void	
sgxColorRGB::Lighten(const tREAL32 pc)
{
	r += pc * 0.01f;
	g += pc * 0.01f;
	b += pc * 0.01f;

	r = sgxRange<tREAL32>(r, 0, 1);
	g = sgxRange<tREAL32>(g, 0, 1);
	b = sgxRange<tREAL32>(b, 0, 1);
}

sgxColorRGB::sgxColorRGB()
{
	r=g=b=1.0f; 
}

sgxColorRGB::sgxColorRGB(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb)
{ 
	r=sgxRange<tREAL32>(vr,0,1); 
	g=sgxRange<tREAL32>(vg,0,1); 
	b=sgxRange<tREAL32>(vb,0,1); 
}

sgxColorRGB::sgxColorRGB(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb, const tREAL32 va) 
{ 
	r=sgxRange<tREAL32>(vr,0,1); 
	g=sgxRange<tREAL32>(vg,0,1); 
	b=sgxRange<tREAL32>(vb,0,1); 
}

//
// RGBA (Red, Green, Blue, Alpha)
//
tUINT32 
sgxGetRGBA32(const tREAL32 r, const tREAL32 g, const tREAL32 b, const tREAL32 a)
{
tUINT32 col;

	col = sgxRange<tUINT32>(tUINT32(r*255), 0, 255) << 24;
	col|= sgxRange<tUINT32>(tUINT32(g*255), 0, 255) << 16;
	col|= sgxRange<tUINT32>(tUINT32(b*255), 0, 255) <<  8;
	col|= sgxRange<tUINT32>(tUINT32(a*255), 0, 255) <<  0;

	return col;
}

sgxColorRGBA::sgxColorRGBA() 
{ 
	r=g=b=1.0f; 
	a=1.0f; 
}

sgxColorRGBA::sgxColorRGBA(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb)
{
	r=sgxRange<tREAL32>(vr,0,1); 
	g=sgxRange<tREAL32>(vg,0,1); 
	b=sgxRange<tREAL32>(vb,0,1); 
	a=1; 
}

sgxColorRGBA::sgxColorRGBA(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb, const tREAL32 va) 
{ 
	r=sgxRange<tREAL32>(vr,0,1); 
	g=sgxRange<tREAL32>(vg,0,1); 
	b=sgxRange<tREAL32>(vb,0,1); 
	a=sgxRange<tREAL32>(va,0,1); 
}

//
// HSV (Hue, Saturation, Value)
//
sgxColorHSV::sgxColorHSV()
{
	h=s=v=1.0f; 
}

sgxColorHSV::sgxColorHSV(const tREAL32 vh, const tREAL32 vs, const tREAL32 vv)
{ 
	h=sgxRange<tREAL32>(vh,0,1); 
	s=sgxRange<tREAL32>(vs,0,1); 
	v=sgxRange<tREAL32>(vv,0,1); 
}

sgxColorHSV::sgxColorHSV(const tREAL32 vh, const tREAL32 vs, const tREAL32 vv, const tREAL32 va) 
{ 
	h=sgxRange<tREAL32>(vh,0,1); 
	s=sgxRange<tREAL32>(vs,0,1); 
	v=sgxRange<tREAL32>(vv,0,1); 
}
