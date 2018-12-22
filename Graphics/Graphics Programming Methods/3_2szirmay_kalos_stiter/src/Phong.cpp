#include "Phong.hpp"

/*!
@param in points TO the surface
@param out points FROM the surface
@param normal Normal of the surface at the point
@param dotNormalOut dot product of (Normal, Out) ; precalculated value
@param dotNormalIn  dot product of (Normal, In) ;precalculated value
*/
Color CPhongBrdf::EvalBrdf (const Vector& in, const Vector& out, const Vector& normal, 
						  const float dotNormalIn, const float dotNormalOut)
{
	if (dotNormalOut < 0) {
		return Color::BLACK;
	}

// add diffuse part
	Color result = kd;	

// add specular
	Vector idealReflected;
	idealReflected.SetIdealReflectedDirection (in, normal, dotNormalIn);
	float dotProduct = idealReflected*out;
    if(dotProduct > 0) {	// this is Lafortune's modified Phong
		float scale = powf (dotProduct, Ns) * specularScaling;

		//MAXPHONG 
		float nDotIn = - dotNormalIn;

		if(dotNormalOut > nDotIn) scale /= dotNormalOut;
		else scale /= nDotIn;

		result+=Ks*scale;
    }

	return result;
}

/*!
@param in points TO the surface
@param out points FROM the surface
@param normal Normal of the surface at the point
@param dotNormalOut dot product of (Normal, Out) ; precalculated value
*/
Color CPhongBrdf::EvalDiffuseBrdf (const Vector& in, const Vector& out, const Vector& normal, const float dotNormalOut)
{
	if (dotNormalOut < 0) return Color::BLACK;
	else return kd;	// add diffuse part
}

/*!
@param in points TO the surface
@param out points FROM the surface
@param normal Normal of the surface at the point
@param dotNormalOut dot product of (Normal, Out) ; precalculated value
@param dotNormalIn  dot product of (Normal, In) ;precalculated value
*/
Color CPhongBrdf::EvalSpecularBrdf (const Vector& in, const Vector& out, const Vector& normal, 
								  const float dotNormalIn, const float dotNormalOut)
{
	if (dotNormalOut < 0) return Color::BLACK;

// add specular
	Vector idealReflected;
	idealReflected.SetIdealReflectedDirection (in, normal, dotNormalIn);
	float dotProduct = idealReflected*out;
    if(dotProduct > 0) {	// this is Lafortune's modified Phong
		float scale = powf (dotProduct, Ns) * specularScaling;

		//MAXPHONG 
		float nDotIn = - dotNormalIn;

		if(dotNormalOut > nDotIn) scale /= dotNormalOut ;
		else scale /= nDotIn;

		Vector outIdealMirror;
		outIdealMirror.SetIdealReflectedDirection(-out, idealReflected);
		//if ( (outIdealMirror * normal)<0) 
		//	scale*=2;
		return Ks*scale;
	} else {
		return Color::BLACK;
	}
}


Color CPhongBrdf::CalculateMaxPhongSpecularAlbedo(const Vector& outDir, const Vector& normal)
{	
	Color albedo = Color::BLACK;
	long nSample = 2000;
	Vector inDir;
	for (long i = 0; i < nSample; i++) {
		inDir.GenerateRandomTransillumDir();
		float cosIn = inDir * normal;	//.DotProduct(normal);
		if (cosIn > 0) {
			inDir *= -1.0f;
		} else {
			cosIn *= -1.0;
		}

		Color fx = EvalSpecularBrdf(inDir, outDir, normal);

		// in points TO the surface
		albedo += fx * (cosIn * PI2);
	}
	albedo *= (1.0f / nSample);

	//Color err=albedo-Ks;
	//float error=(fabsf(err.r)+fabsf(err.g)+fabsf(err.b))/3.0f;
	return albedo;
}


Color CPhongBrdf::ComputedSpecularMaxPhongAlbedo(const Vector& out, const Vector& normal) {
	float cosOut		= out * normal; //.DotProduct(normal);
	float outRadian		= acosf(cosOut);
	short index			= (short) (outRadian / (M_PI/2.0f)* SpecularAlbedosNum);
//	assert(index >= 0 && index < SpecularAlbedosNum);

	return maxPhongSpecularAlbedos[index];
	//return Ks;
}
