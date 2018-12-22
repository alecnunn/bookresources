/* Copyright (C) Jason Shankel, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jason Shankel, 2000"
 */
/*
Interpolating Quaternions
Jason Shankel
*/

#include <math.h>

/*
provisional quaternion class and associated operators.  These will 
be replaced when the common quaternion library is available
*/

class quaternion
{
public:
	float x,y,z,w;
	const quaternion &Normalize()
	{
		float mag = static_cast<float>(sqrt(x*x+y*y+z*z+w*w));
		if(mag>0)
		{
			x/=mag;
			y/=mag;
			z/=mag;
			w/=mag;
		}
		return *this;
	}
};

quaternion operator *(const quaternion &q1,const quaternion &q2)
{
	quaternion ret;

	ret.x = q1.y*q2.z - q1.z*q2.y + q1.w*q2.x + q1.x*q2.w;
	ret.y = q1.z*q2.x - q1.x*q2.z + q1.w*q2.y + q1.y*q2.w;
	ret.z = q1.x*q2.y - q1.y*q2.x + q1.w*q2.z + q1.z*q2.w;
	ret.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	return ret;
}

quaternion operator *(const quaternion &q,float v)
{
	quaternion ret;
	ret.x = q.x*v;
	ret.y = q.y*v;
	ret.z = q.z*v;
	ret.w = q.w*v;
	return ret;
}

quaternion operator *(float v,const quaternion &q)
{
	return q*v;
}

quaternion operator /(const quaternion &q,float v)
{
	quaternion ret;
	ret.x = q.x/v;
	ret.y = q.y/v;
	ret.z = q.z/v;
	ret.w = q.w/v;
	return ret;
}



quaternion operator -(const quaternion &q1,const quaternion &q2)
{
	quaternion ret;
	ret.x = q1.x-q2.x;
	ret.y = q1.y-q2.y;
	ret.z = q1.z-q2.z;
	ret.w = q1.w-q2.w;
	return ret;
}

quaternion operator +(const quaternion &q1,const quaternion &q2)
{
	quaternion ret;
	ret.x = q1.x+q2.x;
	ret.y = q1.y+q2.y;
	ret.z = q1.z+q2.z;
	ret.w = q1.w+q2.w;
	return ret;
}

/*
Forward declarations
*/

quaternion Qlerp(const quaternion &q1,const quaternion &q2,float t);
quaternion Interpolate(int mode,const quaternion &q1,const quaternion &q2,const quaternion &a,const quaternion &b,float t);
quaternion Qslerp(const quaternion &q1,const quaternion &q2,float t);
quaternion Qsquad(const quaternion &q1,const quaternion &q2,const quaternion &a,const quaternion &b,float t);
quaternion Qsquad(const quaternion &q1,const quaternion &q2,const quaternion &a,float t);
quaternion Qspline(const quaternion &q1,const quaternion &q2,const quaternion &q3);
quaternion Qexp(const quaternion &q);
quaternion Qlog(const quaternion &q);

/*
Logarithm of a quaternion, given as:
Qlog(q) = v*a where q = [cos(a),v*sin(a)]
*/
quaternion Qlog(const quaternion &q)
{
	float a = static_cast<float>(acos(q.w));
	float sina = static_cast<float>(sin(a));
	quaternion ret;
	ret.w = 0;
	if (sina > 0)
	{
		ret.x = a*q.x/sina;
		ret.y = a*q.y/sina;
		ret.z = a*q.z/sina;
	}
	else
	{
		ret.x=ret.y=ret.z=0;
	}
	return ret;
}

/*
e^quaternion given as:
Qexp(v*a) = [cos(a),vsin(a)]
*/
quaternion Qexp(const quaternion &q)
{
	float a = static_cast<float>(sqrt(q.x*q.x + q.y*q.y + q.z*q.z));
	float sina = static_cast<float>(sin(a));
	float cosa = static_cast<float>(cos(a));
	quaternion ret;

	ret.w = cosa;
	if(a > 0)
	{
		ret.x = sina * q.x / a;
		ret.y = sina * q.y / a;
		ret.z = sina * q.z / a;
	}
	else
	{
		ret.x = ret.y = ret.z = 0;
	}

	return ret;
}

/*
Linear interpolation between two quaternions
*/
quaternion Qlerp(const quaternion &q1,const quaternion &q2,float t)
{
	quaternion ret;
	ret = q1 + t*(q2-q1);
	/*
		return value must be normalized
	*/
	return ret.Normalize();
}

/*
Spherical linear interpolation between two quaternions
*/
quaternion Qslerp(const quaternion &q1,const quaternion &q2,float t)
{
	quaternion q3;
	float dot;
	dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

	/*
	dot = cos(theta)
	if (dot < 0), q1 and q2 are more than 90 degrees apart,
	so we can invert one to reduce spinning
	*/
	if (dot < 0)
	{
		dot = -dot;
		q3 = -1*q2;
	}
	else
	{
		q3 = q2;
	}

	
	if (dot < 0.95f)
	{
		float angle = static_cast<float>(acos(dot));
		float sina,sinat,sinaomt;
		sina = static_cast<float>(sin(angle));
		sinat = static_cast<float>(sin(angle*t));
		sinaomt = static_cast<float>(sin(angle*(1-t)));
		return (q1*sinaomt+q3*sinat)/sina;
	}
	/*
	if the angle is small, use linear interpolation
	*/
	else
	{
		return Qlerp(q1,q3,t);
	}
}

/*
This version of slerp, used by squad, does not check for theta > 90.
*/

quaternion QslerpNoInvert(const quaternion &q1,const quaternion &q2,float t)
{
	float dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

	if (dot > -0.95f && dot < 0.95f)
	{
		float angle = static_cast<float>(acos(dot));
		float sina,sinat,sinaomt;
		sina = static_cast<float>(sin(angle));
		sinat = static_cast<float>(sin(angle*t));
		sinaomt = static_cast<float>(sin(angle*(1-t)));
		return (q1*sinaomt+q2*sinat)/sina;
	}
	/*
	if the angle is small, use linear interpolation
	*/
	else
	{
		return Qlerp(q1,q2,t);
	}
}


/*
Spherical cubic interpolation
*/
quaternion Qsquad(const quaternion &q1,const quaternion &q2,const quaternion &a,const quaternion &b,float t)
{
	quaternion c,d;
	c = QslerpNoInvert(q1,q2,t);
	d = QslerpNoInvert(a,b,t);
	return QslerpNoInvert(c,d,2*t*(1-t));
}


/*
Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
*/

quaternion Qspline(const quaternion &qnm1,const quaternion &qn,const quaternion &qnp1)
{
	quaternion qni;
	
	qni.x = -qn.x;
	qni.y = -qn.y;
	qni.z = -qn.z;
	qni.w = qn.w;

	return qn*Qexp((Qlog(qni*qnm1)+Qlog(qni*qnp1))/-4);
}