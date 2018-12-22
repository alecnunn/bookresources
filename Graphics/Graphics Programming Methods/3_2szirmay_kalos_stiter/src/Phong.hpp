#pragma once

#include "World.hpp"

#define SpecularAlbedosNum (90)

/*! Lafortune's modified Phong BRDF.
It satisfies the requirements of symmetry and energy conservation.
The BRDF is expressed as:
 
  brdf(in, out) = kd + ks*pow(cos(a),n)
 
  Where:  
  kd: diffuse coefficient of the BRDF
  ks: specular coefficient of the BRDF
  n: specular power
    n small : glossy reflectance
    n large : specular reflectance (>= PHONG_LOWEST_SPECULAR_EXP)
  a: angle between the out direction and the perfect mirror direction for in*/
class CPhongBrdf {
public: 
 /*! Kd = kd*pi		or	
  Ks = ks*2*pi/(n+2)	or	ks = Ks*(n+2)/(2*pi)*/
	//! diffuse albedo (kd = Kd/pi)
	Color Kd;

	//! specular albedo (ks = Ks*(n+2)/(2*pi))
    Color Ks;

	//! diffuse coefficient of the BRDF
	Color kd;

	//! average BRDF
	Color avgBrdf;

	//! specular exponent
	float Ns;

	//! average diffuse and specular albedo
	float avgKd, avgKs;

	//! pre-processed values
	float specularScaling, specularPdfScaling, scatteredPower;

	//! evalute the BRDF for the given incoming and outgoing directions
	/*!
	@param in points TO the surface
	@param out points FROM the surface
	@param normal Normal of the surface at the point
	*/
	Color EvalBrdf (const Vector& in, const Vector& out, const Vector& normal) { return EvalBrdf(in, out, normal, normal*in, normal*out); }
	Color EvalBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalIn) { return EvalBrdf(in, out, normal, dotNormalIn, normal*out); }
	Color EvalBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalIn, const float dotNormalOut);

	//! evalute the diffuse BRDF function for the given incoming and outgoing directions
	/*!
	@param in points TO the surface
	@param out points FROM the surface
	@param normal Normal of the surface at the point
	*/
	Color EvalDiffuseBrdf (const Vector& in, const Vector& out, const Vector& normal)	{ return EvalDiffuseBrdf(in, out, normal, normal*out);}
	Color EvalDiffuseBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalOut);

	//! evalute the specular BRDF function for the given incoming and outgoing directions
	/*!
	@param in points TO the surface
	@param out points FROM the surface
	@param normal Normal of the surface at the point
	*/
	Color EvalSpecularBrdf (const Vector& in, const Vector& out, const Vector& normal) { return EvalSpecularBrdf(in, out, normal, normal*in, normal*out); }
	Color EvalSpecularBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalIn) { return EvalSpecularBrdf(in, out, normal, dotNormalIn, normal*out); }
	Color EvalSpecularBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalIn, const float dotNormalOut);

	Color GetDiffuseBrdf() { return kd; };
	Color GetAverageBrdf() { return avgBrdf; };

	Color GetDiffuseAlbedo() { return Kd; }
	Color GetSpecularAlbedo() { return Ks; }

	Color GetAlbedo () { return Ks+Kd; }

	float GetReflectanceLum () { return avgKd + avgKs; }

	Color	specularMaxPhongScaling;
	Color	maxPhongSpecularAlbedos[SpecularAlbedosNum];

	Color	ComputedSpecularMaxPhongAlbedo(const Vector& out, const Vector& normal);
	Color	CalculateMaxPhongSpecularAlbedo(const Vector& outDir, const Vector& normal);

	//! precalculate values needed for quick evaluation from primary parameters
	void FinishPhong () {
		// calculate kd
		kd = Kd * M_1_PI;
		avgKd = Kd.Average ();
		avgKs = Ks.Average ();
		specularScaling = (Ns + 2.0f) / PI2;
		specularPdfScaling = (Ns + 1.0f)  / PI2;
		scatteredPower =  avgKd + avgKs;
		avgBrdf = kd + Ks*(1.0f/PI2);

		Vector surfaceDir(-1.0, 0.0, 0.0);
		Vector normal(0.0, 1.0, 0.0);
		for (short i = 0; i < SpecularAlbedosNum; i++) {
			
			Vector outDir;
			float a = ((float)i+0.5f) / (float)SpecularAlbedosNum;
			outDir=surfaceDir*a + normal * (1.0f - a);
			outDir.Normalize();
			maxPhongSpecularAlbedos[i] = CalculateMaxPhongSpecularAlbedo(outDir, normal);
		}

	}

};

//!Phong-type emission distribution function
class CPhongEdf {
public:
	//! radiance
	Color Kd;

	//! radiosity
	Color kd;

	//! calculte radiance from radiosity
	void FinishPhong() {
		Kd = kd * M_PI;
	}

	//! multiply radiance with 'patchArea'
	Color GetPowerOfArea(float patchArea) { return Kd * patchArea; }

	//! return average radiosity along wavelenghts
	float GetEmissionLum() { return kd.Average();	}
	
	Color GetEmissionRad() { return kd; }
	
	//! calculate radiance into a given direction
	Color EvalEdf (const Vector& out, const Vector& normal) { return EvalEdf(out, normal, out*normal); };
	Color EvalEdf (const Vector& out, const Vector& normal, const float dotNormalOut) {
		if (dotNormalOut<0) return Color::BLACK;	// not properly oriented
		else return kd;	// add diffuse part only now
	}
};

//! This class binds the emission and the reflection characteristic together */
class Material {
public:
	//! brdf must not be NULL, should always point to a valid brdf function
	CPhongBrdf*	m_brdf;

	//! edf may be NULL, if the material is non-emissive
	CPhongEdf*	m_edf;

	Material() : m_brdf(NULL), m_edf(NULL) {} ;
};