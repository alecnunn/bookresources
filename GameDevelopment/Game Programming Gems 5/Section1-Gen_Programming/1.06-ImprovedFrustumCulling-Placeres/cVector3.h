#ifndef VECTOR3f
#define VECTOR3f

#include <math.h>

#define cVector3f cVector3<float>
#define cVector3d cVector3<double>
#define cVector3b cVector3<unsigned char>
#define cVector3i cVector3<int>
#define cVector3ui cVector3<unsigned int>
#define cVector3w cVector3<unsigned short>

template<class T>
struct cVector3{ 
	T x,y,z; 

	//========================================
	// Constructors
	//========================================
    //  Set to (0,0,0)
	inline cVector3()																									:x(0),  y(0),  z(0){}	
    //  Set to (xyz,xyz,xyz)
	inline cVector3( const T xyz )																						:x(xyz),y(xyz),z(xyz){}	
	inline cVector3( const T xx, const T yy, const T zz )																:x(xx), y(yy), z(zz){}
	inline cVector3( const cVector3<T> &v )																				:x(v.x),y(v.y),z(v.z){}

	//========================================
	// Arithmetic Operators
	//========================================
	inline		  cVector3<T> operator+( const cVector3<T> &vector )const												{ return cVector3<T>(vector.x+x,vector.y+y,vector.z+z); }
	inline friend cVector3<T> operator+( const cVector3<T> &vector, T scalar )										{ return cVector3<T>(vector.x+scalar,  vector.y+scalar,  vector.z+scalar); }
	inline friend cVector3<T> operator+( T scalar, const cVector3<T> &vector )										{ return cVector3<T>(vector.x+scalar,  vector.y+scalar,  vector.z+scalar); }
	inline		  cVector3<T> operator-( const cVector3<T> &vector )const												{ return cVector3<T>(x-vector.x,       y-vector.y,       z-vector.z); }
	inline friend cVector3<T> operator-( const cVector3<T> &vector, T scalar )										{ return cVector3<T>(vector.x-scalar,  vector.y-scalar,  vector.z-scalar); }
	inline friend cVector3<T> operator-( T scalar, const cVector3<T> &vector )										{ return cVector3<T>(scalar-vector.x,  scalar-vector.y,  scalar-vector.z); }
	inline	      cVector3<T> operator-()const																		{ return cVector3<T>(-x,-y,-z); }
	inline friend cVector3<T> operator*( const cVector3<T> &vector, T scalar )										{ return cVector3<T>(vector.x*scalar,  vector.y*scalar,  vector.z*scalar); }
	inline friend cVector3<T> operator*( T scalar, const cVector3<T> &vector )										{ return cVector3<T>(scalar*vector.x,  scalar*vector.y,  scalar*vector.z); }
	inline		  cVector3<T> operator/( const cVector3<T> &vector )const													{ return cVector3<T>(x/vector.x,		y/vector.y,		  z/vector.z); }
	inline friend cVector3<T> operator/( const cVector3<T> &vector, T scalar )										{ float invscalar = 1.0f/(float)scalar; return cVector3<T>(vector.x*invscalar,  vector.y*invscalar,  vector.z*invscalar); }
	inline friend cVector3<T> operator/( T scalar, const cVector3<T> &vector )										{ return cVector3<T>(scalar/vector.x,  scalar/vector.y,  scalar/vector.z); }

    // operator Dot Product (vector proyection against another vector, is a commutative proyection )
	inline	      T		   operator*( const cVector3<T> &vector )const;
    // operator Cross Product on the left hand system
	inline	      cVector3<T> operator%( const cVector3<T> &v )const;

	inline		  cVector3<T> &operator+=( const cVector3<T> &vector );
	inline		  cVector3<T> &operator+=( const T scalar );
	inline		  cVector3<T> &operator-=( const cVector3<T> &vector );
	inline		  cVector3<T> &operator-=( const T scalar );
	inline		  cVector3<T> &operator*=( const cVector3<T> &vector );
	inline		  cVector3<T> &operator*=( const T scalar );
	inline		  cVector3<T> &operator/=( const cVector3<T> &vector );
	inline		  cVector3<T> &operator/=( const T scalar );


