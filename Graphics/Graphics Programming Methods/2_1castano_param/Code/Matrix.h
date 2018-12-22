/*============================================================================
	Title: Matrix.h
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: Matrix classes and functions.
	Changes:
		22/10/2000 - Ignacio Castaño
			Math library started.

============================================================================*/

#ifndef _PI_MATRIX_H_
#define _PI_MATRIX_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file Matrix.h
 * Matrix classes and functions.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

#include "Core.h"
#include "Debug.h"
#include "Archive.h"
#include "MathLib.h"
#include "Real.h"



/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/**
 * 3D matrix. To be used as a rotation only matrix, or as a 2d homogeneous 
 * matrix.
**/
struct MATHLIB_API Mat3 {

	Mat3(){}
	Mat3( const Mat3 &src ) {
		data[0]=src.data[0]; data[1]=src.data[1]; data[2]=src.data[2];
		data[3]=src.data[3]; data[4]=src.data[4]; data[5]=src.data[5];
		data[6]=src.data[6]; data[7]=src.data[7]; data[8]=src.data[8];
	}
	~Mat3(){}

	void Identity( void ) {
		data[0]=1; data[3]=0; data[6]=0;
		data[1]=0; data[4]=1; data[7]=0;
		data[2]=0; data[5]=0; data[8]=1;
	}
	void Zero( void ) {
		data[0]=0; data[3]=0; data[6]=0;
		data[1]=0; data[4]=0; data[7]=0;
		data[2]=0; data[5]=0; data[8]=0;
	}

	void Translation2d( REAL a, REAL b ) {
		data[0]=1; data[3]=0; data[6]=a;
		data[1]=0; data[4]=1; data[7]=b;
		data[2]=0; data[5]=0; data[8]=1;
	}
	void Translate2d( REAL a, REAL b ) {
		Mat3 m;
		m.Translation2d(a, b);
		Multiply(m);
	}
	void Rotation2d( REAL a ) {
		REAL c = cos(a);
		REAL s = sin(a);
		data[0]=c; data[3]=-s; data[6]=0;
		data[1]=s; data[4]=c; data[7]=0;
		data[2]=0; data[5]=0; data[8]=1;
	}
	void Rotate2d( REAL a ) {
		Mat3 m;
		m.Rotation2d(a);
		Multiply(m);
	}
	void Scaling2d( REAL a, REAL b ) {
		data[0]=a; data[3]=0; data[6]=0;
		data[1]=0; data[4]=b; data[7]=0;
		data[2]=0; data[5]=0; data[8]=1;
	}
	void Scale2d( REAL a, REAL b ) {
		Mat3 m;
		m.Scaling2d(a, b);
		Multiply(m);
	}
	void Reflection2d( bool axis ) {	// 0=X, 1=Y
		if(axis) Scaling2d(-1, 1);
		else Scaling2d(1, -1);
	}
	void Reflect2d( bool axis ) {
		Mat3 m;
		m.Reflection2d(axis);
		Multiply(m);
	}

	void Multiply( const Mat3 &a ) {
		Mat3 m(*this);

		data[0] = a.data[0]*m.data[0] + a.data[1]*m.data[3] + a.data[2]*m.data[6];
	    data[1] = a.data[0]*m.data[1] + a.data[1]*m.data[4] + a.data[2]*m.data[7];
		data[2] = a.data[0]*m.data[2] + a.data[1]*m.data[5] + a.data[2]*m.data[8];
	    data[3] = a.data[3]*m.data[0] + a.data[4]*m.data[3] + a.data[5]*m.data[6];
		data[4] = a.data[3]*m.data[1] + a.data[4]*m.data[4] + a.data[5]*m.data[7];
	    data[5] = a.data[3]*m.data[2] + a.data[4]*m.data[5] + a.data[5]*m.data[8];
		data[6] = a.data[6]*m.data[0] + a.data[7]*m.data[3] + a.data[8]*m.data[6];
	    data[7] = a.data[6]*m.data[1] + a.data[7]*m.data[4] + a.data[8]*m.data[7];
		data[8] = a.data[6]*m.data[2] + a.data[7]*m.data[5] + a.data[8]*m.data[8];
	}

