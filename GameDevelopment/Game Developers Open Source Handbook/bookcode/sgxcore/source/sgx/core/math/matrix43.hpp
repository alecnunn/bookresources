#ifndef SGX_CORE_MATH_MATRIX43_HPP
#define SGX_CORE_MATH_MATRIX43_HPP	1

class sgxVector2;
class sgxVector3;
class sgxVector4;
class sgxMatrix43;
class sgxMatrix44;

class sgxMatrix43 {
public:
	sgxVector3	right;
	sgxVector3	up;
	sgxVector3	at;
	sgxVector3	pos;

	SGX_INLINE	sgxMatrix43(){}

	SGX_INLINE	sgxMatrix43(const sgxVector3 &r, const sgxVector3 &u, const sgxVector3 &a, const sgxVector3 &p)
	{ 
		right=r;
		up=u;
		at=a;
		pos=p;
	}

	SGX_INLINE	sgxMatrix43(const tREAL32 x, const tREAL32 y, const tREAL32 z)
	{ 
		right=sgxVector3(1,0,0); 
		up=sgxVector3(0,1,0);
		at=sgxVector3(0,0,1); 
		pos=sgxVector3(x,y,z); 
	}

	SGX_INLINE	sgxMatrix43(const sgxVector3 &_pos)
	{ 
		right=sgxVector3(1,0,0); 
		up=sgxVector3(0,1,0);
		at=sgxVector3(0,0,1); 
		pos=_pos;
	}
	
	sgxMatrix43 operator*(const sgxMatrix43 &m);

	SGX_INLINE	void Identity() { right=sgxVector3(1,0,0); up=sgxVector3(0,1,0);
							at=sgxVector3(0,0,1); pos=sgxVector3(0,0,0); }

	SGX_INLINE	void SetPos(const sgxVector3 &vpos) { pos = vpos; }
	SGX_INLINE	void SetPos(const sgxVector4 &vpos) { pos = sgxVector4(vpos); }
	SGX_INLINE	void SetPosition(const sgxVector3 &vpos) { pos = vpos; }
	SGX_INLINE	void SetPosition(const sgxVector4 &vpos) { pos = sgxVector4(vpos); }

				void SetRotateAxis(const tREAL32 theta, const sgxVector3 &axis);
				void SetRotateX(const tREAL32 theta);
				void SetRotateY(const tREAL32 theta);
				void SetRotateZ(const tREAL32 theta);

	SGX_INLINE  void Scale(tREAL32 scale) { right.x *= scale; up.y *= scale; at.z *= scale; }
	SGX_INLINE  void Scale(tREAL32 sx, tREAL32 sy, tREAL32 sz) { right.x *= sx; up.y *= sy; at.z *= sz; }

};

#endif /* SGX_CORE_MATH_MATRIX43_HPP */
