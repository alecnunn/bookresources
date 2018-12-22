#ifndef SGX_CORE_MATH_GEOMETRY_HPP
#define SGX_CORE_MATH_GEOMETRY_HPP	1

class sgxDimension2 {
public:
	tREAL32		width, height;

	sgxDimension2(tREAL32 w, tREAL32 h) { width = w; height = h; }
};

class sgxDimension3 {
public:
	tREAL32		width, height, depth;

	sgxDimension3(tREAL32 w, tREAL32 h, tREAL32 d) { width = w; height = h; depth = d; }
};

class sgxPoint {
public:
	tREAL32		x, y;
};

class sgxRect {
public:
	tUINT32		left, right;
	tUINT32		top, bottom;
};

class sgxBounding {
public:
	virtual	void	Update() {}
};

class sgxBoundingBox : public sgxBounding {
public:
	sgxVector3	min;
	sgxVector3	max;
#if 0
	void Update(const sgxVector3 &centre, const tREAL32 radius);	// with sphere
	void Update(const sgxVector3 &min, const sgxVector3 &max);	// with bbox
	void Update(const sgxMesh &mesh);	// with mesh
#endif
};

class sgxBoundingSphere : public sgxBounding {
public:
	sgxVector3	centre;
	tREAL32		radius;
};

class sgxPlane
{
public:
	sgxVector3	m_Normal;
	tREAL32		D;
	tREAL32		m_Magnitude;

	SGX_INLINE sgxPlane(const sgxVector3 &normal, const tREAL32 d)
	{
		m_Magnitude = normal.GetMagnitude();
		m_Magnitude = 1.0f/m_Magnitude;

		m_Normal = normal * m_Magnitude;
		D = d * m_Magnitude;
	}


	SGX_INLINE sgxPlane(const sgxVector3 &v1, const sgxVector3 &v2, const sgxVector3 &v3)
	{
		sgxVector3 cpv1 = v2 - v1;
		sgxVector3 cpv2 = v3 - v1;

		// The plane eq of Ax+By+Cz+D = 0 is held in normal.x .y and .z
		m_Normal = sgxCross(cpv1, cpv2);

		// Compute D using any of the vectors
		D = -(m_Normal.x*v1.x + m_Normal.y*v1.y + m_Normal.z*v1.z);
		
		// Pre-calculate this for simpler distance checks
		m_Magnitude = sgxSqr(m_Normal.x*m_Normal.x + m_Normal.y*m_Normal.y + m_Normal.z*m_Normal.z);
		
		// Take reciprocal early so we never divide
		m_Magnitude = 1.0f/m_Magnitude;	
	}

	SGX_INLINE tREAL32	GetX(const sgxVector3 &v) const
	{
		return -(m_Normal.y*v.y + m_Normal.z*v.z + D)/m_Normal.x;
	}

	SGX_INLINE tREAL32	GetY(const sgxVector3 &v) const
	{
		return -(m_Normal.x*v.x + m_Normal.z*v.z + D)/m_Normal.y;
	}

	SGX_INLINE tREAL32	GetZ(const sgxVector3 &v) const
	{
		return -(m_Normal.x*v.x + m_Normal.y*v.y + D)/m_Normal.z;
	}

	SGX_INLINE tREAL32	Distance(const sgxVector3 &v) const
	{
		return (m_Normal.x*v.x + m_Normal.y*v.y + m_Normal.z*v.z + D) * m_Magnitude;
	}

	sgxPlane() { }
};

#endif /* SGX_CORE_MATH_GEOMETRY_HPP */