	void TransformVector2d(float * in, float * out) const {
		out[0] = in[0] * data[0] + in[1] * data[3] + data[6];
		out[1] = in[0] * data[1] + in[1] * data[4] + data[7];
	}

	void TransformVec3(float * in, float * out) const {
		out[0] = in[0] * data[0] + in[1] * data[3] + in[2] * data[6];
		out[1] = in[0] * data[1] + in[1] * data[4] + in[2] * data[7];
		out[2] = in[0] * data[2] + in[1] * data[5] + in[2] * data[8];
	}

	void TransformVec3T(float * in, float * out) const {
		out[0] = in[0] * data[0] + in[1] * data[1] + in[2] * data[2];
		out[1] = in[0] * data[3] + in[1] * data[4] + in[2] * data[5];
		out[2] = in[0] * data[6] + in[1] * data[7] + in[2] * data[8];
	}

	void RotationMatrix( float yaw, float pitch, float roll ) {
		REAL sy = sin(yaw);
		REAL cy = cos(yaw);
		REAL sp = sin(pitch);
		REAL cp = cos(pitch);
		REAL sr = sin(roll);
		REAL cr = cos(roll);

		data[0] = cr*cy + sr*sp*sy;
		data[3] = -cr*sy + sr*sp*cy;
		data[6] = sr*cp;

		data[1] = cp*sy;
		data[4] = cp*cy;
		data[7] = -sp;

		data[2] = -sr*cy + cr*sp*sy;
		data[5] = sr*sy + cr*sp*cy;
		data[8] = cr*cp;
	}

	/** Matrix serialization. */
	friend PiArchive & operator<< ( PiArchive & Ar, Mat3 & m ) {
		Ar << m.data[0] << m.data[1] << m.data[2];
		Ar << m.data[3] << m.data[4] << m.data[5];
		Ar << m.data[6] << m.data[7] << m.data[8];
	}

	REAL data[9];
};



/**
 * 3D homogeneous matrix.
 *
 * Some methods of this class are commented out because they were not being used. That
 * code was untested, and in some cases wrong, and that caused me some headaches, so
 * commented code just means, code that has to be revised in order to be used.
 *
 * @sa @ref order
**/
struct MATHLIB_API Matrix {

	Matrix(){}
	Matrix(const Matrix &src){
		data[0] =src.data[0];  data[1] =src.data[1];  data[2] =src.data[2];  data[3] =src.data[3];
		data[4] =src.data[4];  data[5] =src.data[5];  data[6] =src.data[6];  data[7] =src.data[7];
		data[8] =src.data[8];  data[9] =src.data[9];  data[10]=src.data[10]; data[11]=src.data[11];
		data[12]=src.data[12]; data[13]=src.data[13]; data[14]=src.data[14]; data[15]=src.data[15];
	}
	~Matrix(){}

	operator REAL * () const { return (REAL *) data; }

