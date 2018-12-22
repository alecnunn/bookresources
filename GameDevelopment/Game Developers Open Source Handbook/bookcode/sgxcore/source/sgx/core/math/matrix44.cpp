#include "sgx/core/types/types.hpp"
#include "sgx/core/math/constants.hpp"
#include "sgx/core/math/numerics.hpp"
#include "sgx/core/math/trigonometry.hpp"
#include "sgx/core/math/vector3.hpp"
#include "sgx/core/math/vector4.hpp"
#include "sgx/core/math/matrix44.hpp"

sgxMatrix44 sgxMatrix44::Null(sgxVector4::Null, sgxVector4::Null, sgxVector4::Null,sgxVector4::Null);
sgxMatrix44 sgxMatrix44::Ident(sgxVector4::UnitX, sgxVector4::UnitY, sgxVector4::UnitZ, sgxVector4::UnitW);

void sgxMatrix44::SetRotateX(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right = sgxVector4::UnitX;
	up.x = 0; up.y = c; up.z = s; up.w = 0;
	at.x = 0; at.y = -s; at.z = c; at.w = 0;
	pos = sgxVector4::UnitW;
}

void sgxMatrix44::SetRotateY(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right.x = c; right.y = 0; right.z = -s; right.w = 0;
	up = sgxVector4::UnitY;
	at.x = s; at.y = 0; at.z =  c; at.w = 0;
	pos = sgxVector4::UnitW;
}

void sgxMatrix44::SetRotateZ(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right.x = c; right.y = s; right.z = 0; right.w = 0;
	up.x = -s; up.y = c; up.z = 0; up.w = 0;
	at = sgxVector4::UnitZ;
	pos = sgxVector4::UnitW;
}

sgxMatrix44 sgxMatrix44::operator*(const sgxMatrix44 &m)
{
	sgxMatrix44 out;

	out.right.x = right.x*m.right.x + right.y*m.up.x + right.z*m.at.x + right.w*m.pos.x;
	out.right.y = right.x*m.right.y + right.y*m.up.y + right.z*m.at.y + right.w*m.pos.y;
	out.right.z = right.x*m.right.z + right.y*m.up.z + right.z*m.at.z + right.w*m.pos.z;
	out.right.w = right.x*m.right.w + right.y*m.up.w + right.z*m.at.w + right.w*m.pos.w;

	out.up.x = up.x*m.right.x + up.y*m.up.x + up.z*m.at.x + up.w*m.pos.x;
	out.up.y = up.x*m.right.y + up.y*m.up.y + up.z*m.at.y + up.w*m.pos.y;
	out.up.z = up.x*m.right.z + up.y*m.up.z + up.z*m.at.z + up.w*m.pos.z;
	out.up.w = up.x*m.right.w + up.y*m.up.w + up.z*m.at.w + up.w*m.pos.w;

	out.at.x = at.x*m.right.x + at.y*m.up.x + at.z*m.at.x + at.w*m.pos.x;
	out.at.y = at.x*m.right.y + at.y*m.up.y + at.z*m.at.y + at.w*m.pos.y;
	out.at.z = at.x*m.right.z + at.y*m.up.z + at.z*m.at.z + at.w*m.pos.z;
	out.at.w = at.x*m.right.w + at.y*m.up.w + at.z*m.at.w + at.w*m.pos.w;

	out.pos.x = pos.x*m.right.x + pos.y*m.up.x + pos.z*m.at.x + pos.w*m.pos.x;
	out.pos.y = pos.x*m.right.y + pos.y*m.up.y + pos.z*m.at.y + pos.w*m.pos.y;
	out.pos.z = pos.x*m.right.z + pos.y*m.up.z + pos.z*m.at.z + pos.w*m.pos.z;
	out.pos.w = pos.x*m.right.w + pos.y*m.up.w + pos.z*m.at.w + pos.w*m.pos.w;

	return out;
}

