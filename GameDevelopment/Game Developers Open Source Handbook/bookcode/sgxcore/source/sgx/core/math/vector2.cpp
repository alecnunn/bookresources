#include "sgx/core/math/math.hpp"

sgxVector2 sgxVector2::Null(0,0);
sgxVector2 sgxVector2::Origin(0,0);
sgxVector2 sgxVector2::UnitX(1,0);
sgxVector2 sgxVector2::UnitY(0,1);

void
sgxVector2::Rotate(const sgxVector2 &centre, const tREAL32 theta)
{
	if (theta) {
		tREAL32 length = GetDistance(centre);
		tREAL32 dx = x - centre.x;
		tREAL32 dy = y - centre.y;
		
		tREAL32 angle = sgxAtan2(dx, dy) - sgxPI2;

		angle += theta;

		x = sgxCos(angle) * length + centre.x;
		y = -sgxSin(angle) * length + centre.y;
	}
}