	void Copy( const Matrix &src ){
		data[0] = src.data[0]; data[1] = src.data[1]; data[2] = src.data[2]; data[3] = src.data[3];
		data[4] = src.data[4]; data[5] = src.data[5]; data[6] = src.data[6]; data[7] = src.data[7];
		data[8] = src.data[8]; data[9] = src.data[9]; data[10] = src.data[10]; data[11] = src.data[11];
		data[12] = src.data[12]; data[13] = src.data[13]; data[14] = src.data[14]; data[15] = src.data[15];
	}
	void Identity(void){
		data[0] = 1; data[1] = 0; data[2] =0; data[3] =0;
		data[4] = 0; data[5] = 1; data[6] =0; data[7] =0;
		data[8] = 0; data[9] = 0; data[10]=1; data[11]=0;
		data[12]= 0; data[13]= 0; data[14]=0; data[15]=1;
	}
	void Zero(void){
		data[0] = 0; data[1] = 0; data[2] =0; data[3] =0;
		data[4] = 0; data[5] = 0; data[6] =0; data[7] =0;
		data[8] = 0; data[9] = 0; data[10]=0; data[11]=0;
		data[12]= 0; data[13]= 0; data[14]=0; data[15]=0;
	}

	
	//
	// some usefull matrices:
	//
/*	void TranslationMatrix(Vec3 t){
		data[0] = 1; data[1] = 0; data[2] = 0; data[3] = t.x;
		data[4] = 0; data[5] = 1; data[6] = 0; data[7] = t.y;
		data[8] = 0; data[9] = 0; data[10]= 1; data[11]= t.z;
		data[12]= 0; data[13]= 0; data[14]= 0; data[15]= 1;
	}
	void ScaleMatrix(const REAL * v){
		data[0] = v[0]; data[1] = 0; data[2] = 0; data[3] = 0;
		data[4] = 0; data[5] = v[1]; data[6] = 0; data[7] = 0;
		data[8] = 0; data[9] = 0; data[10] = v[2]; data[11] = 0;
		data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 1;
	}
	void ScaleMatrix(REAL f){
		data[0] = f; data[1] = 0; data[2] = 0; data[3] = 0;
		data[4] = 0; data[5] = f; data[6] = 0; data[7] = 0;
		data[8] = 0; data[9] = 0; data[10] = f; data[11] = 0;
		data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 1;
	}
*/
	//
	// rotate around vector v (normalizes v!!!)
	//
/*	void RotationMatrix(REAL theta, REAL * v){
		float cost = cos(theta);
		float sint = sin(theta);

		if ( 1 == v[0] && 0 == v[1] && 0 == v[2]) {
			data[0] = 1.0f;	data[1] = 0.0f;	data[2] = 0.0f;	data[3] = 0.0f;
	        data[4] = 0.0f;	data[5] = cost;	data[6] = -sint;data[7] = 0.0f;
		    data[8] = 0.0f;	data[9] = sint;	data[10] = cost;data[11] = 0.0f;
			data[12] = 0.0f;data[13] = 0.0f;data[14] = 0.0f;data[15] = 1.0f;
	    }
		else if (0 == v[0]  && 1 == v[1] && 0 == v[2]) {
	        data[0] = cost;	data[1] = 0.0f;	data[2] = sint;	data[3] = 0.0f;
		    data[4] = 0.0f;	data[5] = 1.0f;	data[6] = 0.0f;	data[7] = 0.0f;
			data[8] = -sint;data[9] = 0.0f;data[10] = cost;	data[11] = 0.0f;
			data[12] = 0.0f;data[13] = 0.0f;data[14] = 0.0f;data[15] = 1.0f;
	    }
		else if (0 == v[0] && 0 == v[1] && 1 == v[2]) {
			data[0] = cost;	data[1] = -sint;data[2] = 0.0f;	data[3] = 0.0f;
	        data[4] = sint; data[5] = cost;	data[6] = 0.0f;	data[7] = 0.0f;
		    data[8] = 0.0f;	data[9] = 0.0f;	data[10] = 1.0f;data[11] = 0.0f;
			data[12] = 0.0f;data[13] = 0.0f;data[14] = 0.0f;data[15] = 1.0f;
	    } else {
			//we need scale a,b,c to unit length.
			REAL a2, b2, c2;
	        a2 = v[0] * v[0];
		    b2 = v[1] * v[1];
			c2 = v[2] * v[2];

			REAL iscale = InverseSqrt(a2 + b2 + c2);
			v[0]*=iscale;
			v[1]*=iscale;
			v[2]*=iscale;

			REAL abm, acm, bcm;
			REAL mcos, asin, bsin, csin;
	        mcos = 1.0f - cost;
		    abm = v[0] * v[1] * mcos;
			acm = v[0] * v[2] * mcos;
	        bcm = v[1] * v[2] * mcos;
		    asin = v[0] * sint;
			bsin = v[1] * sint;
	        csin = v[2] * sint;
		    data[0] = a2 * mcos + cost;
			data[1] = abm - csin;
	        data[2] = acm + bsin;
		    data[3] = abm + csin;
			data[4] = 0.0f;
	        data[5] = b2 * mcos + cost;
		    data[6] = bcm - asin;
			data[7] = acm - bsin;
			data[8] = 0.0f;
		    data[9] = bcm + asin;
			data[10] = c2 * mcos + cost;
			data[11] = 0.0f;
			data[12] = 0.0f;data[13] = 0.0f;data[14] = 0.0f;data[15] = 1.0f;
		}
	}
*/


