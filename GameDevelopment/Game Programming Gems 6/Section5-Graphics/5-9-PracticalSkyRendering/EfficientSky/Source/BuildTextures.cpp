// BuildTextures.cpp: Build any pre-computed textures.
// Date: 08/04/05
// Created by: Aurelio Reis

#include "dxstdafx.h"
#include <ctime>


//////////////////////////////////////////////////////////////////////////////////////////
// Irradiance Map Functions (from Real-Time Computation of Dynamic Irradiance
// Environment Maps [Gary King, GPU Gems 2, 2005]).
//////////////////////////////////////////////////////////////////////////////////////////

double factorial(unsigned int y)
{
    const double table[16] = { 1., 1., 2., 6., 24., 120., 720., 5040., 40320., 362880., 
        3628800., 39916800., 479001600., 6227020800., 87178291200., 1307674368000. };
   
    double result = table[(y>15)?15:y];

    while (y>=16)
    {
        result = result * double(y);
        y--;
    }

    return result;
}

void CubeCoord( D3DXVECTOR3* vec, int face, const D3DXVECTOR2* uv )
{
    D3DXVECTOR3 tmp;
    switch (face)
    {
    case 0: tmp = D3DXVECTOR3(   1.f, -uv->y, -uv->x); break;
    case 1: tmp = D3DXVECTOR3(  -1.f, -uv->y,  uv->x); break;
    case 2: tmp = D3DXVECTOR3( uv->x,    1.f,  uv->y); break;
    case 3: tmp = D3DXVECTOR3( uv->x,   -1.f, -uv->y); break;
    case 4: tmp = D3DXVECTOR3( uv->x, -uv->y,    1.f); break;
    case 5: tmp = D3DXVECTOR3(-uv->x, -uv->y,   -1.f); break;
    }
    D3DXVec3Normalize(&tmp, &tmp);
    vec->x =  tmp.z;
    vec->y = -tmp.x;
    vec->z =  tmp.y;
    //*vec = tmp;
}

bool ParaboloidCoord( D3DXVECTOR3* vec, int face, const D3DXVECTOR2* uv )
{
    //  sphere direction is the reflection of the orthographic view direction (determined by
    //  face), reflected about the normal to the paraboloid at uv
    double r_sqr = uv->x*uv->x + uv->y*uv->y;
    
    if (r_sqr > 1.)
        return false;

    D3DXVECTOR3 axis;
    if (face==0)
        axis = D3DXVECTOR3(0.f, 0.f, -1.f);
    else
        axis = D3DXVECTOR3(0.f, 0.f, 1.f);

    // compute normal on the parabaloid at uv
    D3DXVECTOR3 N( uv->x, uv->y, 1.f );
    D3DXVec3Normalize(&N, &N);

    // reflect axis around N, to compute sphere direction
    float v_dot_n = D3DXVec3Dot(&axis, &N);
    D3DXVECTOR3 R = axis - 2*v_dot_n*N;
    
    *vec = R;
    return true;
}


struct SH_Reflectance_Al_Evaluator
{
    virtual double operator()(int l) const = 0;
};

struct Lambert_Al_Evaluator: public SH_Reflectance_Al_Evaluator
{
    virtual double operator()(int l) const;
};

struct Phong_Al_Evaluator: public SH_Reflectance_Al_Evaluator
{
    Phong_Al_Evaluator( double spec ): m_specular(spec) { }
    virtual double operator()(int l) const;

protected:
    Phong_Al_Evaluator(): m_specular(1.f) { }
    double m_specular;
};


//  this is a direct evaluation of the spherical harmonic basis functions for
//  lambertian diffuse, taken from Hanrahan & Ramamoorthi. "An Efficient 
//  Representation for Irradiance Environment Maps" (SIGGRAPH 2001)
double Lambert_Al_Evaluator::operator ()( int l ) const
{
    if (l<0)       // bogus case
        return 0.;

    if ( (l&1)==1 )
    {
        if (l==1) 
            return 2.*D3DX_PI / 3.;
        else 
            return 0.;
    }
    else  // l is even
    {
        double l_fac = factorial((unsigned int)l);
        double l_over2_fac = factorial((unsigned int)(l>>1));
        double denominator = (l+2)*(l-1);
        double sign = ( (l>>1) & 1 )?1.f : -1.f;  // -1^(l/2 - 1) = -1 when l is a multiple of 4, 1 for other multiples of 2
        double exp2_l = (1 << (unsigned int)l);
        return (sign*2.*D3DX_PI*l_fac) / (denominator*exp2_l*l_over2_fac);
    }
}

