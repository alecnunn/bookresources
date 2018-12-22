//
// Test file for fast quaternion interpolation methods
//
// Andy Thomason (athomason@acm.org)
//
// This code has been tested on Win32, but should work on most common
// compilers and platforms.
//
// Call SlerpApprox::Test() to run tests.
//
// Tests
//
#ifdef WIN32
	#include <windows.h>
	#include <mm3dnow.h>
#endif

#include <math.h>
#include <iostream>

#ifdef WIN32
	namespace PlatformSpecific
	{
		typedef __int64 Time;

		Time GetTime()
		{
			Time result;
			::QueryPerformanceCounter( (LARGE_INTEGER *)&result );
			return result;
		}
		double TimeToSecs( Time time )
		{
			Time frequency;
			::QueryPerformanceFrequency( (LARGE_INTEGER *)&frequency );
			return (double)time / (double)frequency;
		}
	}
#endif

namespace SlerpApprox
{
	using namespace PlatformSpecific;
	
	// very basic quaternion for testing
	struct Quat
	{
		float X;
		float Y;
		float Z;
		float W;
		Quat() {}
		Quat( const float &x, const float &y, const float &z, const float &w ) { X = x; Y = y; Z = z; W = w; }
	};
	
	
	// functions for C float type
	float Float( float x ) { return x; }
	float Abs( float x ) { return fabsf( x ); }
	float Sqrt( float x ) { return sqrtf( x ); } // use a native instruction here
	float RecipSqrt( float x ) { return 1.0f / sqrtf( x ); } // use a native instruction here
	float Recip( float x ) { return 1.0f / x; }
	float Max( float x, float y ) { return ( x + y + fabsf( x - y ) ) * 0.5f; }
	float Min( float x, float y ) { return ( x + y - fabsf( x - y ) ) * 0.5f; }
	float Arccos( float x ) { return acosf( x ); }
	float Sin( float x ) { return sinf( x ); }
	float ArccosFast( float x ) { return Sqrt( 2.218480716f - 2.441884385f * x + .2234036692f * x * x )  - Sqrt( 2.218480716f + 2.441884385f * x + .2234036692f * x * x ) + 1.570796327f + .6391287330f * x; }
	float SinFast( float x ) { return -.67044e-5f + ( 1.000271283f + (  -.17990919e-2f + (  -.1621365372f + (  -.556099983e-2f + ( .1198086481e-1f - .1271209213e-2f * x ) * x ) * x ) * x ) * x ) * x; }
	
	// functions for testing "Squad"
	Quat MulScalar( float a, const Quat &b ) { return Quat( a * b.X, a * b.Y, a * b.Z, a * b.W ); }
	Quat Conj( const Quat &a ) { return Quat(  - a.X,  - a.Y,  - a.Z, a.W ); }
	Quat Mul( const Quat &a, const Quat &b ) { return Quat( a.W*b.X+a.X*b.W - a.Y*b.Z+a.Z*b.Y, a.W*b.Y+a.Y*b.W - a.Z*b.X+a.X*b.Z, a.W*b.Z+a.Z*b.W - a.X*b.Y+a.Y*b.X, a.W*b.W - a.X*b.X - a.Y*b.Y - a.Z*b.Z ); }
	Quat Add( const Quat &a, const Quat &b ) { return Quat( a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W ); }
	Quat Exp( const Quat &a ) { float modV = Max( 1e-10f, sqrtf(a.X*a.X+a.Y*a.Y+a.Z*a.Z) ); return Quat ( expf(a.W)*sinf(modV)/modV*a.X, expf(a.W)*sinf(modV)/modV*a.Y, expf(a.W)*sinf(modV)/modV*a.Z, expf(a.W)*cosf(modV) ); }
	Quat Log( const Quat &a ) { float modV = Max( 1e-10f, sqrtf(a.X*a.X+a.Y*a.Y+a.Z*a.Z) ); return Quat ( atan2f(modV,a.W)/modV*a.X, atan2f(modV,a.W)/modV*a.Y, atan2f(modV,a.W)/modV*a.Z, logf(sqrtf(a.X*a.X+a.Y*a.Y+a.Z*a.Z+a.W*a.W)) );}

