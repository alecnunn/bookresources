#ifndef SGX_CORE_MATH_VECTOR2_HPP
#define SGX_CORE_MATH_VECTOR2_HPP	1

class sgxVector2;
class sgxVector3;
class sgxVector4;
class sgxMatrix43;
class sgxMatrix44;

class sgxVector2 {
public:
	static sgxVector2 Null;
	static sgxVector2 Origin;
	static sgxVector2 UnitX;
	static sgxVector2 UnitY;

#if SGX_DEBUG
    sgxVector2() { x = y = sgxSNAN; }
#else		
	sgxVector2() {}
#endif
	sgxVector2(tREAL32 vx, tREAL32 vy) { x=vx; y=vy; }

    //
	// Basic operator overloads
	//
	SGX_INLINE	sgxVector2	operator-(void) { return sgxVector2(-x, -y); }
	SGX_INLINE	sgxVector2	operator+(const tREAL32 &n) const { return sgxVector2(x+n, y+n); }
	SGX_INLINE	sgxVector2	operator-(const tREAL32 &n) const { return sgxVector2(x-n, y-n); }
	SGX_INLINE	sgxVector2	operator*(const tREAL32 &n) const { return sgxVector2(x*n, y*n); }
	SGX_INLINE	sgxVector2	operator/(const tREAL32 &n) const { return sgxVector2(x/n, y/n); }
	SGX_INLINE	sgxVector2	operator+(const sgxVector2 &v) const { return sgxVector2(x+v.x, y+v.y); }
	SGX_INLINE	sgxVector2	operator-(const sgxVector2 &v) const { return sgxVector2(x-v.x, y-v.y); }
	SGX_INLINE	sgxVector2	operator*(const sgxVector2 &v) const { return sgxVector2(x*v.x, y*v.y); }
	SGX_INLINE	sgxVector2	operator/(const sgxVector2 &v) const { return sgxVector2(x/v.x, y/v.y); }
	SGX_INLINE	sgxVector2	operator+=(const tREAL32 &n) { x+=n; y+=n; return *this; }
	SGX_INLINE	sgxVector2	operator-=(const tREAL32 &n) { x-=n; y-=n; return *this; }
	SGX_INLINE	sgxVector2	operator*=(const tREAL32 &n) { x*=n; y*=n; return *this; }
	SGX_INLINE	sgxVector2	operator/=(const tREAL32 &n) { x/=n; y/=n; return *this; }
	SGX_INLINE	sgxVector2	operator+=(const sgxVector2 &v) { x+=v.x; y+=v.y; return *this; }
	SGX_INLINE	sgxVector2	operator-=(const sgxVector2 &v) { x-=v.x; y-=v.y; return *this; }
	SGX_INLINE	sgxVector2	operator*=(const sgxVector2 &v) { x*=v.x; y*=v.y; return *this; }
	SGX_INLINE	sgxVector2	operator/=(const sgxVector2 &v) { x/=v.x; y/=v.y; return *this; }
	SGX_INLINE	bool		operator==(const sgxVector2 &r) { return (sgxEq(x,r.x)&&sgxEq(y,r.y))?TRUE:FALSE; }
	SGX_INLINE	bool		operator!=(const sgxVector2 &r) { return (sgxNeq(x,r.x)||sgxNeq(y,r.y))?TRUE:FALSE; }

	//
	// Traditional features
	//
	SGX_INLINE	void		Set(tREAL32 x_, tREAL32 y_) { x=x_; y=y_;  }
	SGX_INLINE	tREAL32		GetDistanceSquared(const sgxVector2 &v) const { tREAL32 dx=x-v.x; tREAL32 dy=y-v.y; return dx*dx+dy*dy; }
	SGX_INLINE	tREAL32		GetMagnitude() const { return sgxSqr(x*x+y*y); } 
	SGX_INLINE	tREAL32		GetDistance(const sgxVector2 &v) const { return sgxSqr(GetDistanceSquared(v)); } 
	SGX_INLINE	void		Normalise(void) { tREAL32 r = 1.0f/sgxSqr(x*x+y*y); (*this)*=r; }
				void		Rotate(const sgxVector2 &centre, const tREAL32 theta);


	tREAL32 x, y;
};

SGX_INLINE tREAL32		sgxDot(const sgxVector2 &v1, const sgxVector2 &v2) { return v1.x*v2.x + v1.y*v2.y; }
SGX_INLINE sgxVector2	sgxLerp(const sgxVector2 &v1, const sgxVector2 &v2, tREAL32 t) { return v1+(v2-v1)*t; }

#endif /* SGX_CORE_MATH_VECTOR2_HPP */
