#ifndef SGX_CORE_MATH_VECTOR3_HPP
#define SGX_CORE_MATH_VECTOR3_HPP	1

class sgxVector2;
class sgxVector3;
class sgxVector4;
class sgxMatrix43;
class sgxMatrix44;

class sgxVector3 {
public:
	static sgxVector3 Null;
	static sgxVector3 Origin;
	static sgxVector3 UnitX;
	static sgxVector3 UnitY;
	static sgxVector3 UnitZ;

#if SGX_DEBUG
    sgxVector3() { x = y = z = sgxSNAN; }
#else
    sgxVector3() {}     // if a ctor is provided, then the default must be too
#endif
    sgxVector3(tREAL32 theta) { x=sgxSin(theta); y=-sgxCos(theta); z=0; }
    sgxVector3(tREAL32 vx, tREAL32 vy, tREAL32 vz) { x=vx; y=vy; z=vz; }
    sgxVector3(const sgxVector3 &v3) { *this = v3; }
    sgxVector3(const sgxVector4 &v4);

    //
	// Basic operator overloads
	//
	SGX_INLINE	sgxVector3	operator-(void) { return sgxVector3(-x, -y, -z); }
	SGX_INLINE	sgxVector3	operator+(const tREAL32 &n) const { return sgxVector3(x+n, y+n, z+n); }
	SGX_INLINE	sgxVector3	operator-(const tREAL32 &n) const { return sgxVector3(x-n, y-n, z-n); }
	SGX_INLINE	sgxVector3	operator*(const tREAL32 &n) const { return sgxVector3(x*n, y*n, z*n); }
	SGX_INLINE	sgxVector3	operator/(const tREAL32 &n) const { return sgxVector3(x/n, y/n, z/n); }
	SGX_INLINE	sgxVector3	operator+(const sgxVector3 &v) const { return sgxVector3(x+v.x, y+v.y, z+v.z); }
	SGX_INLINE	sgxVector3	operator-(const sgxVector3 &v) const { return sgxVector3(x-v.x, y-v.y, z-v.z); }
	SGX_INLINE	sgxVector3	operator*(const sgxVector3 &v) const { return sgxVector3(x*v.x, y*v.y, z*v.z); }
	SGX_INLINE	sgxVector3	operator/(const sgxVector3 &v) const { return sgxVector3(x/v.x, y/v.y, z/v.z); }
	SGX_INLINE	sgxVector3	operator+=(const tREAL32 &n) { x+=n; y+=n; z+=n; return *this; }
	SGX_INLINE	sgxVector3	operator-=(const tREAL32 &n) { x-=n; y-=n; z-=n; return *this; }
	SGX_INLINE	sgxVector3	operator*=(const tREAL32 &n) { x*=n; y*=n; z*=n; return *this; }
	SGX_INLINE	sgxVector3	operator/=(const tREAL32 &n) { x/=n; y/=n; z/=n; return *this; }
	SGX_INLINE	sgxVector3	operator+=(const sgxVector3 &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	SGX_INLINE	sgxVector3	operator-=(const sgxVector3 &v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	SGX_INLINE	sgxVector3	operator*=(const sgxVector3 &v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
	SGX_INLINE	sgxVector3	operator/=(const sgxVector3 &v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
	SGX_INLINE	bool		operator==(const sgxVector3 &r) { return (sgxEq(x,r.x)&&sgxEq(y,r.y)&&sgxEq(z,r.z))?TRUE:FALSE; }
	SGX_INLINE	bool		operator!=(const sgxVector3 &r) { return (sgxNeq(x,r.x)||sgxNeq(y,r.y)||sgxNeq(z,r.z))?TRUE:FALSE; }

	//
	// Traditional features
	//
	SGX_INLINE	void		Set(tREAL32 x_, tREAL32 y_, tREAL32 z_) { x=x_; y=y_; z=z_; }
	SGX_INLINE	tREAL32		GetDistanceSquared(const sgxVector3 &v) const { tREAL32 dx=x-v.x; tREAL32 dy=y-v.y; tREAL32 dz=z-v.z; return dx*dx+dy*dy+dz*dz; }
	SGX_INLINE	tREAL32		GetMagnitude() const { return sgxSqr(x*x+y*y+z*z); } 
	SGX_INLINE	tREAL32		GetDistance(const sgxVector3 &v) const { return sgxSqr(GetDistanceSquared(v)); } 
	SGX_INLINE	void		Normalise(void) { tREAL32 r = 1.0f/sgxSqr(x*x+y*y+z*z); (*this)*=r; }
				void		Transform(const sgxMatrix43 &m);
				void		Transform(const sgxMatrix44 &m);
			   
    tREAL32 x, y, z;
};

//
// V3 Utilities
//
SGX_INLINE tREAL32		sgxDot(const sgxVector3 &v1, const sgxVector3 &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
SGX_INLINE sgxVector3	sgxCross(const sgxVector3 &v1, const sgxVector3 &v2) { return sgxVector3(v1.y*v2.z-v1.z*v2.y, v1.z*v2.z-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x); }
SGX_INLINE sgxVector3	sgxLerp(const sgxVector3 &v1, const sgxVector3 &v2, tREAL32 t) { return v1+(v2-v1)*t; }
SGX_INLINE sgxVector3	sgxApproach(const sgxVector3 from, const sgxVector3 approaching, const tREAL32 max_delta=0.1f) { return sgxVector3(sgxApproach(from.x,approaching.x,max_delta), sgxApproach(from.y,approaching.y,max_delta), sgxApproach(from.z,approaching.z,max_delta)); }


#endif /* SGX_CORE_MATH_VECTOR3_HPP */