//  this is a direct evaluation of the spherical harmonic basis functions for
//  a phong reflector with exponent s, taken from Hanrahan & Ramamoorthi. 
//  "A Signal-Processing Framework for Inverse Rendering" (SIGGRAPH 2001)
//  this is an approximation to the actual coefficients, good when s >> l
//  don't compute 10th order SH for (R.L)^5 with this...
double Phong_Al_Evaluator::operator ()( int l ) const
{
    if ( l<0 ) return 0;

    double lambda = sqrt(4*D3DX_PI/(2.*double(l)+1.));
    double x = -double(l*l) / (2.*m_specular);
    return exp(x) / lambda;
}

static BOOL IsPow2(DWORD i) { return ((i&(i-1))==0); }
static DWORD NextPow2(DWORD i) { DWORD d=0x1; while (d<i) d<<=1; return d; }

//  builds the textures required for phase 2, using an arbitrary BRDF.
//  individual bands are saved as subrects in a larger texture, so that only 6 textures are
//  generated.  6 textures are generated, 1 for each cube face.
//  resulting environment map is parameterized based on the normal
HRESULT BuildIrradianceTextures(LPDIRECT3DTEXTURE9 *weightTextures, LPDIRECT3DDEVICE9 lpDevice, DWORD dwOrder, DWORD dwSize, const SH_Reflectance_Al_Evaluator& AlEvaluator)
{
    if (!weightTextures || !IsPow2(dwSize))
        return E_FAIL;

    DWORD nSize = NextPow2( dwOrder );
    float* basisProj = new float[dwOrder*dwOrder];

    for (int face=0; face<6; face++)
    {
        //  allocate the texture, lock the surface
        //  use an R32F texture to store the coefficients
        if (FAILED(lpDevice->CreateTexture(dwSize*nSize, dwSize*nSize, 1, 0, D3DFMT_R32F, D3DPOOL_SCRATCH, &weightTextures[face], NULL)))
            return E_FAIL;

        float* coefficients;
        coefficients = new float[dwSize*nSize * dwSize*nSize];
        ZeroMemory(coefficients, dwSize*dwSize*nSize*nSize*sizeof(float));

        for (unsigned int t=0; t<dwSize; t++)
        {
            for (unsigned int s=0; s<dwSize; s++)
            {
                D3DXVECTOR3 cubeVec;
                double sd=((s+0.5)/double(dwSize))*2. - 1.;
                double td=((t+0.5)/double(dwSize))*2. - 1.;
                D3DXVECTOR2 stVec ( (float)sd, (float)td );

                CubeCoord(&cubeVec, face, &stVec);

                //  compute the N^2 spherical harmonic basis functions
                float* basisProj = new float[dwOrder*dwOrder];
                D3DXSHEvalDirection( basisProj, dwOrder, &cubeVec );

                int basis=0;
                for (int l=0; l<(int)dwOrder; l++)
                {
                    double Al = AlEvaluator(l);
                    for (int m=-l; m<=l; m++, basis++)
                    {
                        int tiley = basis / dwOrder;
                        int tilex = basis % dwOrder;
                        double Ylm = basisProj[l*(l+1) + m];
                        
                        int offset = ((tiley*dwSize+t)*dwSize*nSize) + tilex*dwSize+s;
                        coefficients[offset] = (float)(Al * Ylm);
                    }
                }
            }
        }
        
        D3DLOCKED_RECT lockRect;
        if (FAILED(weightTextures[face]->LockRect(0, &lockRect, NULL, 0)))
            return E_FAIL;
        unsigned char* dst = (unsigned char*)lockRect.pBits;
        unsigned char* src = (unsigned char*)coefficients;
        unsigned int  srcPitch = dwSize * nSize * sizeof(float);

        for ( UINT i=0; i<dwSize*nSize; i++, dst+=lockRect.Pitch, src+=srcPitch )
            memcpy(dst, src, srcPitch);
        
        weightTextures[face]->UnlockRect(0);
        delete [] coefficients;
    }    
    
    delete [] basisProj;
    return S_OK;
}

