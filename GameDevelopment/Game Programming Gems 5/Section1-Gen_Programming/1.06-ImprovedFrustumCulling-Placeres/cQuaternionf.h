#pragma once

#include "cVector3.h"
//#include "cEulerf.h"
//#include "cMatrix4f.h"

//=============================================================================
// The performed rotations are counter-clockwise related
//=============================================================================

class cQuaternionf
{
	public:
		inline cQuaternionf ()																									:x(0),y(0),z(0),w(1)			{};
		inline cQuaternionf ( const cQuaternionf &q )																			:x(q.x),y(q.y),z(q.z),w(q.w)	{};
		inline cQuaternionf ( const float AngleRAD, const float AxisX, const float AxisY, const float AxisZ )													{ SetFromAngleAxisRAD(AngleRAD,AxisX,AxisY,AxisZ);}
		inline cQuaternionf ( const float PitchRAD, const float YawRAD, const float RollRAD )																	{ SetFromEulerRAD(PitchRAD, YawRAD, RollRAD); }

		inline void	Normalize();

		inline void SetFromQuaternion( const float X, const float Y, const float Z, const float W )						{ x=X; y=Y; z=Z; w=W; }
		inline void SetFromEulerRAD (const float Pitch, const float Yaw, const float Roll);
		inline void	SetFromAngleAxisRAD	( float angleRad, const float ax,  const float ay, const float az);
//		inline cMatrix3f GetMatrix3f();
// 		inline cMatrix4f GetMatrix4f();
		inline void	GetEulersRAD		(float &Pitch, float &Yaw, float &Roll) const;
		inline void	GetAngleAxisRAD		(float &Angle, float &ax,  float &ay, float &az) const;
		inline void	NegateVectorPart()																					{ x = -x; y = -y;	z = -z; }
		inline void	LoadIdentity()																						{ x = y = z = 0; w = 1; }
		inline void	Slerp				(const cQuaternionf& from, const cQuaternionf& to, const float t);
		inline void	Lerp				(const cQuaternionf& from, const cQuaternionf& to, const float t);


		inline cQuaternionf& operator *= (const cQuaternionf &q);
		inline cQuaternionf  operator *  (const cQuaternionf &q) const;
		inline cVector3f     operator *  (const cVector3f &Point) const;
		inline bool		    operator == (const cQuaternionf &q) const;
		inline bool		    operator != (const cQuaternionf &q) const;

		// this = this * newQuat
		inline void RotateBd ( const float AngleRAD, const float AxisX, const float AxisY, const float AxisZ );
		inline void RotateBd ( const float PitchRAD, const float YawRAD, const float RollRAD );

		// this = newQuat * this
		inline void RotateWd ( const float AngleRAD, const float AxisX, const float AxisY, const float AxisZ );
		inline void RotateWd ( const float PitchRAD, const float YawRAD, const float RollRAD );

		float	x,y,z,w;
};


///////////////////////////////////////////////////////////////////////////////////////
//			NORMALIZE
///////////////////////////////////////////////////////////////////////////////////////

