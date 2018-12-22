#include "sgx/core/types/types.hpp"
#include "sgx/core/sgxlib/sgxlib.hpp"
#include "sgx/core/math/math.hpp"
#include "sgx/domains/graphics/color/colortypes.hpp"

//
// Color conversion code
//
sgxColorHSV sgxColorRGB::hsv() const 
{
	tREAL32 rgb_max = sgxMax(r,g,b);
	tREAL32 rgb_min = sgxMin(r,g,b);
	tREAL32 delta = rgb_max - rgb_min;
	sgxColorHSV col;

	if (rgb_max == 0) {
		return sgxColorHSV(0, 0, 0); // v is technically undefined
	}

	col.v = rgb_max;
	col.s = delta / rgb_max;

	if (r == rgb_max) {				// red is most prominent
		col.h = (g - b) / delta;
	} else if (g == rgb_max) {		// green is most prominent
		col.h = 2+(b - r) / delta;
	} else if (b == rgb_max) {		// blue is most prominent
		col.h = 4+(r - g) / delta;
	}

	col.h /= 6.0f;		// scale back to 0-1 range
	if (col.h < 0.0f) {
		col.h = 1.0f;
	}


	return col;
}
 
sgxColorRGB sgxColorHSV::rgb() const 
{
	// Greys are an easy-out case
	if (s == 0) {
		return sgxColorRGB(v,v,v);
	}

	tREAL32 sector = v*6.0f;		// rescale
	tINT32	sector_int = sgxFloor(sector);
	tREAL32 sector_fraction = sector - tREAL32(sector_int);

	tREAL32 p,q,t;

	p = v * (1.0f - s);
	q = v * (1.0f - s*sector_fraction);
	t = v * (1.0f - s*(1.0f-sector_fraction));

	switch(sector_int) {
	case	1:	
		return sgxColorRGB(q,v,p);
	case	2:	
		return sgxColorRGB(p,v,t);
	case	3:	
		return sgxColorRGB(p,q,v);
	case	4:	
		return sgxColorRGB(t,p,v);
	case	5:	
		return sgxColorRGB(v,p,q);
	default:	
		return sgxColorRGB(v,t,p);
	}
}