HRESULT BuildLambertIrradianceTextures(LPDIRECT3DTEXTURE9 *weightTextures, LPDIRECT3DDEVICE9 lpDevice, DWORD dwOrder, DWORD dwSize)
{
    return BuildIrradianceTextures( weightTextures, lpDevice, dwOrder, dwSize, Lambert_Al_Evaluator() );
}

HRESULT BuildPhongIrradianceTextures(LPDIRECT3DTEXTURE9 *weightTextures, LPDIRECT3DDEVICE9 lpDevice, DWORD dwOrder, DWORD dwSize, FLOAT specular)
{
    return BuildIrradianceTextures( weightTextures, lpDevice, dwOrder, dwSize, Phong_Al_Evaluator(specular) );
}


//  These two functions compute weight coefficients for each texel in a rendered image.
//  iterate over the texels, summing weight*color, and the end result will be a spherical harmonic
//  coefficient.  to compute 16 coefficients, render to a 4x4 image (there will be subrects in the
//  weight texture corresponding to each coefficient)
HRESULT BuildDualParaboloidWeightTextures(LPDIRECT3DTEXTURE9 *weightTextures, LPDIRECT3DDEVICE9 lpDevice, DWORD dwOrder, DWORD dwSize)
{
    if (!weightTextures || !IsPow2(dwSize))
        return E_FAIL;

    DWORD nSize = NextPow2( dwOrder );

    //  texels need to be weighted by solid angle
    //  compute differential solid angle at texel center (cos(theta)/r^2), and then normalize and scale by 4*PI
    double *d_omega;
    double sum_d_omega = 0.f;
    d_omega = new double[dwSize*dwSize];

    unsigned int s, t;
    //  paraboloids are symmetrical, so compute total diff. solid angle for one half, and double it.
    for (t=0; t<dwSize; t++)
    {
        for (s=0; s<dwSize; s++)
        {
            double x=((s+0.5)/double(dwSize))*2. - 1.;
            double y=((t+0.5)/double(dwSize))*2. - 1.;
            double r_sqr = x*x + y*y;

            int index = t*dwSize + s;
            if ( r_sqr > 1. )  // only count points inside the circle
            {
                d_omega[index] = 0.;
                continue;
            }

            double z = 0.5*(1. - r_sqr);
            double mag = sqrt(r_sqr + z*z);  // =0.5[1+(x*x + y*y)]

            double cosTheta = 1.;  // cos(theta) terms fall out, since dA is first projected 
                                   // orthographically onto the paraboloid ( dA' = dA / dot(zAxis, Np) ), then reflected
                                   // and projected onto the unit sphere (dA'' = dA' dot(R,Np) / len^2)
                                   // dot(zAxis, Np) == dot(R, Np), so dA'' = dA / len^2
            d_omega[index] = cosTheta / (mag*mag);  //  = 1 / (mag^2)
            sum_d_omega += d_omega[index];
        }
    }

    double d_omega_scale = 4.*D3DX_PI / (2.f*sum_d_omega);

    float* basisProj = new float[dwOrder*dwOrder];

    for (int face=0; face<2; face++)
    {
        //  allocate the texture, lock the surface
        //  use an R32F texture to store the coefficients
        if (FAILED(lpDevice->CreateTexture(dwSize*nSize, dwSize*nSize, 1, 0, D3DFMT_R32F, D3DPOOL_SCRATCH, &weightTextures[face], NULL)))
            return E_FAIL;

        float *coefficients;
        coefficients = new float[dwSize*nSize * dwSize*nSize];
        ZeroMemory(coefficients, dwSize*dwSize*nSize*nSize*sizeof(float));

        for (t=0; t<dwSize; t++)
        {
            for (s=0; s<dwSize; s++)
            {
                D3DXVECTOR3 parabVec;
                double sd=((s+0.5)/double(dwSize))*2. - 1.;
                double td=((t+0.5)/double(dwSize))*2. - 1.;
                D3DXVECTOR2 stVec ( (float)sd, (float)td );

                if (!ParaboloidCoord(&parabVec, face, &stVec))
                    continue;   //  skip if this texel is outside the paraboloid


                //  compute the N^2 spherical harmonic basis functions
                D3DXSHEvalDirection(basisProj, dwOrder, &parabVec);

                int basis=0;
                int index = t*dwSize + s;
                for (int l=0; l<(int)dwOrder; l++)
                {
                    for (int m=-l; m<=l; m++, basis++)
                    {
                        int tiley = basis / dwOrder;
                        int tilex = basis % dwOrder;
                        double Ylm = basisProj[l*(l+1) + m];
                        
                        int offset = ((tiley*dwSize+t)*dwSize*nSize) + tilex*dwSize+s;
                        float weight = (float)(Ylm * d_omega[index] * d_omega_scale);
                        coefficients[offset] = weight;
                    }
                }
            }
        }

        D3DLOCKED_RECT lockRect;
        if (FAILED(weightTextures[face]->LockRect(0, &lockRect, NULL, 0)))
            return E_FAIL;
        unsigned char* dst = (unsigned char*)lockRect.pBits;
        unsigned char* src = (unsigned char*)coefficients;
        unsigned int  srcPitch = dwSize * nSize * sizeof(float);

        for ( UINT i=0; i<dwSize*nSize; i++, dst+=lockRect.Pitch, src+=srcPitch )
            memcpy(dst, src, srcPitch);

        weightTextures[face]->UnlockRect(0);
        delete [] coefficients;
    }

    delete [] basisProj;
    delete [] d_omega;
    return S_OK;
}

