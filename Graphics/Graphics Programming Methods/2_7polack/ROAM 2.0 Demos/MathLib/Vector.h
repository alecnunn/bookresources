//==============================================================
//= Vector.h
//= Original coders: Trent Polack (trent@voxelsoft.com) and
//-					 Evan Pipho	  (evan@codershq.com)
//==============================================================
//= A 3D vector class
//==============================================================
#ifndef __MathLib_Vector_H__
#define __MathLib_Vector_H__

//handy math constants and macros
#define PI 3.1415926535897932384626433832795f
#define PIOVER180 0.0174532925199432957692369076848861f
#define PIUNDER180 57.2957795130823208767981548141052f

#define DEG_TO_RAD( angle )	( ( angle )*PIOVER180 )
#define RAD_TO_DEG( radians )	( ( radians )*PIUNDER180 )

#define SQR( number )	( number*number )
#define CUBE( number )	( number*number*number )

#define MIN( a, b )  ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define MAX( a, b )  ( ( ( a ) < ( b ) ) ? ( b ) : ( a ) )

#define CLAMP( x, min, max )	( ( x= ( x < min  ? min : x < max ? x : max ) ) )

namespace MathLib {
	//a 3-Dimensional vector class
	class Vector {
			float m_vec[3];

	public:

		float Length( void );

		void Normalize( void );
		Vector GetNormal( void );

		float DotProduct( Vector& vec );
		Vector CrossProduct( Vector& vec );

		void Negate( void );

		//-
		//get the vector's xyz information
		inline float *Get( void ) {
			return m_vec;
		}

		//-
		//set the vector's xyz info
		inline void Set( float x, float y, float z ) {
			m_vec[0]= x;
			m_vec[1]= y;
			m_vec[2]= z;
		}

		//-
		//set the vector's information
		inline void Set( float *pValues ) {
			m_vec[0]= pValues[0];
			m_vec[1]= pValues[1];
			m_vec[2]= pValues[2];
		}

		//overloaded '[]' operator for vector info access, vector[1] (vector.m_vec[1])
		inline float& operator []( int iIdx ) {
			return m_vec[iIdx];
		}

		//overloaded '-' operator, vector - scalar
		inline const Vector operator -  ( const float scalar ) const {
			return Vector( m_vec[0] - scalar,
							m_vec[1] - scalar,
							m_vec[2] - scalar );
		}

		//overloaded '-' operator, vector - vector
		inline const Vector operator -  ( const Vector& vec )  const {
			return Vector( m_vec[0] - vec.m_vec[0],
							m_vec[1] - vec.m_vec[1],
							m_vec[2] - vec.m_vec[2] );
		}

		//overloaded '-' operator for vector negation, -vector
		inline const Vector operator -  ( void ) {
			m_vec[0]= -m_vec[0];
			m_vec[1]= -m_vec[1];
			m_vec[2]= -m_vec[2];

			return *this;
		}

		//overloaded '+' operator, vector + scalar
		inline const Vector operator +  (const float scalar ) const {
			return Vector( m_vec[0] + scalar,
							m_vec[1] + scalar,
							m_vec[2] + scalar );

		}

		//overloaded '+' operator, vector + vector
		inline const Vector operator +  (const Vector& vec )  const {
			return Vector( m_vec[0] + vec.m_vec[0],
							m_vec[1] + vec.m_vec[1],
							m_vec[2] + vec.m_vec[2] );
		}

		//overloaded '=' operator, vector= scalar
		inline const void operator =  ( const Vector& vec ) {
			m_vec[0]= vec.m_vec[0];
			m_vec[1]= vec.m_vec[1];
			m_vec[2]= vec.m_vec[2];
		}

		//overloaded '+=' operator, vector+= scalar
		inline const void operator += ( const float scalar ) {
			m_vec[0]+= scalar;
			m_vec[1]+= scalar;
			m_vec[2]+= scalar;
		}

		//overloaded '+=' operator, vector+= vector
		inline const void operator += ( const Vector& vec ) {
			m_vec[0]+= vec.m_vec[0];
			m_vec[1]+= vec.m_vec[1];
			m_vec[2]+= vec.m_vec[2];
		}

		//overloaded '-=' operator, vector-= scalar
		inline const void operator -= ( const float scalar ) {
			m_vec[0]-= scalar;
			m_vec[1]-= scalar;
			m_vec[2]-= scalar;
		}

		//overloaded '-=' operator, vector-= vector
		inline const void operator -= ( const Vector& vec ) {
			m_vec[0]-= vec.m_vec[0];
			m_vec[1]-= vec.m_vec[1];
			m_vec[2]-= vec.m_vec[2];
		}

		//overloaded '*' operator for evaluation, vector * scalar
		inline const Vector operator *  ( const float scalar ) const {
			return Vector( m_vec[0]*scalar,
							m_vec[1]*scalar,
							m_vec[2]*scalar );
		}

		//overloaded '*' operator, vector * vector
		inline const Vector operator *  ( const Vector& vec )  const {
			return Vector( m_vec[0]*vec.m_vec[0],
							m_vec[1]*vec.m_vec[1],
							m_vec[2]*vec.m_vec[2] );
		}

		//overloaded '/' operator, vector/scalar
		inline const Vector operator /  ( const float scalar ) const {
			float fTemp;

			fTemp= 1/scalar;
			
			return Vector( m_vec[0]*fTemp,
							m_vec[1]*fTemp,
							m_vec[2]*fTemp );
		}

		//overloaded '*=' operator, vector*=scalar
		inline const void operator *= ( const float scalar ) {
			m_vec[0]*= scalar;
			m_vec[1]*= scalar;
			m_vec[2]*= scalar;
		}

		//overloaded '*=' operator, vector/vector
		inline const void operator *= ( const Vector& vec ) {
			m_vec[0]*= vec.m_vec[0];
			m_vec[1]*= vec.m_vec[1];
			m_vec[2]*= vec.m_vec[2];
		}

		//overloaded '/=' operator, vector/= scalar
		inline const void operator /= ( const float scalar ) {
			m_vec[0]/= scalar;
			m_vec[1]/= scalar;
			m_vec[2]/= scalar;
		}

		//overloaded '/=' operator, vector/vector
		inline const void operator /= ( const Vector& vec ) {
			m_vec[0]/= vec.m_vec[0];
			m_vec[1]/= vec.m_vec[1];
			m_vec[2]/= vec.m_vec[2];
		}

		//overloaded '==' operator for evaluation, vector == vector
		inline const bool operator == ( const Vector& vec )  const {
			return ( ( m_vec[0]==vec.m_vec[0] ) &&
					 ( m_vec[1]==vec.m_vec[1] ) && 
					 ( m_vec[2]==vec.m_vec[2] ) );
		}

		//overloaded '!=' operator for evaluation, vector != vector
		inline const bool operator != ( const Vector& vec )  const {
			return !( *this==vec );
		}

	Vector( void ) {
		m_vec[0]= 0;
		m_vec[1]= 0;
		m_vec[2]= 0;
	}

	Vector( float *pValues ) {
		Set( pValues );
	}

	Vector( float x, float y, float z ) {
		m_vec[0]= x;
		m_vec[1]= y;
		m_vec[2]= z;
	}

	~Vector( void ) { }
	};
}

#endif	//__MathLib_Vector_H__