	// Reference class, uses trig functions.
	// Note the use of Min to avoid a branch, and the loss of dozens
	// of clock cycles to pipeline stalls.
	// We clamp the dot product below 1.
	// If the dot product is -1, no interpolation is possible.
	class SlerpReference
	{
	public:
		SlerpReference( const Quat &a, const Quat &b ) : mA( a ), mB( b )
		{
			float adotb = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
			adotb = Min( adotb, 0.99999f );
			mTheta = acosf( adotb );
			mRecipSqrt = RecipSqrt( 1 - adotb * adotb );
		}
		
		Quat Interpolate( float t ) const
		{
			float alpha = sinf( ( 1 - t ) * mTheta ) * mRecipSqrt;
			float beta = sinf( t * mTheta ) * mRecipSqrt;
			return Quat( alpha * mA.X + beta * mB.X, alpha * mA.Y + beta * mB.Y, alpha * mA.Z + beta * mB.Z, alpha * mA.W + beta * mB.W );
		}
	private:
		float mTheta;
		float mRecipSqrt;
		const Quat &mA;
		const Quat &mB;
	};

	// Not bad, simple to understand and quite accurate
	// The arccos and sin can be cut down to improve speed over accuracy.
	class SlerpDirect
	{
	public:
		SlerpDirect( const Quat &a, const Quat &b ) : mA( a ), mB( b )
		{
			float adotb = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
			adotb = Min( adotb, 0.995f );
			float even = 2.218480716f + .2234036692f * adotb * adotb;
			float odd = 2.441884385f * adotb;
			mTheta = Sqrt( even - odd ) - Sqrt( even + odd ) + 1.570796327f + .6391287330f * adotb;
			mRecipSqrt = RecipSqrt( 1 - adotb * adotb );
		}
		
		Quat Interpolate( float t ) const
		{
			float A = ( 1 - t ) * mTheta - 1.570796327f; A = A * A;
			float B = t * mTheta - 1.570796327f; B = B * B;
			float sinA = .9999999535f+(-.4999990537f+(.4166358517e-1f+(-.1385370794e-2f+.2315401401e-4f*A)*A)*A)*A;
			float sinB = .9999999535f+(-.4999990537f+(.4166358517e-1f+(-.1385370794e-2f+.2315401401e-4f*B)*B)*B)*B;
			float alpha = sinA * mRecipSqrt;
			float beta = sinB * mRecipSqrt;
			return Quat( alpha * mA.X + beta * mB.X, alpha * mA.Y + beta * mB.Y, alpha * mA.Z + beta * mB.Z, alpha * mA.W + beta * mB.W );
		}
	private:
		float mTheta;
		float mRecipSqrt;
		const Quat &mA;
		const Quat &mB;
	};

	// The clear winner, has simple setup and interpolate and is accurate
	// to a good number of bits.
	class SlerpMatrix
	{
	public:
		SlerpMatrix( const Quat &a, const Quat &b ) : mA( a ), mB( b )
		{
			float adotb = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
			mRecipOnePlusAdotB = Recip( 1 + adotb );
			mC1 = 1.570994357f+(.5642929859f+( -.1783657717f+.4319949352e-1f*adotb)*adotb)*adotb;
			mC3 = -.6461396382f+(.5945657936f+(.8610323953e-1f -.3465122928e-1f*adotb)*adotb)*adotb;
			mC5 = .7949823521e-1f+( -.1730436931f+(.1079279599f -.1439397801e-1f*adotb)*adotb)*adotb;
			mC7 = -.4354102836e-2f+(.1418962736e-1f+( -.1567189691e-1f+.5848706227e-2f*adotb)*adotb)*adotb;
		}
		
		Quat Interpolate( float t ) const
		{
			float T = 1 - t, t2 = t * t, T2 = T * T;
			float alpha = (mC1+(mC3+(mC5+mC7*T2)*T2)*T2)*T * mRecipOnePlusAdotB;
			float beta = (mC1+(mC3+(mC5+mC7*t2)*t2)*t2)*t * mRecipOnePlusAdotB;
			return Quat( alpha * mA.X + beta * mB.X, alpha * mA.Y + beta * mB.Y, alpha * mA.Z + beta * mB.Z, alpha * mA.W + beta * mB.W );
		}
	private:
		float mRecipOnePlusAdotB;
		float mC1, mC3, mC5, mC7;
		const Quat &mA;
		const Quat &mB;
	};

