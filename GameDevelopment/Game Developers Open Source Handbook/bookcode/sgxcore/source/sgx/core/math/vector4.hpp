#ifndef SGX_CORE_MATH_VECTOR4_HPP
#define SGX_CORE_MATH_VECTOR4_HPP	1

class sgxVector2;
class sgxVector3;
class sgxVector4;
class sgxMatrix43;
class sgxMatrix44;

class sgxVector4 {
public:
	static sgxVector4 Null;
	static sgxVector4 Origin;
	static sgxVector4 UnitX;
	static sgxVector4 UnitY;
	static sgxVector4 UnitZ;
	static sgxVector4 UnitW;

#if SGX_DEBUG
    sgxVector4() { x = y = z = w = sgxSNAN; }
#else
    sgxVector4() {}     // if a ctor is provided, then the default must be too
#endif
    sgxVector4(const sgxVector3 &v3) { x=v3.x; y=v3.y; z=v3.z; w=1; }
    sgxVector4(const sgxVector4 &v4) { *this = v4; }
    sgxVector4(tREAL32 vx, tREAL32 vy, tREAL32 vz) { x=vx; y=vy; z=vz; w=1; }
	sgxVector4(tREAL32 vx, tREAL32 vy, tREAL32 vz, tREAL32 vw) { x=vx; y=vy; z=vz; w=vw; }

    //
	// Basic operator overloads
	//
	SGX_INLINE sgxVector4 operator-(void) { return sgxVector4(-x, -y, -z, -w); }
	SGX_INLINE sgxVector4 operator+(const tREAL32 &n) { return sgxVector4(x+n, y+n, z+n, w+n); }
	SGX_INLINE sgxVector4 operator-(const tREAL32 &n) { return sgxVector4(x-n, y-n, z-n, w-n); }
	SGX_INLINE sgxVector4 operator*(const tREAL32 &n) { return sgxVector4(x*n, y*n, z*n, w*n); }
	SGX_INLINE sgxVector4 operator/(const tREAL32 &n) { return sgxVector4(x/n, y/n, z/n, w/n); }
	SGX_INLINE sgxVector4 operator+(const sgxVector4 &v) { return sgxVector4(x+v.x, y+v.y, z+v.z, w+v.w); }
	SGX_INLINE sgxVector4 operator-(const sgxVector4 &v) { return sgxVector4(x-v.x, y-v.y, z-v.z, w-v.w); }
	SGX_INLINE sgxVector4 operator*(const sgxVector4 &v) { return sgxVector4(x*v.x, y*v.y, z*v.z, w*v.w); }
	SGX_INLINE sgxVector4 operator/(const sgxVector4 &v) { return sgxVector4(x/v.x, y/v.y, z/v.z, w/v.w); }
	SGX_INLINE sgxVector4 operator+=(const tREAL32 &n) { x+=n; y+=n; z+=n; w+=n; return *this; }
	SGX_INLINE sgxVector4 operator-=(const tREAL32 &n) { x-=n; y-=n; z-=n; w-=n; return *this; }
	SGX_INLINE sgxVector4 operator*=(const tREAL32 &n) { x*=n; y*=n; z*=n; w*=n; return *this; }
	SGX_INLINE sgxVector4 operator/=(const tREAL32 &n) { x/=n; y/=n; z/=n; w/=n; return *this; }
	SGX_INLINE sgxVector4 operator+=(const sgxVector4 &v) { x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
	SGX_INLINE sgxVector4 operator-=(const sgxVector4 &v) { x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
	SGX_INLINE sgxVector4 operator*=(const sgxVector4 &v) { x*=v.x; y*=v.y; z*=v.z; w*=v.w; return *this; }
	SGX_INLINE sgxVector4 operator/=(const sgxVector4 &v) { x/=v.x; y/=v.y; z/=v.z; w/=v.w; return *this; }
	SGX_INLINE bool		  operator==(const sgxVector4 &r) { return (sgxEq(x,r.x)&&sgxEq(y,r.y)&&sgxEq(z,r.z)&&sgxEq(w,r.w))?TRUE:FALSE; }
	SGX_INLINE bool		  operator!=(const sgxVector4 &r) { return (sgxNeq(x,r.x)||sgxNeq(y,r.y)||sgxNeq(z,r.z)||sgxNeq(w,r.w))?TRUE:FALSE; }

	//
	// Traditional features
	//
	SGX_INLINE	void	Reduce(void)    { tREAL32 r = 1.0f/w; (*this)*=r; }
	SGX_INLINE	void	Normalise(void) { tREAL32 r = 1.0f/sgxSqr(x*x+y*y+z*z+w*w); (*this)*=r; }
				void	Transform(const sgxMatrix44 &m);

    tREAL32 x, y, z, w;
};

#endif /* SGX_CORE_MATH_VECTOR4_HPP */