	inline		  bool		operator==( const cVector3<T> &vector )const;
	inline		  bool		operator!=( const cVector3<T> &vector )const;
	inline		  bool		operator< ( const cVector3<T> &vector )const;
	inline		  bool		operator< ( const T value )const;
	inline		  bool		operator> ( const cVector3<T> &vector )const;
	inline		  bool		operator> ( const T value )const;
			// Operators to proyect one vector onto another
	inline friend cVector3<T>  operator>>( const cVector3<T> &v1,  const cVector3<T> &v2)											{ return v2*(v1*v2)/(v2*v2); }
    inline friend cVector3<T>  operator<<( const cVector3<T> &v1,  const cVector3<T> &v2)											{ return v1*(v1*v2)/(v1*v1); }

	//========================================
	// Functions
	//========================================
	inline void		  Set( const T xyz )																			{ x = y = z = xyz; }
	inline void		  Set( const T xx, const T yy, const T zz)														{ x = xx;	y = yy;		z = zz; }
	inline void		  Set( const cVector3<T>  &v )																	{ x = v.x;  y = v.y;    z = v.z;}
	inline void		  SetX( const T NewX )																	{ x = NewX;}
	inline void		  SetY( const T NewY )																	{ y = NewY;}
	inline void		  SetZ( const T NewZ )																	{ z = NewZ;}

	inline void		  Invert();
	inline void		  Normalize();
	inline float	  Length()const;
	inline float	  Distance( const cVector3<T>  &Point)const;
	inline float	  Distance( const T xx, const T yy, const T zz)const;
	inline T		  DotProduct(   const cVector3<T> &v1 )const;
	inline cVector3<T>   CrossProduct( const cVector3<T> &v1 )const;

		// Angle between this and a vector, don't has to be a normal vector
	inline T		  UnNormalizedAngleTo( const cVector3<T> &v )const;
	inline T		  UnNormalizedAngleTo( const T x, const T y, const T z )const;

		// Angle between this and a vector, HAS to be a normal vector
	inline T		  NormalizedAngleTo( const cVector3<T>  &v )const;
	inline T		  NormalizedAngleTo( const T x, const T y, const T z )const;

	inline bool		  EqualToWithError( const cVector3<T>  &vector, const T Epsilon)const;
		// Linear interpolation from this to vector
	inline cVector3<T>	  LerpTo( const cVector3<T>  &vector, const T t)const;
};


//==================================================
//          I M P L E M E N T A T I O N
//==================================================

template<class T>
T cVector3<T>::operator*( const cVector3<T>  &vector )const
{ 
	return x*vector.x + y*vector.y + z*vector.z; 
}

//==================================================

