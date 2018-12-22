#pragma once

#include "globals.hpp"

//! Implements data structure and operations for 3 floating point variables
//! Intented to use for 3D operations but suitable for RGB-represented color
//! also
class Vector {
public:
	//!Vector is also used for storing colours
	//!data can be accessed through various aliases
	union{
		float v[3];
		struct{ float x; float y; float z; };
		struct{ float r; float g; float b; };
		};

	static const Vector BLACK;

	Vector(){};

	Vector(float x, float y, float z)
	{
		Set(x,y,z);
	}

	void Set(float x, float y, float z)
	{
		v[0] = x; v[1] = y; v[2] = z;
	}

	bool NearEqual (const Vector& a) const {
		return fabs(a.x - x) < EPSILON && fabs(a.y - y) < EPSILON && fabs(a.z - z) < EPSILON;
	}

	//! Clip values to the [0.0f, 1.0f] closed interval
	void ClipToUnitInterval() {
		v[0] = MIN(MAX(v[0], 0.0f), 1.0f);
		v[1] = MIN(MAX(v[1], 0.0f), 1.0f);
		v[2] = MIN(MAX(v[2], 0.0f), 1.0f);
	}

	Vector operator-() const
	{
		return Vector(-v[0], -v[1], -v[2]);
	}

	Vector operator+(const Vector& addOperand) const
	{
		return Vector (	v[0] + addOperand.v[0],
						v[1] + addOperand.v[1],
						v[2] + addOperand.v[2]);
	}

	Vector operator-(const Vector& substractOperand) const
	{
		return Vector(  v[0] - substractOperand.v[0],
						v[1] - substractOperand.v[1],
						v[2] - substractOperand.v[2]);
	}

	void operator-=(const Vector& a)
	{
		v[0] -= a[0];
		v[1] -= a[1];
		v[2] -= a[2];
	}

	void operator+=(const Vector& a)
	{
		v[0] += a[0];
		v[1] += a[1];
		v[2] += a[2];
	}

	//! Implements the blend operator
	void operator%=(const Vector& a)
	{
		v[0] *= a[0];
		v[1] *= a[1];
		v[2] *= a[2];
	}

	//! Implements the blend operator
	Vector operator%(const Vector& blendOperand) const
	{
		return Vector(	v[0] * blendOperand.v[0],
						v[1] * blendOperand.v[1],
						v[2] * blendOperand.v[2]);
	}

	//! Implmentes the scale operator
	void operator*=(const float scale)
	{
		v[0] *= scale;
		v[1] *= scale;
		v[2] *= scale;
	}

	//! Implmentes the scale operator
	Vector operator*(const float scale) const
	{
		return Vector(scale * v[0], scale * v[1], scale * v[2]);
	}

	Vector operator/(const float scale) const
	{
		return Vector(v[0]/scale, v[1]/scale, v[2]/scale);
	}

	const Vector& operator/=(const float scale)
	{
		v[0]/=scale;
		v[1]/=scale;
		v[2]/=scale;

		return *this;
	}

	//! @return Calculates the dot product
	float operator*(const Vector& dotProductOperand) const
	{
		return	v[0] * dotProductOperand[0] + 
				v[1] * dotProductOperand[1] +
				v[2] * dotProductOperand[2];
	}

	//! @return Calculates the cross product
	Vector operator&&(const Vector& crossProductOperand) const
	{
		return Vector(
			v[1] * crossProductOperand[2] - v[2] * crossProductOperand[1],
			v[2] * crossProductOperand[0] - v[0] * crossProductOperand[2],
			v[0] * crossProductOperand[1] - v[1] * crossProductOperand[0]);
	}

	//! @return Calculates the length of the vector
	float Norm() { return sqrtf(Norm2()); }
	float Norm2() { return v[0]*v[0] + v[1]*v[1] + v[2]*v[2]; }

	//! Normalizes the vector
	//! @return the length of original Vector
	float Normalize ()
	{
		float length=Norm();
		operator*=(1.0f/length);
		return length;
	}

	const Vector& operator=(const Vector& other)
	{
		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
		return *this;
	}

	float& operator[](const int index)
	{
		return v[index];
	}

	float operator[](const int index) const
	{
		return v[index];
	}

	//!accumulate <b>mimimum</b> operator
	void operator<=(const Vector& s)
	{
		if(v[0] > s[0]) v[0] = s[0];
		if(v[1] > s[1]) v[1] = s[1];
		if(v[2] > s[2]) v[2] = s[2];
	}

	//!accumulate <b>maximum</b> operator
	void operator>=(const Vector& large)
	{
		if(v[0] < large[0]) v[0] = large[0];
		if(v[1] < large[1]) v[1] = large[1];
		if(v[2] < large[2]) v[2] = large[2];
	}