HRESULT BuildCubemapWeightTextures(LPDIRECT3DTEXTURE9 *weightTextures, LPDIRECT3DDEVICE9 lpDevice, DWORD dwOrder, DWORD dwSize)
{
    if (!weightTextures || !IsPow2(dwSize))
        return E_FAIL;

    DWORD nSize = NextPow2( dwOrder );

    // compute the total differential solid angle for samples over the dwSize*dwSize cube.  this is
    // used as a normalization scale factor for the individual solid angles...
    double *d_omega;
    double sum_d_omega = 0.f; 
    d_omega = new double[dwSize*dwSize];
    float* basisProj = new float[dwOrder*dwOrder];

    unsigned int s, t;

    //  faces are symmetrical, so just compute d_omega for one face, and replicate 6 times.
    for (t=0; t<dwSize; t++)
    {
        for (s=0; s<dwSize; s++)
        {
            D3DXVECTOR3 cubeVec;
            double sd=((s+0.5)/double(dwSize))*2. - 1.;
            double td=((t+0.5)/double(dwSize))*2. - 1.;
            D3DXVECTOR2 stVec ( (float)sd, (float)td );
            int index = t*dwSize + s;
            double r_sqr = sd*sd+td*td+1;        // distance origin to texel
            double cos_theta = 1./sqrt(r_sqr);   // dot product between cube vector (sphere direction)
                                                 // and surface normal (axis direction)
            d_omega[index] = cos_theta / r_sqr;  // =(r^-3/2)
            sum_d_omega += d_omega[index];
        }
    }

    double d_omega_scale = 4.*D3DX_PI / (6.f*sum_d_omega);

    for (int face=0; face<6; face++)
    {
        //  allocate the texture, lock the surface
        //  use an R32F texture to store the coefficients
        if (FAILED(lpDevice->CreateTexture(dwSize*nSize, dwSize*nSize, 1, 0, D3DFMT_R32F, D3DPOOL_SCRATCH, &weightTextures[face], NULL)))
            return E_FAIL;

        float* coefficients;
        coefficients = new float[dwSize*nSize * dwSize*nSize];
        ZeroMemory(coefficients, dwSize*dwSize*nSize*nSize*sizeof(float));

        for (t=0; t<dwSize; t++)
        {
            for (s=0; s<dwSize; s++)
            {
                D3DXVECTOR3 cubeVec;
                double sd=((s+0.5)/double(dwSize))*2. - 1.;
                double td=((t+0.5)/double(dwSize))*2. - 1.;
                D3DXVECTOR2 stVec ( (float)sd, (float)td );

                CubeCoord(&cubeVec, face, &stVec);

                int index = t*dwSize + s;

                //  compute the N^2 spherical harmonic basis functions
                D3DXSHEvalDirection( basisProj, dwOrder, &cubeVec );

                int basis=0;
                for (int l=0; l<(int)dwOrder; l++)
                {
                    for (int m=-l; m<=l; m++, basis++)
                    {
                        int tiley = basis / dwOrder;
                        int tilex = basis % dwOrder;
                        double Ylm = basisProj[l*(l+1)+m];
                        
                        int offset = ((tiley*dwSize+t)*dwSize*nSize) + tilex*dwSize+s;
                        coefficients[offset] = (float)(Ylm * d_omega[index] * d_omega_scale);
                    }
                }
            }
        }

        D3DLOCKED_RECT lockRect;
        if (FAILED(weightTextures[face]->LockRect(0, &lockRect, NULL, 0)))
            return E_FAIL;
        unsigned char* dst = (unsigned char*)lockRect.pBits;
        unsigned char* src = (unsigned char*)coefficients;
        unsigned int  srcPitch = dwSize * nSize * sizeof(float);

        for ( UINT i=0; i<dwSize*nSize; i++, dst+=lockRect.Pitch, src+=srcPitch )
            memcpy(dst, src, srcPitch);

        weightTextures[face]->UnlockRect(0);
        delete [] coefficients;
    }
    
    delete [] basisProj;
    delete [] d_omega;

    return S_OK;
}