template<class T>
cVector3<T> cVector3<T>::operator%( const cVector3<T>  &v )const
{ 
	return cVector3<T> ( v.y*z - v.z*y,   
					 v.z*x - v.x*z,    
					 v.x*y - v.y*x); 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator+=( const cVector3<T>  &vector )
{ 
	x+=vector.x;
	y+=vector.y;
	z+=vector.z;
	return *this;
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator+=( const T scalar )
{ 
	x+=scalar;   
	y+=scalar;   
	z+=scalar; 	 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator-=( const cVector3<T>  &vector )
{ 
	x-=vector.x; 
	y-=vector.y; 
	z-=vector.z; 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator-=( const T scalar )
{ 
	x-=scalar;   
	y-=scalar;   
	z-=scalar; 	 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator*=( const cVector3<T>  &vector )
{ 
	x*=vector.x; 
	y*=vector.y; 
	z*=vector.z; 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator*=( const T scalar )
{ 
	x*=scalar;   
	y*=scalar;   
	z*=scalar; 	 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator/=( const cVector3<T>  &vector )
{ 
	x/=vector.x; 
	y/=vector.y; 
	z/=vector.z; 
	return *this; 
}

//==================================================

template<class T>
cVector3<T>& cVector3<T>::operator/=( const T scalar )
{ 
	float invscalar = 1.0f/(float)scalar;
	x *= invscalar;   
	y *= invscalar;   
	z *= invscalar; 	 
	return *this; 
}

//==================================================

template<class T>
bool cVector3<T>::operator==( const cVector3<T>  &vector )const
{ 
	return	x == vector.x   &&  
			y == vector.y   &&   
			z == vector.z; 
}

//==================================================

template<class T>
bool cVector3<T>::operator!=( const cVector3<T>  &vector )const
{ 
	return	x != vector.x   ||  
			y != vector.y   ||   
			z != vector.z; 
}

//==================================================

template<class T>
bool cVector3<T>::operator< ( const cVector3<T>  &vector )const
{ 
	return	x < vector.x    &&  
			y < vector.y    &&   
			z < vector.z;  
}

//==================================================

template<class T>
bool cVector3<T>::operator< ( const T value )const
{ 
	return	x < value &&  
			y < value &&   
			z < value;  
}

//==================================================

template<class T>
bool cVector3<T>::operator> ( const cVector3<T>  &vector )const
{ 
	return	x > vector.x   &&  
			y > vector.y   &&   
			z > vector.z;  
}

//==================================================

template<class T>
bool cVector3<T>::operator> ( const T value )const
{ 
	return	x > value &&
			y > value &&   
			z > value;  
}

//==================================================

template<class T>
void cVector3<T>::Invert()
{ 
	x = -x;   
	y = -y;  
	z = -z; 
}

//==================================================

template<class T>
void cVector3<T>::Normalize()
{ 
	float reciproc= x*x + y*y + z*z; 
	if (reciproc == 0.0) return; 
	reciproc = InvSqrt( reciproc );
	x *= reciproc; 
	y *= reciproc;
	z *= reciproc; 
}

//==================================================

template<class T>
float cVector3<T>::Length()const
{ 
	return sqrtf(x*x + y*y + z*z); 
}

//==================================================

template<class T>
float cVector3<T>::Distance( const cVector3<T>  &Point)const
{ 
	return (*this-Point).Length(); 
}

//==================================================

template<class T>
float cVector3<T>::Distance( const T xx, const T yy, const T zz)const
{
	return (*this-cVector3<T>(xx,yy,zz)).Length(); 
}

//==================================================

template<class T>
T cVector3<T>::DotProduct(   const cVector3<T>  &v1 )const
{ 
	return *this * v1; 
}

//==================================================

template<class T>
cVector3<T> cVector3<T>::CrossProduct( const cVector3<T>  &v1 )const
{ 
	return *this % v1; 
}

//==================================================

template<class T>
T cVector3<T>::UnNormalizedAngleTo( const cVector3<T>  &v )const
{
	return RADtoDEG( acos( (*this * v ) / (Length() * v.Length()) ) );
}

//==================================================

template<class T>
T cVector3<T>::UnNormalizedAngleTo( const T x, const T y, const T z )const
{ 
	cVector3<T>  v(x,y,z);
	return RADtoDEG( acos( (*this * v ) / (Length() * v.Length()) ) );
}

//==================================================

template<class T>
T cVector3<T>::NormalizedAngleTo( const cVector3<T>  &v )const
{
	return RADtoDEG( acos(*this * v ) );
}

//==================================================

template<class T>
T cVector3<T>::NormalizedAngleTo( const T x, const T y, const T z )const
{ 
	cVector3<T>  v(x,y,z); 
	return RADtoDEG( acos(*this * v ) ); 
}

//==================================================

template<class T>
bool cVector3<T>::EqualToWithError( const cVector3<T>  &vector, const T Epsilon)const
{ 
	return	fabs(vector.x-x)<Epsilon && 
			fabs(vector.y-y)<Epsilon && 
			fabs(vector.z-z)<Epsilon; 
}

//==================================================

template<class T>
cVector3<T> cVector3<T>::LerpTo( const cVector3<T>  &vector, const T t)const
{ 
	return *this + (-t)*(vector - *this); 
}

#endif
