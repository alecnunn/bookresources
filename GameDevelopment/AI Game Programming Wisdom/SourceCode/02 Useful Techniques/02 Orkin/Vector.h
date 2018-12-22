#ifndef __VECTOR_H__
#define __VECTOR_H__


struct Vector
{
	Vector() {}
	Vector(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	float x; 
	float y;
	float z;
};


#endif