	// Improvement on the simple lerp and renormalise, accurate but slow.
	class SlerpRenormal
	{
	public:
		SlerpRenormal( const Quat &a, const Quat &b ) : mA( a ), mB( b )
		{
			float adotb = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
			adotb = Min( adotb, 0.995f );
			float even = 2.218480716f + .2234036692f * adotb * adotb;
			float odd = 2.441884385f * adotb;
			mTheta = Sqrt( even - odd ) - Sqrt( even + odd ) + 1.570796327f + .6391287330f * adotb;
		}
		
		Quat Interpolate( float t ) const
		{
			float T = 1 - t, t2 = t * t, T2 = T * T;
			float A = ( 1 - t ) * mTheta;
			float B = t * mTheta;
			float sinA =  -.67044e-5f + ( 1.000271283f + (  -.17990919e-2f + (  -.1621365372f + (  -.556099983e-2f + ( .1198086481e-1f - .1271209213e-2f * A ) * A ) * A ) * A ) * A ) * A;
			float sinB =  -.67044e-5f + ( 1.000271283f + (  -.17990919e-2f + (  -.1621365372f + (  -.556099983e-2f + ( .1198086481e-1f - .1271209213e-2f * B ) * B ) * B ) * B ) * B ) * B;
			float recipAB = Recip( sinA + sinB );
			float alpha = sinA * recipAB;
			float beta = sinB * recipAB;

			// renormalise
			Quat result( alpha * mA.X + beta * mB.X, alpha * mA.Y + beta * mB.Y, alpha * mA.Z + beta * mB.Z, alpha * mA.W + beta * mB.W );
			float recip = RecipSqrt( result.X * result.X + result.Y * result.Y + result.Z * result.Z + result.W * result.W );
			return Quat( result.X * recip, result.Y * recip, result.Z * recip, result.W * recip );
		}
	private:
		float mTheta;
		const Quat &mA;
		const Quat &mB;
	};

	
	// If you have the luxury of error tolerence, use this!
	// Fine for fingers, toes and all small joints. accuracy 4-8 bits
	class SlerpSimpleRenormal
	{
	public:
		SlerpSimpleRenormal( const Quat &a, const Quat &b ) : mA( a ), mB( b )
		{
		}
		
		Quat Interpolate( float t ) const
		{
			float alpha = 1 - t;
			float beta = t;
			Quat result( alpha * mA.X + beta * mB.X, alpha * mA.Y + beta * mB.Y, alpha * mA.Z + beta * mB.Z, alpha * mA.W + beta * mB.W );
			float recip = RecipSqrt( result.X * result.X + result.Y * result.Y + result.Z * result.Z + result.W * result.W );
			return Quat( result.X * recip, result.Y * recip, result.Z * recip, result.W * recip );
		}
	private:
		const Quat &mA;
		const Quat &mB;
	};

	// Squad derivative calculation, traditional method for reference
	Quat DerivativeReference( const Quat &a, const Quat &b, const Quat &c )
	{
		return Mul(
			b, Exp(
				MulScalar(
					 - 1.0f/4,
					Add(
						Log( Mul( c, Conj( b ) ) ),
						Log( Mul( a, Conj( b ) ) )
					)
				)
			)
		);
	}

	// Spherical quadrangle derivative calculation, optimised by hand
	// This is quite fast and gives excellent results (~16 bits)
	// try to remove the branches implicit in the ? operators.
	Quat DerivativeCompact( const Quat &a, const Quat &b, const Quat &c )
	{
		Quat bconj = Conj( b );
		Quat arel = Mul( a, bconj );
		Quat crel = Mul( c, bconj );
		float aScale = arel.W > 0.9999f ? -0.25f : -0.25f*ArccosFast( arel.W ) * RecipSqrt( 1 - arel.W * arel.W );
		float cScale = crel.W > 0.9999f ? -0.25f : -0.25f*ArccosFast( crel.W ) * RecipSqrt( 1 - crel.W * crel.W );
		float logx = aScale * arel.X + cScale * crel.X;
		float logy = aScale * arel.Y + cScale * crel.Y;
		float logz = aScale * arel.Z + cScale * crel.Z;
		float length = sqrtf( logx * logx + logy * logy + logz * logz );
		float sinLength = SinFast( length );
		float cosLength = Sqrt( 1 - sinLength * sinLength );
		float xyzScale = length < 1e-5f ? 1 : sinLength / length;
		return Mul( b, Quat( xyzScale * logx, xyzScale * logy, xyzScale * logz, cosLength ) );
  }

