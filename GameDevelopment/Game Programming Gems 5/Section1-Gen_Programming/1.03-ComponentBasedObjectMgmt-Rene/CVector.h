#ifndef __CVECTOR_H
#define __CVECTOR_H

class CVector
{
public:
	CVector()	{}
	CVector(float x, float y, float z) : mX(x), mY(y), mZ(z)		{}

	float GetX() const { return mX; }
	float GetY() const { return mY; }
	float GetZ() const { return mZ; }
	void SetX(float x) { mX = x; }
	void SetY(float y) { mY = y; }
	void SetZ(float z) { mZ = z; }
private:
	float mX, mY, mZ;
};

#endif //__CVECTOR_H