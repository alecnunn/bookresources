#include "sgx/core/math/math.hpp"

sgxVector4 sgxVector4::Null(0,0,0,0);
sgxVector4 sgxVector4::Origin(0,0,0,0);
sgxVector4 sgxVector4::UnitX(1,0,0,0);
sgxVector4 sgxVector4::UnitY(0,1,0,0);
sgxVector4 sgxVector4::UnitZ(0,0,1,0);
sgxVector4 sgxVector4::UnitW(0,0,0,1);

void sgxVector4::Transform(const sgxMatrix44 &m)
{
	sgxVector4 v = *this;

	x = v.x*m.right.x + v.y*m.up.x + v.z*m.at.x + v.w*m.pos.x;
	y = v.x*m.right.y + v.y*m.up.y + v.z*m.at.y + v.w*m.pos.y;
	z = v.x*m.right.z + v.y*m.up.z + v.z*m.at.z + v.w*m.pos.z;
	w = v.x*m.right.w + v.y*m.up.w + v.z*m.at.w + v.w*m.pos.w;
}

