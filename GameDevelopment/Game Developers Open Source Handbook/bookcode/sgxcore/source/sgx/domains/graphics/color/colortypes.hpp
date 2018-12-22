#ifndef SGX_DOMAIN_GRAPHICS_COLORTYPES_HPP
#define SGX_DOMAIN_GRAPHICS_COLORTYPES_HPP		1

tUINT32  sgxGetRGB32(const tREAL32 r, const tREAL32 g, const tREAL32 b);
tUINT32  sgxGetRGBA32(const tREAL32 r, const tREAL32 g, const tREAL32 b, const tREAL32 a);

class sgxColorRGB;
class sgxColorHSV
{
public:
	sgxColorHSV();
	sgxColorHSV(const tREAL32 vh, const tREAL32 vs, const tREAL32 vv);
	sgxColorHSV(const tREAL32 vh, const tREAL32 vs, const tREAL32 vv, const tREAL32 va);

	sgxColorRGB rgb() const;

	tREAL32 h,s,v;
};

class sgxColorRGB
{
public:
	sgxColorRGB();
	sgxColorRGB(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb);
	sgxColorRGB(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb, const tREAL32 va);

	sgxColorHSV hsv() const;

	SGX_INLINE	bool	operator!=(const sgxColorRGB &rhs) const { return (r!=rhs.r || g!=rhs.g || b!=rhs.b)?TRUE:FALSE; }


				void	Darken(const tREAL32 pc=10);
				void	Lighten(const tREAL32 pc=10);

	static sgxColorRGB	White;
	static sgxColorRGB	LtGrey;
	static sgxColorRGB	DkGrey;
	static sgxColorRGB	Black;
	static sgxColorRGB	Red;
	static sgxColorRGB	Green;
	static sgxColorRGB	Blue;

	tREAL32 r,g,b;
};

class sgxColorRGBA
{
public:
	sgxColorRGBA();
	sgxColorRGBA(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb);
	sgxColorRGBA(const tREAL32 vr, const tREAL32 vg, const tREAL32 vb, const tREAL32 va);

	SGX_INLINE	bool operator!=(const sgxColorRGBA &rhs) const 
	{ 
		return (r!=rhs.r || g!=rhs.g || b!=rhs.b || a!=rhs.a)?TRUE:FALSE; 
	}

	static sgxColorRGBA	White;
	static sgxColorRGBA	LtGrey;
	static sgxColorRGBA	DkGrey;
	static sgxColorRGBA	Black;
	static sgxColorRGBA	Red;
	static sgxColorRGBA	Green;
	static sgxColorRGBA	Blue;

	tREAL32 r,g,b,a;
};

#endif /* SGX_DOMAIN_GRAPHICS_COLORTYPES_HPP */