	/**
	 * Generate rotation matrix for the euler angles. This is the same as computing
	 * 3 rotation matrices and multiplying them together in our custom order.
	 *
	 * FIXME: generate sin and cos at the same time using low level assembly code
	**/
	void RotationMatrix( REAL yaw, REAL pitch, REAL roll){
	//	REAL sy = sin(yaw+ToRadian(90));
	//	REAL cy = cos(yaw+ToRadian(90));
	//	REAL sp = sin(pitch-ToRadian(90));
	//	REAL cp = cos(pitch-ToRadian(90));
		REAL sy = sin(yaw+ToRadian(90));
		REAL cy = cos(yaw+ToRadian(90));
		REAL sp = sin(pitch-ToRadian(90));
		REAL cp = cos(pitch-ToRadian(90));
		REAL sr = sin(roll);
		REAL cr = cos(roll);

		data[0] = cr*cy + sr*sp*sy;
		data[1] = cp*sy;
		data[2] = -sr*cy + cr*sp*sy;
		data[3] = 0;

		data[4] = -cr*sy + sr*sp*cy;
		data[5] = cp*cy;
		data[6] = sr*sy + cr*sp*cy;
		data[7] = 0;

		data[8] = sr*cp;
		data[9] = -sp;
		data[10] = cr*cp;
		data[11] = 0;

		data[12] = 0;
		data[13] = 0;
		data[14] = 0;
		data[15] = 1;
	}


	/** Create a frustum matrix with the far plane at the infinity. */
	void Frustum( REAL xmin, REAL xmax, REAL ymin, REAL ymax, REAL zNear, REAL zFar ) {
		REAL one_deltax, one_deltay, one_deltaz, doubleznear;

		doubleznear = 2. * zNear;
		one_deltax = 1. / (xmax - xmin);
		one_deltay = 1. / (ymax - ymin);
		one_deltaz = 1. / (zFar - zNear);

		data[0] = (REAL)(doubleznear * one_deltax);
		data[1] = 0.f;
		data[2] = 0.f;
		data[3] = 0.f;
		data[4] = 0.f;
		data[5] = (REAL)(doubleznear * one_deltay);
		data[6] = 0.f;
		data[7] = 0.f;
		data[8] = (REAL)((xmax + xmin) * one_deltax);
		data[9] = (REAL)((ymax + ymin) * one_deltay);
		data[10] = (REAL)(-(zFar + zNear) * one_deltaz);
		data[11] = -1.f;
		data[12] = 0.f;
		data[13] = 0.f;
		data[14] = (REAL)(-(zFar * doubleznear) * one_deltaz);
		data[15] = 0.f;
	}


	/** Create a frustum matrix with the far plane at the infinity. */
	void FrustumInf( REAL left, REAL right, REAL bottom, REAL top, REAL zNear ) {
	    float nudge = 1.0; // 0.999;
		data[0] = (2*zNear) / (right - left);
		data[1] = 0;
		data[2] = (right + left) / (right - left);
		data[3] = 0;
		data[4] = 0;
		data[5] = (2*zNear) / (top - bottom);
		data[6] = (top + bottom) / (top - bottom);
		data[7] = 0;
		data[8] = 0;
		data[9] = 0;
		data[10] = -1  * nudge;
		data[11] = -2*zNear * nudge;
		data[12] = 0;
		data[13] = 0;
		data[14] = -1;
		data[15] = 0;
	}