// Fill an F Texture pixel.
VOID WINAPI Fill_FTexture( D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
						const D3DXVECTOR2* pTexelSize, LPVOID pData )
{
	double fSum = 0.0;
	double a = 0.0;
	double u = pTexCoord->y * 2.0f, v = pTexCoord->x * 1.57f;
	// Approximate the integral with 1000 slices.
	double dz = v / 1000.0;
	double z;

	// Step through the integral.
	for ( z = a; z < v; z += dz )
	{
		fSum += exp( -u * tan( z ) ) * dz;
	}

	*pOut = D3DXVECTOR4( (float)fSum, (float)fSum, (float)fSum, (float)fSum );
}


const int g_iFTextureSize = 512;


void SaveFTexture()
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	if ( FAILED( D3DXCreateTexture( g_pd3dDevice, g_iFTextureSize, g_iFTextureSize, 1,
									0, D3DFMT_R32F, D3DPOOL_SCRATCH, &pTexture ) ) )
	{
		return;
	}

	if ( FAILED( D3DXFillTexture( pTexture, Fill_FTexture, NULL ) ) )
	{
		return;
	}

	// Save out the file.
	D3DXSaveTextureToFile( L"media/lookups/F.dds", D3DXIFF_DDS, pTexture, NULL );

	SAFE_RELEASE( pTexture );
}


__forceinline float RandFloat( float fLow, float fHigh ) { return fLow + ( fHigh - fLow ) * ( (float)rand() ) / RAND_MAX; }

// Fill an F Texture pixel.
VOID WINAPI Fill_StarCube( D3DXVECTOR4* pOut, const D3DXVECTOR3* pTexCoord, 
						const D3DXVECTOR3* pTexelSize, LPVOID pData )
{
	float fRand = RandFloat( 0.0f, 1.0f );
	float fIntensity = min( 1.0f, max( 0.0f, fRand - 0.985f ) * 26.0f );
	*pOut = D3DXVECTOR4( fIntensity, fIntensity, fIntensity, 1.0f );
}