	// Approximate squad derivative calculation, optimised by Maple
	// This one, seemingly good, lost out to the one above.
	Quat DerivativeSqrt( const Quat &a, const Quat &b, const Quat &c )
	{
			float t5 = 1+a.W*b.W+a.X*b.X+a.Y*b.Y+a.Z*b.Z;
			float t7 = .7071067810f/sqrtf(t5);
      float t9 = t5*t7+1;
      float t11 = .7071067810f/sqrtf(t9);
      float t12 = t9*t11;
      float t22 = 1+c.W*b.W+c.X*b.X+c.Y*b.Y+c.Z*b.Z;
      float t24 = .7071067810f/sqrtf(t22);
      float t27 = t22*t24+1;
      float t29 = .7071067810f/sqrtf(t27);
      float t30 = ( - c.W*b.X+c.X*b.W+c.Y*b.Z - c.Z*b.Y)*t24*t29;
      float t37 = ( - a.W*b.X+a.X*b.W+a.Y*b.Z - a.Z*b.Y)*t7;
      float t39 = t11*t27*t29;
      float t46 = ( - a.W*b.Y+a.Y*b.W+a.Z*b.X - a.X*b.Z)*t7;
      float t47 = t46*t11;
      float t54 = ( - c.W*b.Z+c.Z*b.W+c.X*b.Y - c.Y*b.X)*t24*t29;
      float t61 = ( - a.W*b.Z+a.Z*b.W+a.X*b.Y - a.Y*b.X)*t7;
      float t62 = t61*t11;
      float t69 = ( - c.W*b.Y+c.Y*b.W+c.Z*b.X - c.X*b.Z)*t24*t29;
      float t71 =  - t12*t30 - t37*t39+t47*t54 - t62*t69;
      float t75 = t37*t11;
      float t79 = t12*t27*t29 - t75*t30 - t47*t69 - t62*t54;
      float t85 =  - t12*t54 - t61*t39+t75*t69 - t47*t30;
      float t91 =  - t12*t69 - t46*t39+t62*t30 - t75*t54;
      return Quat(
				b.W*t71+b.X*t79 - b.Y*t85+b.Z*t91,b.W*t91+b.Y*t79 - b.Z*
				t71+b.X*t85,b.W*t85+b.Z*t79 - b.X*t91+b.Y*t71,b.W*t79 - b.X*t71 - b.Y*t91 - b
				.Z*t85
			);
	}

	// Generate some random quaternions. Do not use this in-game as it is far
	// from uniformly distributed.
	void GenerateQuats( int numQuats, Quat *quats, float maxTheta )
	{
		for( int i = 0; i < numQuats; ++i )
		{
			float x = (rand()&0xfff)*2/4095.0f - 1.0f;
			float y = (rand()&0xfff)*2/4095.0f - 1.0f;
			float z = (rand()&0xfff)*2/4095.0f - 1.0f;
			float normal = 1 / sqrtf( x * x + y * y + z * z );
			float theta = (rand()&0xfff)*maxTheta/4095.0f - maxTheta/2;
			float sinTheta = sinf( theta ), cosTheta = cosf( theta );
			quats[ i ] = Quat( x * normal * sinTheta, y * normal * sinTheta, z * normal * sinTheta, cosTheta );
		}
	}
	
	// Structure for recording the errors
	struct Errors
	{
		// Slerp errors
		float Direct;
		float Renormal;
		float Matrix;
		float SimpleRenormal;

		// Squad derivative errors
		float Sqrt;
		float Compact;
	};
	
	// Return rms error between two quaterniosn
	float Error( const Quat &a, const Quat &b )
	{
		return Float(
			Sqrt(
				( a.X - b.X ) * ( a.X - b.X ) +
				( a.Y - b.Y ) * ( a.Y - b.Y ) +
				( a.Z - b.Z ) * ( a.Z - b.Z ) +
				( a.W - b.W ) * ( a.W - b.W )
			)
		);
	}