	/** Create an inverse frustum matrix with the far plane at the infinity. */
	void FrustumInfInv( REAL left, REAL right, REAL bottom, REAL top, REAL zNear ) {
		data[0] = (right - left) / (2 * zNear);
		data[1] = 0;
		data[2] = 0;
		data[3] = (right + left) / (2 * zNear);
		data[4] = 0;
		data[5] = (top - bottom) / (2 * zNear);
		data[6] = 0;
		data[7] = (top + bottom) / (2 * zNear);
		data[8] = 0;
		data[9] = 0;
		data[10] = 0;
		data[11] = -1;
		data[12] = 0;
		data[13] = 0;
		data[14] = -1 / (2 * zNear);
		data[15] = 1 / (2 * zNear);
	}


	/** Create an homogeneous projection matrix. */
	void Perspective( REAL fov, REAL aspect, REAL zNear, REAL zFar ) {
		REAL xmin, xmax, ymin, ymax;

		xmax = zNear * tan( fov/2 );
		xmin = -xmax;

		ymax = xmax / aspect;
		ymin = -ymax;

		Frustum(xmin, xmax, ymin, ymax, zNear, zFar);
	}

	/** Create a projection matrix with the far plane at the infinity. */
	void PerspectiveInf( REAL fov, REAL aspect, REAL zNear ) {
		REAL x = zNear * tan( fov/8 );
		REAL y = x / aspect;
		FrustumInf(-x, x, -y, y, zNear);
	}

	/** Create an inverse projection matrix with far plane at the infinity. */
	void PerspectiveInfInv( REAL fov, REAL aspect, REAL zNear ) {
		REAL x = zNear * tan( fov/2 );
		REAL y = x / aspect;
		FrustumInfInv(-x, x, -y, y, zNear);
	}


	//
	// Build matrix for offset and quaternion
	//
	/*void Build(Quat q, Vec3 offset) {
		REAL x2, y2, z2, xx, xy, xz, yy, yz, zz, wx, wy, wz;

		// calculate coefficients
		x2 = q.x + q.x; 
		y2 = q.y + q.y; 
		z2 = q.z + q.z;

		xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
		yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
		wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

		data[0] = 1.0f - (yy + zz); 	
		data[1] = xy - wz;
		data[2] = xz + wy;		
		data[3] = 0.0f;
 
		data[4] = xy + wz;		
		data[5] = 1.0f - (xx + zz);
		data[6] = yz - wx;		
		data[7] = 0.0f;

		data[8] = xz - wy;		
		data[9] = yz + wx;
		data[10] = 1.0f - (xx + yy);		
		data[11] = 0.0f;

		data[12] = offset.x;
		data[13] = offset.y;
		data[14] = offset.z;			
		data[15] = 1.0f;
	}*/



	//
	// Transformations:
	//

	/*void Multiply(const Mat3 &a){
		Matrix m(*this);

		data[0] = a.data[0]*m.data[0] + a.data[1]*m.data[3] + a.data[2]*m.data[6];
	    data[1] = a.data[0]*m.data[1] + a.data[1]*m.data[4] + a.data[2]*m.data[7];
		data[2] = a.data[0]*m.data[2] + a.data[1]*m.data[5] + a.data[2]*m.data[8];
	    data[3] = a.data[3]*m.data[0] + a.data[4]*m.data[3] + a.data[5]*m.data[6];
		data[4] = a.data[3]*m.data[1] + a.data[4]*m.data[4] + a.data[5]*m.data[7];
	    data[5] = a.data[3]*m.data[2] + a.data[4]*m.data[5] + a.data[5]*m.data[8];
		data[6] = a.data[6]*m.data[0] + a.data[7]*m.data[3] + a.data[8]*m.data[6];
	    data[7] = a.data[6]*m.data[1] + a.data[7]*m.data[4] + a.data[8]*m.data[7];
		data[8] = a.data[6]*m.data[2] + a.data[7]*m.data[5] + a.data[8]*m.data[8];
	}
	void Add(const Matrix &a, const Matrix &b){
	}
	void Sub(const Matrix &a, const Matrix &b){
	}*/