	void SetLinearCombination (float scaleA, const Vector& a, float scaleB, const Vector& b, float scaleC, const Vector& c) {
		*this = a*scaleA + b*scaleB + c*scaleC;
	}

	void SetLinearCombination (const Vector& a, float scaleB, const Vector& b, float scaleC, const Vector& c) {
		*this = a + b*scaleB + c*scaleC;
	}

	//! Calculates the average of the components
	float Average() { return (v[0]+v[1]+v[2])/3.0f; }


	//----------------------------------------------------


	//! Set the vector to the ideal reflection dir of the incoming direction
	void SetIdealReflectedDirection (const Vector& in, const Vector& normal)
	{
		*this = in - normal * (2.0f * (normal * in));
	}

	//! Set the vector to the ideal reflection dir of the incoming direction, 
	//! using a precomputed dotNormalIn dotproduct
	void SetIdealReflectedDirection (const Vector& in, const Vector& normal, float dotNormalIn)
	{
		*this = in - normal * (2.0f * dotNormalIn);
	}

	//! Determines the dominant axe
	/*! @return Returns the dominant axe out of (x,y,z) */
	DominantAxis GetDominantAxis (void) const {
		double dx = fabs (x);
		double dy = fabs (y);
		double dz = fabs (z);
		return (dx > dy) ? ((dx > dz) ? X_DOMINANT : Z_DOMINANT) : ((dy > dz) ? Y_DOMINANT : Z_DOMINANT);
	}

	void CalculateCoordSystem (Vector& X, Vector& Y, Vector& Z) const
	{
		Z = *this;
		DominantAxis dominantAxis = GetDominantAxis ();
		
		Vector axis;
		if (dominantAxis == Z_DOMINANT) 
			axis.Set(0.0, 1.0, 0.0);
		else 
			axis.Set(0.0, 0.0, 1.0);

		X= axis && Z;	// CrossProduct
		X.Normalize ();
		Y= Z && X;		// CrossProduct
		Y.Normalize ();
	}

	//! Sample the hemisphere following the cos distribution
	/*! For further information, see McCabe: Markov Chain MC: p. 18
	 * *this = normal vector
	 * @param outDir [out] The generated new direction, using *this as the normal vector
	 * @param pdf [out] Probability of the selected direction
	 */
	void SampleHemisphereCosTheta (Vector& outDir, float& pdf) const
	{
		Vector X, Y, Z; 
		CalculateCoordSystem (X, Y, Z);

		float phi = (float)(2.0f * M_PI * RAND);
		float xi_2 = RAND;
		float cos_phi = cosf(phi);
		float sin_phi = sinf(phi);
		float cos_theta = sqrtf(xi_2);
		float sin_theta = sqrtf(1.0f - xi_2);

		// see McCabe: Markov Chain MC: p. 18
		outDir.SetLinearCombination(sin_theta * cos_phi, X, sin_theta * sin_phi, Y, cos_theta, Z);
		pdf = (float) (cos_theta / M_PI);
	}

	/*! find a random direction of a cos^n(Theta) distribution with its modus at the ideal 
	 *	reflection direction
	 *	*this = idealReflDir
	 * @param Ns [in] specular exponent of the Brdf function
	 * @param specularPdfScaling [in] Precalculated Brdf variable, specularPdfScaling = (Ns + 1.0f)  / PI2;
	 * @param outDir [out] The generated new direction, using *this as the idealReflDir
	 * @param pdf [out] Probability of the selected direction
	*/
	void SampleHemisphereCosNTheta (float Ns, float specularPdfScaling, Vector& outDir, float& pdf)
	{
		Vector X, Y, Z; 
		CalculateCoordSystem (X, Y, Z);
		
		float phi = (float)(2.0f * M_PI * RAND);
		float cos_phi = cosf (phi);
		float sin_phi = sinf (phi);
		float cos_theta = powf (RAND, 1.0f / (Ns + 1));
		float sin_theta = sqrtf (1.0f - cos_theta*cos_theta);

		outDir.SetLinearCombination(sin_theta * cos_phi, X, sin_theta * sin_phi, Y, cos_theta, Z);
		pdf = powf (cos_theta, Ns) * specularPdfScaling;
	}

	void GenerateRandomTransillumDir()
	{
		float mfi = 2.0f * M_PI * RAND;
		float mteta = acosf(1.0f - 2.0f * RAND);
		Set(sinf(mfi) * sinf(mteta), cosf(mfi) * sinf(mteta), cosf(mteta));
	}

};

//! Color is represented by Red, Green, Blue components
typedef Vector Color;
