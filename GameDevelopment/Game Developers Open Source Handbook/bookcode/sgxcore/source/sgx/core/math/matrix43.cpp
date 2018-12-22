#include "sgx/core/math/math.hpp"

void sgxMatrix43::SetRotateX(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right = sgxVector3::UnitX;
	up.x = 0; up.y = c; up.z = s;
	at.x = 0; at.y = -s; at.z = c;
}

void sgxMatrix43::SetRotateY(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right.x = c; right.y = 0; right.z = -s; 
	up = sgxVector3::UnitY;
	at.x = s; at.y = 0; at.z =  c;
}

void sgxMatrix43::SetRotateZ(tREAL32 theta)
{
tREAL32 s=sgxSin(theta);
tREAL32 c=sgxCos(theta);

	right.x = c; right.y = s; right.z = 0;
	up.x = -s; up.y = c; up.z = 0;
	at = sgxVector3::UnitZ;
}

void
sgxMatrix43::SetRotateAxis(const tREAL32 theta, const sgxVector3 &axis)
{
tREAL32 ct = sgxCos(theta);
tREAL32 st = sgxSin(theta);
tREAL32 xx = axis.x * axis.x;
tREAL32 xy = axis.x * axis.y;
tREAL32 xz = axis.x * axis.z;
tREAL32 yy = axis.y * axis.y;
tREAL32 yz = axis.y * axis.z;
tREAL32 zz = axis.z * axis.z;

    /* R = uu' + cos(theta)*(I-uu') + sin(theta)*S
     *
     * S =  0  -z   y    u' = (x, y, z)
     *	    z   0  -x
     *	   -y   x   0
     */
	right.x = xx + ct*(1-xx);
	up.x = xy + ct*(0-xy) - st*axis.z;
	at.x = xz + ct*(0-xz) + st*axis.y;

	right.y = xy + ct*(0-xy) + st*axis.z;
	up.y = yy + ct*(1-yy);
	at.y = yz + ct*(0-yz) - st*axis.x;

	right.z = xz + ct*(0-xz) - st*axis.y;
	up.z = yz + ct*(0-yz) + st*axis.x;
	at.z = zz + ct*(1-zz);
}

sgxMatrix43 sgxMatrix43::operator*(const sgxMatrix43 &m)
{
	sgxMatrix43 out;

	out.right.x = right.x*m.right.x + right.y*m.up.x + right.z*m.at.x + 0;
	out.right.y = right.x*m.right.y + right.y*m.up.y + right.z*m.at.y + 0;
	out.right.z = right.x*m.right.z + right.y*m.up.z + right.z*m.at.z + 0;

	out.up.x = up.x*m.right.x + up.y*m.up.x + up.z*m.at.x;
	out.up.y = up.x*m.right.y + up.y*m.up.y + up.z*m.at.y;
	out.up.z = up.x*m.right.z + up.y*m.up.z + up.z*m.at.z;

	out.at.x = at.x*m.right.x + at.y*m.up.x + at.z*m.at.x;
	out.at.y = at.x*m.right.y + at.y*m.up.y + at.z*m.at.y;
	out.at.z = at.x*m.right.z + at.y*m.up.z + at.z*m.at.z;

	out.pos.x = pos.x*m.right.x + pos.y*m.up.x + pos.z*m.at.x + m.pos.x;
	out.pos.y = pos.x*m.right.y + pos.y*m.up.y + pos.z*m.at.y + m.pos.y;
	out.pos.z = pos.x*m.right.z + pos.y*m.up.z + pos.z*m.at.z + m.pos.z;

	return out;
}