	/*void Scale(REAL f){
		data[0] *= f;	data[4] *= f;	data[8]  *= f;
		data[1] *= f;	data[5] *= f;	data[9]  *= f;
		data[2] *= f;	data[6] *= f;	data[10] *= f;
		data[3] *= f;	data[7] *= f;	data[11] *= f;
	}
	void Scale(REAL x, REAL y, REAL z){
		data[0] *= x;	data[4] *= y;	data[8]  *= z;
		data[1] *= x;	data[5] *= y;	data[9]  *= z;
		data[2] *= x;	data[6] *= y;	data[10] *= z;
		data[3] *= x;	data[7] *= y;	data[11] *= z;
	}*/


	/** 
	 * Translate the matrix by t. This is the same as multiplying by a 
	 * translation matrix with the given offset.
	**/
	void Translate( const Vec3 &t ) {
		data[12] = data[0] * t.x + data[4] * t.y + data[8]  * t.z + data[12];
		data[13] = data[1] * t.x + data[5] * t.y + data[9]  * t.z + data[13];
		data[14] = data[2] * t.x + data[6] * t.y + data[10] * t.z + data[14];
		data[15] = data[3] * t.x + data[7] * t.y + data[11] * t.z + data[15];
	}

	/** 
	 * Translate the matrix by x, y, z. This is the same as multiplying by a 
	 * translation matrix with the given offsets.
	**/
	void Translate( REAL x, REAL y, REAL z ) {
		data[12] = data[0] * x + data[4] * y + data[8]  * z + data[12];
		data[13] = data[1] * x + data[5] * y + data[9]  * z + data[13];
		data[14] = data[2] * x + data[6] * y + data[10] * z + data[14];
		data[15] = data[3] * x + data[7] * y + data[11] * z + data[15];
	}


	/** Compute the transposed matrix. */
	void Transpose( void ) {
		REAL temp;
		temp=data[1]; data[1]=data[4]; data[4]=temp;
		temp=data[2]; data[2]=data[8]; data[8]=temp;
		temp=data[6]; data[6]=data[9]; data[9]=temp;
		temp=data[3]; data[3]=data[12]; data[12]=temp;
		temp=data[7]; data[7]=data[13]; data[13]=temp;
		temp=data[11]; data[11]=data[14]; data[14]=temp;
	}


	/** Compute the inverse of a rigid-body/isometry/orthonormal matrix. */
	void IsometryInverse( void ) {
		REAL temp;

		temp=data[1]; data[1]=data[4]; data[4]=temp;	// transposed 3x3 upper left matrix
		temp=data[2]; data[2]=data[8]; data[8]=temp;
		temp=data[6]; data[6]=data[9]; data[9]=temp;

		Vec3 v(-data[12], -data[13], -data[14]);
		data[12] = data[13] = data[14] = 0;
		Translate(v);									// translate by the negative offsets
	}