void cQuaternionf::Normalize()
{ 
	float mag = (x*x + y*y + z*z + w*w); 
	if (mag > 0){
		mag = 1/sqrtf(mag);
 	    x *= mag;	
		y *= mag; 
		z *= mag;	
		w *= mag; 
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//			SET FROM EULER ANGLES (RAD)
///////////////////////////////////////////////////////////////////////////////////////

void cQuaternionf::SetFromEulerRAD (const float Pitch, const float Yaw, const float Roll)
{ 	
	float tx,ty,tz,cx,cy,cz,sx,sy,sz,cc,cs,sc,ss; 
	tx = Pitch * 0.5f;
	ty = Yaw * 0.5f;
	tz = Roll * 0.5f;
	cx = cosf(tx);
	cy = cosf(ty);
	cz = cosf(tz);
	sx = sinf(tx);
	sy = sinf(ty);
	sz = sinf(tz);
	cc = cx * cz;
	cs = cx * sz;
	sc = sx * cz;
	ss = sx * sz;
	x = (cy * sc) - (sy * cs);
	y = (cy * ss) + (sy * cc);
	z = (cy * cs) - (sy * sc);
	w = (cy * cc) + (sy * ss);
	Normalize(); 
}

///////////////////////////////////////////////////////////////////////////////////////
//			GET MATRIX 4x4
///////////////////////////////////////////////////////////////////////////////////////
/*
cMatrix4f cQuaternionf::GetMatrix4f()
{ 	
	  float xx = x * x,
	  yy = y * y,
	  zz = z * z,
	  xy = x * y,
	  xz = x * z,
	  yz = y * z,
	  wx = w * x,
	  wy = w * y,
	  wz = w * z;
  	  return cMatrix4f(	1.0f - 2.0f * ( yy + zz ),
						2.0f * ( xy + wz ),
						2.0f * ( xz - wy ),
						0.0f,

						2.0f * ( xy - wz ),
						1.0f - 2.0f * ( xx + zz ),
						2.0f * ( yz + wx ),
						0.0f,
						
						2.0f * ( xz + wy ),
						2.0f * ( yz - wx ),
						1.0f - 2.0f* ( xx + yy ),
						0.0f,
						
						0.0f,
						0.0f,
						0.0f,
						1.0f);
}
///////////////////////////////////////////////////////////////////////////////////////
//			GET MATRIX 3x3
///////////////////////////////////////////////////////////////////////////////////////

cMatrix3f cQuaternionf::GetMatrix3f()
{ 	
	float xx = x * x,
	  yy = y * y,
	  zz = z * z,
	  xy = x * y,
	  xz = x * z,
	  yz = y * z,
	  wx = w * x,
	  wy = w * y,
	  wz = w * z;
  	  return cMatrix3f(	1.0f - 2.0f * ( yy + zz ),
						2.0f * ( xy + wz ),
						2.0f * ( xz - wy ),

						2.0f * ( xy - wz ),
						1.0f - 2.0f * ( xx + zz ),
						2.0f * ( yz + wx ),
						
						2.0f * ( xz + wy ),
						2.0f * ( yz - wx ),
						1.0f - 2.0f * ( xx + yy ));
}
*/

void	cQuaternionf::SetFromAngleAxisRAD	(float angleRad, const float ax,  const float ay, const float az)
{
	angleRad *= 0.5f;

	float sa( sinf(angleRad) );

	// Build a quaternion!
	w = cosf(angleRad);
	x = ax * sa;
	y = ay * sa;
	z = az * sa;

	Normalize();
}
///////////////////////////////////////////////////////////////////////////////////////
//			GET EULER ANGLES
///////////////////////////////////////////////////////////////////////////////////////

void	cQuaternionf::GetEulersRAD (float &Pitch, float &Yaw, float &Roll) const
{
	float matrix00,		  matrix10,
		  matrix20,		  matrix21,
		  matrix22,
		  matrix11,		  matrix12;
	float cy,sy;
	float OneDivCy;

	float yy = y * y;
	float zz = z * z;
	float xx = x * x;
	float wx = w * x;
	float yz = y * z;

    ///////////////////////////////////////////////////////////////////////////////
	// CONVERT QUATERNION TO MATRIX - I DON'float REALLY NEED ALL OF IT
	matrix00 = 1.0f - 2.0f * ( yy  + zz );
	matrix10 =		  2.0f * ( x*y + w*z );
	matrix20 =		  2.0f * ( x*z - w*y );
	matrix21 =		  2.0f * ( yz  + wx );
	matrix22 = 1.0f - 2.0f * ( xx  + yy );

	sy = -matrix20;
	cy = sqrtf(1 - (sy * sy));
	// atan2f(sy,cy);
	Yaw = atan2f(sy , cy);

	// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270 
	// NOT CHECKING cy BECAUSE OF PRECISION ERRORS
	if (sy != 1.0f && sy != -1.0f)	
	{
		OneDivCy = 1/cy;
		// atan2f( sx, cx )
		Pitch = atan2f(matrix21 * OneDivCy, matrix22 * OneDivCy);

		// atan2f( sz, cz )
		Roll = atan2f(matrix10 * OneDivCy, matrix00 * OneDivCy);
	}
	else
	{
		// sinfCE cosf(Y) IS 0, I AM SCREWED.  ADOPT THE STANDARD Z = 0
		// I THINK THERE IS A WAY TO FIX THIS BUT I AM NOT SURE.  EULERS SUCK
		// NEED SOME MORE OF THE MATRIX TERMS NOW
		matrix11 = 1.0f - 2.0f * ( xx + zz );
		matrix12 = 2.0f * ( yz - wx );

		// atan2f( sx, cx )
		Pitch = atan2f(-matrix12,matrix11);

		// atan2f( sz, cz ) = atan2f(0,1) = 0
		Roll = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//			GET ANGLE AND AXIS
///////////////////////////////////////////////////////////////////////////////////////

void	cQuaternionf::GetAngleAxisRAD (float &Angle, float &ax,  float &ay, float &az) const
{
   float	len;
    float one_over_len;

	len = x*x + y*y + z*z;

    if (len > 1e-6) 
	{
		one_over_len = 1.0f / len;
		ax = x * one_over_len;
		ay = y * one_over_len;
		az = z * one_over_len;
	    Angle = 2.0f * acosf(w);
    }

    else {
		ax = ay = Angle = 0.0f;
		az = 1.0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//			SET AS THE INTERPOLATION BETWEEN TWO QUATERNIONS
///////////////////////////////////////////////////////////////////////////////////////


void	cQuaternionf::Slerp (const cQuaternionf& from, const cQuaternionf& to, const float t)
{
	float cosfine = (from.x * to.x) + (from.y * to.y) + (from.z * to.z) + (from.w * to.w); 
	float angleR = acosf(cosfine); // angle is in radians..

	float lower_weight;
	float upper_weight;

	// set up our weights

	if( fabs(angleR)<0.0005f )
	{
		// numerically unstable when angle is close to 0, lerp instead
		lower_weight = t;
		upper_weight = 1.0f - t;
	}
	else
	{
		// this will also fall apart if w approaches k*pi/2 for k = [1, 2, ...]
		float i_sinf_angle = 1.0f / sinf(angleR);
		lower_weight = sinf(angleR - angleR*t) * i_sinf_angle;
		upper_weight = sinf(angleR * t) * i_sinf_angle;
	}
	w = (from.w * (lower_weight)) + (to.w * (upper_weight));
	x = (from.x * (lower_weight)) + (to.x * (upper_weight));
	y = (from.y * (lower_weight)) + (to.y * (upper_weight));
	z = (from.z * (lower_weight)) + (to.z * (upper_weight));
	Normalize();
}


void	cQuaternionf::Lerp (const cQuaternionf& from, const cQuaternionf& to, const float t)
{
	float lower_weight = t;
	float upper_weight = 1.0f - t;

	w = (to.w * (lower_weight)) + (from.w * (upper_weight));
	x = (to.x * (lower_weight)) + (from.x * (upper_weight));
	y = (to.y * (lower_weight)) + (from.y * (upper_weight));
	z = (to.z * (lower_weight)) + (from.z * (upper_weight));
}

///////////////////////////////////////////////////////////////////////////////////////
//			OVERLOADED OPERATORS
///////////////////////////////////////////////////////////////////////////////////////

cQuaternionf& cQuaternionf::operator *= (const cQuaternionf &q)
{
    float xx = x,
		  yy = y,
		  zz = z,
		  ww = w;
  
    x =	ww*q.x + xx*q.w + yy*q.z - zz*q.y;
    y =	ww*q.y + yy*q.w + zz*q.x - xx*q.z;
    z =	ww*q.z + zz*q.w + xx*q.y - yy*q.x;
    w =	ww*q.w - xx*q.x - yy*q.y - zz*q.z;

	return *this;
}


cQuaternionf cQuaternionf::operator *  (const cQuaternionf &q) const
{
	cQuaternionf quat;

	quat.x =	w * q.x + x * q.w   +	y * q.z - z * q.y;
    quat.y  =	w * q.y + y * q.w   +	z * q.x - x * q.z;
    quat.z  =	w * q.z + z * q.w   +	x * q.y - y * q.x;
    quat.w  =	w * q.w - x * q.x   -	y * q.y - z * q.z;
	return quat;
}

cVector3f cQuaternionf::operator *  (const cVector3f &v) const
{
	float xx = x * x,
		  yy = y * y,
		  zz = z * z,
		  xy = x * y,
		  xz = x * z,
		  yz = y * z,
		  wx = w * x,
		  wy = w * y,
		  wz = w * z;
	return cVector3f(
//								Matrix[0]						Matrix[3]					Matrix[6]
						(1.0f - 2.0f * ( yy + zz ))*v.x + (2.0f * ( xy - wz ))*v.y  + (2.0f * ( xz + wy ))*v.z,
//								Matrix[1]						Matrix[4]					Matrix[7]
						  (2.0f * ( xy + wz ))*v.x +  (1.0f - 2.0f * ( xx + zz ))*v.y  + (2.0f * ( yz - wx ))*v.z,
//								Matrix[2]						Matrix[5]					Matrix[8]
  						  (2.0f * ( xz - wy ))*v.x +      (2.0f * ( yz + wx ))*v.y  + (1.0f - 2.0f * ( xx + yy ))*v.z
					);
}


bool cQuaternionf::operator == (const cQuaternionf &q) const
{
	if (x != q.x) return false;
	if (y != q.y) return false;
	if (z != q.z) return false;
	if (w != q.w) return false;
	return true;
}


bool cQuaternionf::operator != (const cQuaternionf &q) const
{
	if (x != q.x) return true;
	if (y != q.y) return true;
	if (z != q.z) return true;
	if (w != q.w) return true;
	return false;
}


// this = this * newQuat
void cQuaternionf::RotateBd ( const float AngleRAD, const float AxisX, const float AxisY, const float AxisZ )
{
	cQuaternionf q(AngleRAD,  AxisX,AxisY,AxisZ);
	*this *= q;
}

void cQuaternionf::RotateBd ( const float PitchRAD, const float YawRAD, const float RollRAD )
{
	cQuaternionf q( PitchRAD, YawRAD, RollRAD );
	*this *= q;
}

// this = newQuat * this
void cQuaternionf::RotateWd ( const float AngleRAD, const float AxisX, const float AxisY, const float AxisZ )
{
	cQuaternionf q(AngleRAD,  AxisX,AxisY,AxisZ);
	*this = q * *this;
}

void cQuaternionf::RotateWd ( const float PitchRAD, const float YawRAD, const float RollRAD )
{
	cQuaternionf q( PitchRAD, YawRAD, RollRAD );
	*this = q * *this;
}
