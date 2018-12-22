#include "sgx/core/math/math.hpp"

sgxVector3 sgxVector3::Null(0,0,0);
sgxVector3 sgxVector3::Origin(0,0,0);
sgxVector3 sgxVector3::UnitX(1,0,0);
sgxVector3 sgxVector3::UnitY(0,1,0);
sgxVector3 sgxVector3::UnitZ(0,0,1);

sgxVector3::sgxVector3(const sgxVector4 &v4) { x=v4.x; y=v4.y; z=v4.z; }

void sgxVector3::Transform(const sgxMatrix43 &m)
{
	sgxVector3 v = *this;

	x = v.x*m.right.x + v.y*m.up.x + v.z*m.at.x + m.pos.x;
	y = v.x*m.right.y + v.y*m.up.y + v.z*m.at.y + m.pos.y;
	z = v.x*m.right.z + v.y*m.up.z + v.z*m.at.z + m.pos.z;
}

void sgxVector3::Transform(const sgxMatrix44 &m)
{
	sgxVector3 v = *this;

	// NOTE: Purists will be annoyed by this since you can't
	// multiple a 3x1 vector by a 4x4 matrix. What we do is pretend
	// the vector is a 4x1 with w=1
	x = v.x*m.right.x + v.y*m.up.x + v.z*m.at.x + m.pos.x;
	y = v.x*m.right.y + v.y*m.up.y + v.z*m.at.y + m.pos.y;
	z = v.x*m.right.z + v.y*m.up.z + v.z*m.at.z + m.pos.z;
}