	/** Standard matrix product: this = A·B. */
	void Multiply( const Matrix &A, const Matrix &B ) {
		data[0] = A.data[0] * B.data[0] + A.data[4] * B.data[1] + A.data[8] * B.data[2] + A.data[12] * B.data[3];
		data[1] = A.data[1] * B.data[0] + A.data[5] * B.data[1] + A.data[9] * B.data[2] + A.data[13] * B.data[3];
		data[2] = A.data[2] * B.data[0] + A.data[6] * B.data[1] + A.data[10] * B.data[2] + A.data[14] * B.data[3];
		data[3] = A.data[3] * B.data[0] + A.data[7] * B.data[1] + A.data[11] * B.data[2] + A.data[15] * B.data[3];
		data[4] = A.data[0] * B.data[4] + A.data[4] * B.data[5] + A.data[8] * B.data[6] + A.data[12] * B.data[7];
		data[5] = A.data[1] * B.data[4] + A.data[5] * B.data[5] + A.data[9] * B.data[6] + A.data[13] * B.data[7];
		data[6] = A.data[2] * B.data[4] + A.data[6] * B.data[5] + A.data[10] * B.data[6] + A.data[14] * B.data[7];
		data[7] = A.data[3] * B.data[4] + A.data[7] * B.data[5] + A.data[11] * B.data[6] + A.data[15] * B.data[7];
		data[8] = A.data[0] * B.data[8] + A.data[4] * B.data[9] + A.data[8] * B.data[10] + A.data[12] * B.data[11];
		data[9] = A.data[1] * B.data[8] + A.data[5] * B.data[9] + A.data[9] * B.data[10] + A.data[13] * B.data[11];
		data[10]= A.data[2] * B.data[8] + A.data[6] * B.data[9] + A.data[10] * B.data[10] + A.data[14] * B.data[11];
		data[11]= A.data[3] * B.data[8] + A.data[7] * B.data[9] + A.data[11] * B.data[10] + A.data[15] * B.data[11];
		data[12]= A.data[0] * B.data[12] + A.data[4] * B.data[13] + A.data[8] * B.data[14] + A.data[12] * B.data[15];
		data[13]= A.data[1] * B.data[12] + A.data[5] * B.data[13] + A.data[9] * B.data[14] + A.data[13] * B.data[15];
		data[14]= A.data[2] * B.data[12] + A.data[6] * B.data[13] + A.data[10] * B.data[14] + A.data[14] * B.data[15];
		data[15]= A.data[3] * B.data[12] + A.data[7] * B.data[13] + A.data[11] * B.data[14] + A.data[15] * B.data[15];
	}


