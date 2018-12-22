#ifndef AI_MATH_H
#define AI_MATH_H

class Vector3
{
	public:
		Vector3(){x=0.f;y=0.f;z=0.f;}

		void operator=(const Vector3& v);
		Vector3& operator-();
		Vector3& operator-(const Vector3& v);
		Vector3& operator+(const Vector3& v);
		void operator+=(const Vector3& v);
		Vector3& operator*(float scalar);
		float operator^(Vector3& v);	// Do a dot product

		void Normalize();
		void Lerp(float percent,Vector3& v1,Vector3& v2);
		void Set(float X,float Y,float Z);
		void Zero(){x=y=z=0.f;}

		float Angle(Vector3& Axis);	// Return the angle, based an the axis.
		
	public:
		float x;
		float y;
		float z;
};

class Matrix34
{
	public:
		Matrix34(){};

		Vector3 XDir;
		Vector3 YDir;
		Vector3 ZDir;
		Vector3 Pos;
};

inline float aiDot(const Vector3& A,const Vector3& B){return A.x*B.x+A.z*B.z;}
inline float aiDist(const Vector3& A,const Vector3& B){return (float)sqrt((A.x-B.x)*(A.x-B.x)+(A.z-B.z)*(A.z-B.z));}
inline float aiDist2(const Vector3& A,const Vector3& B){return ((A.x-B.x)*(A.x-B.x)+(A.z-B.z)*(A.z-B.z));}
inline float aiClamp(float fValue,float min,float max){if(fValue<min)fValue=min;if(fValue>max)fValue=max;}
inline float atan2f(float x,float y);

void Vector3::operator=(const Vector3& v)
{
	x=v.x;
	y=v.y;
	z=v.z;
}

Vector3& Vector3::operator*(float scalar)
{
	Vector3 V;
	V.x=scalar*x;
	V.y=scalar*y;
	V.z=scalar*z;
	return V;
}

Vector3& Vector3::operator+(const Vector3& v)
{
	Vector3 V;
	V.x=x+v.x;
	V.y=y+v.y;
	V.z=z+v.z;
	return V;
}

Vector3& Vector3::operator-(const Vector3& v)
{
	Vector3 V;
	V.x=x-v.x;
	V.y=y-v.y;
	V.z=z-v.z;
	return V;
}

void Vector3::Normalize()
{
	float Mag=sqrt(x*x+y*y+z*z);
	x/=Mag;
	y/=Mag;
	z/=Mag;
}

void Vector3::Set(float X,float Y,float Z)
{
	x=X;
	y=Y;
	z=Z;
}

#endif