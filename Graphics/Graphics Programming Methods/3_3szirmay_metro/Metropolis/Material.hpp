// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

#pragma once
#include "Color.hpp"

#define MIRROR_SHINE_TRESHOLD 99.9  //  in vrml shine is [0.0-1.0] (*100)

// the Material class implemenets the Phong BRDF modell
//-----------------------------------------------------------------
class Material {
	static void SetCoordSystem(const Vector& targetZ, Vector* X, Vector* Y, Vector* Z);
	static void SampleHemisphereCosTheta(const Vector& normal, double rand1, double rand2, Vector* outDir, double* pdf);
	static void SampleHemisphereCosNTheta(const Vector& idealReflDir, double n, double specularPdfScaling, double rand1, double rand2, Vector* outDir, double* pdf);
public:
	Color	diffuseAlbedo;		// the diffuse reflectance of the material
	Color	specularAlbedo;		// the specular reflectance of the material
	double	shine;				// the shine coefficient of the Phong material

	bool	IsMirror();
	Color	Brdf(const Vector& inDir, const Vector& outDir, const Vector& normal);
	bool	SampleOutdir(const Vector& in, const Vector& normal, double rand1, double rand2, double rand3, 
						Vector* out, double* pdf);
};

// return true if it is a mirror material
//-----------------------------------------------------------------
inline bool Material::IsMirror(void) {
	return (shine > MIRROR_SHINE_TRESHOLD);
}

// calculate the Brdf of the material
// the 'in' points TO the surface, the 'out' points From the surface
//-----------------------------------------------------------------
inline Color Material::Brdf (const Vector& inDir, const Vector& outDir, const Vector& normal)
{
	if (normal * outDir < 0)
		return Color(0.0, 0.0, 0.0);

	Vector idealReflDirFrom = inDir + -2.0 * (normal * inDir) * normal; // 'in' points TO the surface
	if (IsMirror()) { // return Color(0) except for ideal mirror direction
		Vector diffVec = idealReflDirFrom - outDir;
		if (diffVec.Norm() < 1e-5F)
			return specularAlbedo;
		else
			return Color(0.0, 0.0, 0.0);
	}
	
	double dotProduct = idealReflDirFrom * outDir;
	if(dotProduct > 0) {	// this is Lafortune's modified Phong
		double scale = pow(dotProduct, shine) * (shine + 2.0) / (2.0*M_PI);
		return scale * specularAlbedo + diffuseAlbedo / M_PI;
	} else
		return diffuseAlbedo / M_PI;
}

// randomly generate the 'out' direction, and give back the probability of the generation in 'pdf'
// the 'in' points TO the surface, the out will points From the surface
//-----------------------------------------------------------------
inline bool	Material::SampleOutdir(const Vector& in, const Vector& normal, double rand1, double rand2, double rand3, 
								   Vector* out, double* pdf)
{	
	if (IsMirror()) {	// if mirror surface, treat in another way
		*out = in + -2.0 * (normal * in) * normal;
		//*pdf = 1.0;
		*pdf = *out * normal; // cosTheta
		return true;
	}

	*pdf = 0.0;
	if (rand1 > diffuseAlbedo.Lum() + specularAlbedo.Lum())	// if the photon was absorbed
		return false;

	Vector idealReflDirFrom = in + -2.0 * (normal * in) * normal; // 'in' points TO the surface
	double	diffDirPdf, specDirPdf;
	if (rand1 < diffuseAlbedo.Lum()) {	// diffuse direction generation is selected
		SampleHemisphereCosTheta(normal, rand2, rand3, out, &diffDirPdf);
		float cosIdeal = *out * idealReflDirFrom;
		specDirPdf = (cosIdeal > 0)?  pow(cosIdeal, shine) * (shine + 1.0) / (2.0*M_PI) : 0.0;
	} else {							// specular direction generation is selected
		SampleHemisphereCosNTheta(idealReflDirFrom, shine, (shine + 1.0) / (2.0*M_PI), rand2, rand3, out, &specDirPdf);
		float cosTheta = *out * normal;
		if(cosTheta <= 0) {				//sampled specular dir is in the back side of the poligon
			*pdf	= 0.0;
			return false;
		}
		diffDirPdf = cosTheta / M_PI;
	}

	*pdf = (diffuseAlbedo.Lum() * diffDirPdf + specularAlbedo.Lum() * specDirPdf);
	return true;
}

// helper function for setting up an orthongonal coordinate system, 
// where the Z corresponds to 'targetZ' direction
//-----------------------------------------------------------------
inline void Material::SetCoordSystem(const Vector& targetZ, Vector* X, Vector* Y, Vector* Z) 
{
	*Z = targetZ;

	double dx = fabs (targetZ.x);
	double dy = fabs (targetZ.y);
	double dz = fabs (targetZ.z);
	DominantAxis dominantAxis = (dx > dy) ? ((dx > dz) ? X_DOMINANT : Z_DOMINANT) : ((dy > dz) ? Y_DOMINANT : Z_DOMINANT);
	Vector axis;
	if (dominantAxis == Z_DOMINANT) 
		axis.Set (0.0, 1.0, 0.0);
	else 
		axis.Set (0.0, 0.0, 1.0);

	*X = axis % *Z;
	X->Normalize();
	*Y = *Z % *X;
	Y->Normalize ();
}

// find an 'outDir' direction, where the probability distribution is 
// the cosine of the angle between the direction vector and the surface normal
// this corresponds to diffuse direction generation
//-----------------------------------------------------------------
inline void Material::SampleHemisphereCosTheta(const Vector& normal, double rand1, double rand2, Vector* outDir, double* pdf)
{
	Vector X, Y, Z; 
	SetCoordSystem(normal, &X, &Y, &Z);

	double phi			= 2.0 * M_PI * rand1;
	double xi_2			= rand2;
	double cos_phi		= cos(phi);
	double sin_phi		= sin(phi);
	double cos_theta	= sqrt(xi_2);
	double sin_theta	= sqrt(1.0 - xi_2);

	*outDir = (sin_theta * cos_phi) * X + (sin_theta * sin_phi) * Y + cos_theta * Z;
	*pdf = cos_theta / M_PI;
}

// find an 'outDir' direction, where the probability distribution is 
// cos^n(Theta) distribution with its modus at the ideal reflection direction
// this corresponds to specular direction generation
//-----------------------------------------------------------------
inline void Material::SampleHemisphereCosNTheta(const Vector& idealReflDir, double n, double specularPdfScaling, double rand1, double rand2, Vector* outDir, double* pdf)
{
	Vector X, Y, Z; 
	SetCoordSystem(idealReflDir, &X, &Y, &Z);

	float phi			= 2. * M_PI * rand1;
	float cos_phi		= cos (phi);
	float sin_phi		= sin (phi);
	float cos_theta		= pow (rand2, 1.0 / (n + 1));
	float sin_theta		= sqrt (1.0 - cos_theta * cos_theta);

	*outDir = (sin_theta * cos_phi) * X + (sin_theta * sin_phi) * Y + cos_theta * Z;
	*pdf = pow(cos_theta, n) * specularPdfScaling;
}