	/** Standard product of matrices, where the last row is [0 0 0 1]. */
	void MultiplyH( const Matrix &a, const Matrix &b ) {
		data[0] = a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12] * b.data[3];
		data[1] = a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13] * b.data[3];
		data[2] = a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14] * b.data[3];
		data[3] = 0.0f;
		data[4] = a.data[0] * b.data[4] + a.data[4] * b.data[5] + a.data[8] * b.data[6] + a.data[12] * b.data[7];
		data[5] = a.data[1] * b.data[4] + a.data[5] * b.data[5] + a.data[9] * b.data[6] + a.data[13] * b.data[7];
		data[6] = a.data[2] * b.data[4] + a.data[6] * b.data[5] + a.data[10] * b.data[6] + a.data[14] * b.data[7];
		data[7] = 0.0f;
		data[8] = a.data[0] * b.data[8] + a.data[4] * b.data[9] + a.data[8] * b.data[10] + a.data[12] * b.data[11];
		data[9] = a.data[1] * b.data[8] + a.data[5] * b.data[9] + a.data[9] * b.data[10] + a.data[13] * b.data[11];
		data[10]= a.data[2] * b.data[8] + a.data[6] * b.data[9] + a.data[10] * b.data[10] + a.data[14] * b.data[11];
		data[11]= 0.0f;
		data[12]= a.data[0] * b.data[12] + a.data[4] * b.data[13] + a.data[8] * b.data[14] + a.data[12] * b.data[15];
		data[13]= a.data[1] * b.data[12] + a.data[5] * b.data[13] + a.data[9] * b.data[14] + a.data[13] * b.data[15];
		data[14]= a.data[2] * b.data[12] + a.data[6] * b.data[13] + a.data[10] * b.data[14] + a.data[14] * b.data[15];
		data[15]= 1.0f;
	}



	// 
	// Operations
	//

	/** Transform 3d vector (w=0) */
	void TransformVec3(const REAL * orig, REAL * dest) const {
		dest[0] = orig[0] * data[0] + orig[1] * data[4] + orig[2] * data[8];
		dest[1] = orig[0] * data[1] + orig[1] * data[5] + orig[2] * data[9];
		dest[2] = orig[0] * data[2] + orig[1] * data[6] + orig[2] * data[10];
	}
	/** Transform 3d vector by the transpose (w=0) */
	void TransformVec3T(const REAL * orig, REAL * dest) const {
		dest[0] = orig[0] * data[0] + orig[1] * data[1] + orig[2] * data[2];
		dest[1] = orig[0] * data[4] + orig[1] * data[5] + orig[2] * data[6];
		dest[2] = orig[0] * data[8] + orig[1] * data[9] + orig[2] * data[10];
	}

	/** Transform a 3d homogeneous vector, where the last coordinate is assumed to be 1. */
	void TransformPoint(const REAL * orig, REAL * dest) const {
		dest[0] = orig[0] * data[0] + orig[1] * data[4] + orig[2] * data[8] + data[12];
		dest[1] = orig[0] * data[1] + orig[1] * data[5] + orig[2] * data[9] + data[13];
		dest[2] = orig[0] * data[2] + orig[1] * data[6] + orig[2] * data[10] + data[14];
	}

	/** Transform a point, normalize it, and return w. */
	float TransformPointAndNormalize(const REAL * orig, REAL * dest) const {
		REAL w;
		dest[0] = orig[0] * data[0] + orig[1] * data[4] + orig[2] * data[8] + data[12];
		dest[1] = orig[0] * data[1] + orig[1] * data[5] + orig[2] * data[9] + data[13];
		dest[2] = orig[0] * data[2] + orig[1] * data[6] + orig[2] * data[10] + data[14];
		w = 1 /  (orig[0] * data[3] + orig[1] * data[7] + orig[2] * data[11] + data[15]);
		dest[0] *= w;
		dest[1] *= w;
		dest[2] *= w;
		return w;
	}

	/** Transform a point and return w. */
	float TransformPointReturnW(const REAL * orig, REAL * dest) const {
		dest[0] = orig[0] * data[0] + orig[1] * data[4] + orig[2] * data[8] + data[12];
		dest[1] = orig[0] * data[1] + orig[1] * data[5] + orig[2] * data[9] + data[13];
		dest[2] = orig[0] * data[2] + orig[1] * data[6] + orig[2] * data[10] + data[14];
		return orig[0] * data[3] + orig[1] * data[7] + orig[2] * data[11] + data[15];
	}

	/** Transform a normalized 3d point by a 4d matrix and return the resulting 4d vector. */
	void TransformVec4(const REAL * orig, REAL * dest) const {
		dest[0] = orig[0] * data[0] + orig[1] * data[4] + orig[2] * data[8] + data[12];
		dest[1] = orig[0] * data[1] + orig[1] * data[5] + orig[2] * data[9] + data[13];
		dest[2] = orig[0] * data[2] + orig[1] * data[6] + orig[2] * data[10] + data[14];
		dest[3] = orig[0] * data[3] + orig[1] * data[7] + orig[2] * data[11] + data[15];
	}

	/** Matrix serialization. */
	friend PiArchive & operator<< ( PiArchive & Ar, Matrix & m ) {
		Ar << m.data[0] << m.data[1] << m.data[2] << m.data[3];
		Ar << m.data[4] << m.data[5] << m.data[6] << m.data[7];
		Ar << m.data[8] << m.data[9] << m.data[10] << m.data[11];
		Ar << m.data[12] << m.data[13] << m.data[14] << m.data[15];
	}


	REAL data[16];
};





#endif // _PI_MATRIX_H_