	// This test the accuracy of various Slerp approximation methods.
	// The result is an error report in fixed-point bits equivalent on stdout.
	void CheckErrors( int numQuats, const Quat *quats, const char *pLabel )
	{
		Errors maxErrors;
		Errors meanErrors;
		maxErrors.Direct = 0;
		meanErrors.Direct = 0;
		maxErrors.Renormal = 0;
		meanErrors.Renormal = 0;
		maxErrors.Matrix = 0;
		meanErrors.Matrix = 0;
		maxErrors.SimpleRenormal = 0;
		meanErrors.SimpleRenormal = 0;
		int tot = 0;
		for( int i = 0; i < numQuats; ++i )
		{
			for( int j = i; j < numQuats; ++j )
			{
				const Quat &a = quats[ i ];
				const Quat &b = quats[ j ];
				SlerpReference slerpReference( a, b );
				SlerpDirect slerpDirect( a, b );
				SlerpMatrix slerpMatrix( a, b );
				SlerpRenormal slerpRenormal( a, b );
				SlerpSimpleRenormal slerpSimpleRenormal( a, b );
				for( int k = 0; k < 10; ++k )
				{
					float t = k * 0.1f;
					Quat qReference = slerpReference.Interpolate( t );
					Quat qDirect = slerpDirect.Interpolate( t );
					Quat qRenormal = slerpRenormal.Interpolate( t );
					Quat qMatrix = slerpMatrix.Interpolate( t );
					Quat qSimpleRenormal = slerpSimpleRenormal.Interpolate( t );

					tot++;
					maxErrors.Direct = Max( maxErrors.Direct, Error( qReference, qDirect ) );
					meanErrors.Direct += Error( qReference, qDirect );
					maxErrors.Renormal = Max( maxErrors.Renormal, Error( qReference, qRenormal ) );
					meanErrors.Renormal += Error( qReference, qRenormal );
					maxErrors.Matrix = Max( maxErrors.Matrix, Error( qReference, qMatrix ) );
					meanErrors.Matrix += Error( qReference, qMatrix );
					maxErrors.SimpleRenormal = Max( maxErrors.SimpleRenormal, Error( qReference, qSimpleRenormal ) );
					meanErrors.SimpleRenormal += Error( qReference, qSimpleRenormal );
				}

			}
		}
		std::cout << pLabel << "  max SlerpDirect " <<  - floorf( logf( fabsf( maxErrors.Direct ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean SlerpDirect " <<  - floorf( logf( fabsf( meanErrors.Direct/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << "  max SlerpRenormal " <<  - floorf( logf( fabsf( maxErrors.Renormal ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean SlerpRenormal " <<  - floorf( logf( fabsf( meanErrors.Renormal/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << "  max SlerpMatrix " <<  - floorf( logf( fabsf( maxErrors.Matrix ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean SlerpMatrix " <<  - floorf( logf( fabsf( meanErrors.Matrix/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << "  max SlerpSimpleRenormal " <<  - floorf( logf( fabsf( maxErrors.SimpleRenormal ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean SlerpSimpleRenormal " <<  - floorf( logf( fabsf( meanErrors.SimpleRenormal/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
	}

	// This check the accuracy of the various Derivative functions using different sets of
	// test quaternions.
	void CheckErrorsDerivative( int numQuats, const Quat *quats, const char *pLabel )
	{
		Errors maxErrors;
		Errors meanErrors;
		maxErrors.Sqrt = 0;
		meanErrors.Sqrt = 0;
		maxErrors.Compact = 0;
		meanErrors.Compact = 0;

		int tot = 0;
		for( int i = 0; i < numQuats; ++i )
		{
			for( int j = i+1; j < numQuats; ++j )
			{
				for( int k = j+1; k < numQuats; ++k )
				{
					const Quat &a = quats[ i ];
					const Quat &b = quats[ j ];
					const Quat &c = quats[ k ];
					Quat qReference = DerivativeReference( a, b, c );
					Quat qSqrt = DerivativeSqrt( a, b, c );
					Quat qCompact = DerivativeCompact( a, b, c );

					maxErrors.Sqrt = Max( maxErrors.Sqrt, Error( qReference, qSqrt ) );
					meanErrors.Sqrt += Error( qReference, qSqrt );
					maxErrors.Compact = Max( maxErrors.Compact, Error( qReference, qCompact ) );
					meanErrors.Compact += Error( qReference, qCompact );
					tot++;
				}
			}
		}
		std::cout << pLabel << "  max DerivativeSqrt " <<  - floorf( logf( fabsf( maxErrors.Sqrt ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean DerivativeSqrt " <<  - floorf( logf( fabsf( meanErrors.Sqrt/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << "  max DerivativeCompact " <<  - floorf( logf( fabsf( maxErrors.Compact ) ) / logf( 2 ) + 0.5f ) << std::endl;
		std::cout << pLabel << " mean DerivativeCompact " <<  - floorf( logf( fabsf( meanErrors.Compact/tot ) ) / logf( 2 ) + 0.5f ) << std::endl;
	}

	// This is more a test of the Squad reference	function than anything else.
	// The test data is coplanar to make the test "human readable"
	// A more thorough test would be needed to make sure that the algorithm
	// was working correctly.
	void CheckSquad( const Quat *pKeys, int numKeys )
	{
		for( float t = 0; t <= numKeys - 1; t += 0.125 )
		{
			int floort = (int)t;
			float fract = t - floort;
			const Quat &q0 = pKeys[ floort - 1 < 0 ? 0 : floort - 1 ];
			const Quat &q1 = pKeys[ floort ];
			const Quat &q2 = pKeys[ floort+1 >= numKeys ? numKeys - 1 : floort+1 ];
			const Quat &q3 = pKeys[ floort+2 >= numKeys ? numKeys - 1 : floort+2 ];
			Quat q12 = SlerpReference( q1, q2 ).Interpolate( fract );
			{
				Quat d1 = DerivativeReference( q0, q1, q2 );
				Quat d2 = DerivativeReference( q3, q2, q1 );
				Quat d12 = SlerpReference( d1, d2 ).Interpolate( fract );
				Quat squad = SlerpReference( q12, d12 ).Interpolate( 2 * fract * ( 1 - fract ) );
				printf( "%6f %8.4f %8.4f\n", t, asinf( squad.X ), acosf( squad.W ) );
			}
			{
				Quat d1 = DerivativeCompact( q0, q1, q2 );
				Quat d2 = DerivativeCompact( q3, q2, q1 );
				Quat d12 = SlerpDirect( d1, d2 ).Interpolate( fract );
				Quat squad = SlerpDirect( q12, d12 ).Interpolate( 2 * fract * ( 1 - fract ) );
				printf( "%6f %8.4f %8.4f\n", t, asinf( squad.X ), acosf( squad.W ) );
			}
			{
				Quat d1 = DerivativeSqrt( q0, q1, q2 );
				Quat d2 = DerivativeSqrt( q3, q2, q1 );
				Quat d12 = SlerpReference( d1, d2 ).Interpolate( fract );
				Quat squad = SlerpReference( q12, d12 ).Interpolate( 2 * fract * ( 1 - fract ) );
				printf( "%6f %8.4f %8.4f\n", t, asinf( squad.X ), acosf( squad.W ) );
			}
		}
	}

	// run a series of tests for accuracy and speed	
	void Test()
	{
		// set up test data
		const int numQuats = 100;
		static Quat bigQuats[ numQuats ];
		static Quat medQuats[ numQuats ];
		static Quat smallQuats[ numQuats ];

		const int numKeys = 7;
		static Quat keys[ numKeys ];
		keys[ 0 ] = Quat( sinf( 0.1f ), 0, 0, cosf( 0.1f ) );
		keys[ 1 ] = Quat( sinf( 0.2f ), 0, 0, cosf( 0.2f ) );
		keys[ 2 ] = Quat( sinf( 0.3f ), 0, 0, cosf( 0.3f ) );
		keys[ 3 ] = Quat( sinf( 0.4f ), 0, 0, cosf( 0.4f ) );
		keys[ 4 ] = Quat( cosf( 0.1f ) * sinf( 0.3f ), sinf( 0.1f ) * sinf( 0.3f ), 0, cosf( 0.3f ) );
		keys[ 5 ] = Quat( cosf( 0.2f ) * sinf( 0.2f ), sinf( 0.2f ) * sinf( 0.2f ), 0, cosf( 0.2f ) );
		keys[ 6 ] = Quat( cosf( 0.3f ) * sinf( 0.1f ), sinf( 0.3f ) * sinf( 0.1f ), 0, cosf( 0.1f ) );
		
		// run accuracy tests. Note that floats are only good to 23 bits.
		GenerateQuats( numQuats, bigQuats, 3.14159f/2 );
		GenerateQuats( numQuats, medQuats, 3.14159f/10 );
		GenerateQuats( numQuats, smallQuats, 3.14159f/10000 );

		CheckErrors( numQuats, bigQuats, "big " );
		std::cout << std::endl;
		CheckErrors( numQuats, medQuats, "med " );
		std::cout << std::endl;
		CheckErrors( numQuats, smallQuats, "tiny" );
		std::cout << std::endl;
		
		CheckErrorsDerivative( numQuats, bigQuats, "big " );
		std::cout << std::endl;
		CheckErrorsDerivative( numQuats, medQuats, "med " );
		std::cout << std::endl;
		CheckErrorsDerivative( numQuats, smallQuats, "tiny" );
		std::cout << std::endl;

		//CheckSquad( keys, numKeys );
		//std::cout << std::endl;
		
		// time tests
		// run the same tests and take the minimum time to
		// factor out task switches and interrupts.
		// Not a guaranteed method and certainly not as good as a
		// performance analyser.
		{
			Time minTime = 0x7fffffff;
			for( int i = 0; i < 20; ++i )
			{
				float t = 0.5f;
				static Quat dead;
				Time startTime = GetTime();
				for( int i = 0; i < numQuats; ++i )
				{
					for( int j = i; j < numQuats; ++j )
					{
						const Quat &a = bigQuats[ i ];
						const Quat &b = bigQuats[ j ];
						SlerpReference slerp( a, b );
						dead = slerp.Interpolate( 0.1f );
						dead = slerp.Interpolate( 0.2f );
						dead = slerp.Interpolate( 0.3f );
						dead = slerp.Interpolate( 0.4f );
						dead = slerp.Interpolate( 0.5f );
						dead = slerp.Interpolate( 0.6f );
						dead = slerp.Interpolate( 0.7f );
						dead = slerp.Interpolate( 0.8f );
						dead = slerp.Interpolate( 0.9f );
					}
				}
				Time endTime = GetTime();
				minTime = minTime < endTime - startTime ? minTime : endTime - startTime;
			}
			std::cout << "time SlerpReference " << minTime << std::endl;
		}

		{
			Time minTime = 0x7fffffff;
			for( int i = 0; i < 20; ++i )
			{
				float t = 0.5f;
				static Quat dead;
				Time startTime = GetTime();
				for( int i = 0; i < numQuats; ++i )
				{
					for( int j = i; j < numQuats; ++j )
					{
						const Quat &a = bigQuats[ i ];
						const Quat &b = bigQuats[ j ];
						SlerpDirect slerp( a, b );
						dead = slerp.Interpolate( 0.1f );
						dead = slerp.Interpolate( 0.2f );
						dead = slerp.Interpolate( 0.3f );
						dead = slerp.Interpolate( 0.4f );
						dead = slerp.Interpolate( 0.5f );
						dead = slerp.Interpolate( 0.6f );
						dead = slerp.Interpolate( 0.7f );
						dead = slerp.Interpolate( 0.8f );
						dead = slerp.Interpolate( 0.9f );
					}
				}
				Time endTime = GetTime();
				minTime = minTime < endTime - startTime ? minTime : endTime - startTime;
			}
			std::cout << "time SlerpDirect " << minTime << std::endl;
		}
		{
			Time minTime = 0x7fffffff;
			for( int i = 0; i < 20; ++i )
			{
				float t = 0.5f;
				static Quat dead;
				Time startTime = GetTime();
				for( int i = 0; i < numQuats; ++i )
				{
					for( int j = i; j < numQuats; ++j )
					{
						const Quat &a = bigQuats[ i ];
						const Quat &b = bigQuats[ j ];
						SlerpMatrix slerp( a, b );
						dead = slerp.Interpolate( 0.1f );
						dead = slerp.Interpolate( 0.2f );
						dead = slerp.Interpolate( 0.3f );
						dead = slerp.Interpolate( 0.4f );
						dead = slerp.Interpolate( 0.5f );
						dead = slerp.Interpolate( 0.6f );
						dead = slerp.Interpolate( 0.7f );
						dead = slerp.Interpolate( 0.8f );
						dead = slerp.Interpolate( 0.9f );
					}
				}
				Time endTime = GetTime();
				minTime = minTime < endTime - startTime ? minTime : endTime - startTime;
			}
			std::cout << "time SlerpMatrix " << minTime << std::endl;
		}
		{
			Time minTime = 0x7fffffff;
			for( int i = 0; i < 20; ++i )
			{
				float t = 0.5f;
				static Quat dead;
				Time startTime = GetTime();
				for( int i = 0; i < numQuats; ++i )
				{
					for( int j = i; j < numQuats; ++j )
					{
						const Quat &a = bigQuats[ i ];
						const Quat &b = bigQuats[ j ];
						SlerpRenormal slerp( a, b );
						dead = slerp.Interpolate( 0.1f );
						dead = slerp.Interpolate( 0.2f );
						dead = slerp.Interpolate( 0.3f );
						dead = slerp.Interpolate( 0.4f );
						dead = slerp.Interpolate( 0.5f );
						dead = slerp.Interpolate( 0.6f );
						dead = slerp.Interpolate( 0.7f );
						dead = slerp.Interpolate( 0.8f );
						dead = slerp.Interpolate( 0.9f );
					}
				}
				Time endTime = GetTime();
				minTime = minTime < endTime - startTime ? minTime : endTime - startTime;
			}
			std::cout << "time SlerpRenormal " << minTime << std::endl;
		}
		{
			Time minTime = 0x7fffffff;
			for( int i = 0; i < 20; ++i )
			{
				float t = 0.5f;
				static Quat dead;
				Time startTime = GetTime();
				for( int i = 0; i < numQuats; ++i )
				{
					for( int j = i; j < numQuats; ++j )
					{
						const Quat &a = bigQuats[ i ];
						const Quat &b = bigQuats[ j ];
						SlerpSimpleRenormal slerp( a, b );
						dead = slerp.Interpolate( 0.1f );
						dead = slerp.Interpolate( 0.2f );
						dead = slerp.Interpolate( 0.3f );
						dead = slerp.Interpolate( 0.4f );
						dead = slerp.Interpolate( 0.5f );
						dead = slerp.Interpolate( 0.6f );
						dead = slerp.Interpolate( 0.7f );
						dead = slerp.Interpolate( 0.8f );
						dead = slerp.Interpolate( 0.9f );
					}
				}
				Time endTime = GetTime();
				minTime = minTime < endTime - startTime ? minTime : endTime - startTime;
			}
			std::cout << "time SlerpSimpleRenormal " << minTime << std::endl;
		}
	}
};

int main()
{
	SlerpApprox::Test();
	getchar();
}


// test results explained:
//
// large quaternion rotations (up to Pi/2)
//
// big   max SlerpDirect 11
// big  mean SlerpDirect 13
// big   max SlerpRenormal 16
// big  mean SlerpRenormal 18
// big   max SlerpMatrix 16
// big  mean SlerpMatrix 19
// big   max SlerpSimpleRenormal 4
// big  mean SlerpSimpleRenormal 8
// 
// medium quaternion rotations (up to Pi/10)
//
// med   max SlerpDirect 11
// med  mean SlerpDirect 11
// med   max SlerpRenormal 17
// med  mean SlerpRenormal 19
// med   max SlerpMatrix 19
// med  mean SlerpMatrix 22
// med   max SlerpSimpleRenormal 11
// med  mean SlerpSimpleRenormal 15
// 
// tiny quaternion rotations (up to Pi/10000)
//
// tiny  max SlerpDirect 11
// tiny mean SlerpDirect 11
// tiny  max SlerpRenormal 19
// tiny mean SlerpRenormal 19
// tiny  max SlerpMatrix 19
// tiny mean SlerpMatrix 19
// tiny  max SlerpSimpleRenormal 19
// tiny mean SlerpSimpleRenormal 19
// 
// Squad derivative accuracy results
//
// big   max DerivativeSqrt 3
// big  mean DerivativeSqrt 6
// big   max DerivativeCompact 15
// big  mean DerivativeCompact 16
// 
// med   max DerivativeSqrt 8
// med  mean DerivativeSqrt 11
// med   max DerivativeCompact 15
// med  mean DerivativeCompact 16
// 
// tiny  max DerivativeSqrt 28
// tiny mean DerivativeSqrt 31
// tiny  max DerivativeCompact 17
// tiny mean DerivativeCompact 17
// 
// Squad sanity check
//
// 0.000000   0.1000   0.1000
// ...
// 6.000000   0.0955   0.1000
// 
// time tests
//
// time SlerpReference 6190
// time SlerpDirect 2360
// time SlerpMatrix 2403
// time SlerpRenormal 3685
// time SlerpSimpleRenormal 1456
