#ifndef SGX_CORE_MATH_MATRIX44_HPP
#define SGX_CORE_MATH_MATRIX44_HPP	1

class sgxVector2;
class sgxVector3;
class sgxVector4;
class sgxMatrix43;
class sgxMatrix44;

class sgxMatrix44 {
public:
	static sgxMatrix44 Null;
	static sgxMatrix44 Ident;

	sgxVector4	right;
	sgxVector4	up;
	sgxVector4	at;
	sgxVector4	pos;

	SGX_INLINE	sgxMatrix44(){}

	SGX_INLINE	sgxMatrix44(const sgxVector4 &r, const sgxVector4 &u, const sgxVector4 &a, const sgxVector4 &p)
	{ 
		right=r;
		up=u;
		at=a;
		pos=p;
	}

	SGX_INLINE	sgxMatrix44(tREAL32 x, tREAL32 y, tREAL32 z, tREAL32 w=1)
	{ 
		right = sgxVector4::UnitX; 
		up    = sgxVector4::UnitY;
		at    = sgxVector4::UnitZ; 
		pos   = sgxVector4(x,y,z,w); 
	}

	sgxMatrix44 operator*(const sgxMatrix44 &m);

	SGX_INLINE	void Identity() 
	{ 
		right = sgxVector4(1,0,0,0); 
		up    = sgxVector4(0,1,0,0);
		at    = sgxVector4(0,0,1,0); 
		pos   = sgxVector4(0,0,0,1); 
	}

	SGX_INLINE	void	SetPos(const sgxVector3 &vpos) { pos = sgxVector3(vpos); }
	SGX_INLINE	void	SetPos(const sgxVector4 &vpos) { pos = vpos; }

				void	SetRotateX(tREAL32 theta);
				void	SetRotateY(tREAL32 theta);
				void	SetRotateZ(tREAL32 theta);

};

#endif /* SGX_CORE_MATH_MATRIX44_HPP */