const int g_iStarCubeSize = 1024;


// Build the Star Cube Map.
void BuildStarCubeMap()
{
	srand( (unsigned int)time( NULL ) );

	LPDIRECT3DCUBETEXTURE9 pTexture = NULL;
	if ( FAILED( D3DXCreateCubeTexture( g_pd3dDevice, g_iStarCubeSize, 1, D3DX_DEFAULT,
										D3DFMT_DXT1, D3DPOOL_SCRATCH, &pTexture ) ) )
	{
		return;
	}

	// NOTE: This is a very rough implementation. A better way to do it would be to render a scene
	// to the cubemap with the stars drawn in the correct astral position, i.e. from using
	// actual bsc information.
	if ( FAILED( D3DXFillCubeTexture( pTexture, Fill_StarCube, NULL ) ) )
	{
		return;
	}

	// Save out the file.
	D3DXSaveTextureToFile( L"media/StarCube.dds", D3DXIFF_DDS, pTexture, NULL );

	SAFE_RELEASE( pTexture );
}

//#define BUILD_IRRADIANCE_LOOKUPS
//#define BUILD_F_LOOKUP
//#define BUILD_STARCUBE

// Build all the pre-computed lookup textures and save them out.
void BuildLookupsAndSave()
{
#ifdef BUILD_IRRADIANCE_LOOKUPS
#if 1
	// SH basis functions for paraboloid map
	LPDIRECT3DTEXTURE9 pParaboloidSHWeights[ 2 ] = { NULL, NULL };
	BuildDualParaboloidWeightTextures( pParaboloidSHWeights, g_pd3dDevice, NUM_ORDER, NUM_RADIANCE_SAMPLES );

	for ( int i = 0; i < 2; i++ )
	{
		LPDIRECT3DSURFACE9 pSrcSurface;
		pParaboloidSHWeights[ i ]->GetSurfaceLevel( 0, &pSrcSurface );
		D3DXSaveSurfaceToFileA( VarArg( "media/lookups/ParaboloidSHWeights%d.dds", i ), D3DXIFF_DDS, pSrcSurface, NULL, NULL );
		SAFE_RELEASE( pParaboloidSHWeights[ i ] );
	}
#endif

#if 1
	LPDIRECT3DTEXTURE9 pLambertSHEval[ 6 ] = { NULL, NULL, NULL, NULL, NULL, NULL };
	BuildLambertIrradianceTextures( pLambertSHEval, g_pd3dDevice, NUM_ORDER, NUM_RESULT_SAMPLES );

	for ( int i = 0; i < 6; i++ )
	{
		LPDIRECT3DSURFACE9 pSrcSurface;
		pLambertSHEval[ i ]->GetSurfaceLevel( 0, &pSrcSurface );
		D3DXSaveSurfaceToFileA( VarArg( "media/lookups/LambertSHEval%d.dds", i ), D3DXIFF_DDS, pSrcSurface, NULL, NULL );
		SAFE_RELEASE( pLambertSHEval[ i ] );
	}
#endif

#if 1
	const float fSpecExponent = 9.0f;
	LPDIRECT3DTEXTURE9 pPhongSHEval[ 6 ] = { NULL, NULL, NULL, NULL, NULL, NULL };
	BuildPhongIrradianceTextures( pPhongSHEval, g_pd3dDevice, NUM_ORDER, NUM_RESULT_SAMPLES, fSpecExponent );

	for ( int i = 0; i < 6; i++ )
	{
		LPDIRECT3DSURFACE9 pSrcSurface;
		pPhongSHEval[ i ]->GetSurfaceLevel( 0, &pSrcSurface );
		D3DXSaveSurfaceToFileA( VarArg( "media/lookups/SpecularSHEval%d.dds", i ), D3DXIFF_DDS, pSrcSurface, NULL, NULL );
		SAFE_RELEASE( pPhongSHEval[ i ] );
	}
#endif
#endif

#ifdef BUILD_F_LOOKUP
	SaveFTexture();
#endif

#ifdef BUILD_STARCUBE
	BuildStarCubeMap();
#endif
}