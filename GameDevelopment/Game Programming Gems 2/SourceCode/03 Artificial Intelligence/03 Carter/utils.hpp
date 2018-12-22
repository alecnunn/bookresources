#ifndef __UTILS_H__
#define __UTILS_H__

class C2DCoordF
{
public:
	float X;
	float Y;

	C2DCoordF() {};
	C2DCoordF(float x, float y) : X(x), Y(y) {};
};


inline float Get2DDistance(const C2DCoordF& pos1, const C2DCoordF& pos2)
{
	float distx = pos1.X - pos2.X;
	float disty = pos1.Y - pos2.Y;

	return(sqrt(distx*distx + disty*disty));
}

#endif //__UTILS_H__