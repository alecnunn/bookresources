//
// Copyright Miguel Gomez, 2002
//
#include <cstdio>
#include <cmath>


//
// A 3D vector template
//
template <class T>
struct t_vector3
{
	T x,y,z;	//x,y,z coordinates

	//constructors
	//default
	t_vector3()
		:	x(0), y(0), z(0)
	{}

	//initialize
	t_vector3( const T a, const T b, const T c )
		:	x(a), y(b), z(c)
	{}

	void init( const T s )
	{
		x = y = z = s;
	}

	void init( const T a, const T b, const T c )
	{
		x = a;
		y = b;
		z = c;
	}

	//indexing (read-only)
	const T& operator [] ( const long i ) const
	{
		return *((&x) + i);
	}

	//indexing (write)
	T& operator [] ( const long i )
	{
		return *((&x) + i);
	}

	//compare
	const bool operator == ( const t_vector3& b ) const
	{
		return (b.x==x && b.y==y && b.z==z);
	}

	const bool operator != ( const t_vector3& b ) const
	{
		return !(b == *this);
	}

	const bool nearlyEquals( const t_vector3& b, const T r ) const
	{
		//within a tolerance
		const t_vector3 t = *this - b;//difference
		return t.dot(t) < r*r;//radius
	}

	//negate
	const t_vector3 operator - () const
	{
		return t_vector3( -x, -y, -z );
	}

	//assign
	const t_vector3& operator = ( const t_vector3& b )
	{
		x = b.x;
		y = b.y;
		z = b.z;

		return *this;
	}

	//increment
	const t_vector3& operator += ( const t_vector3& b ) 
	{
		x += b.x;
		y += b.y;
		z += b.z;

		return *this;
	} 

	//decrement
	const t_vector3& operator -= ( const t_vector3& b ) 
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;

		return *this;
	} 

	//self-multiply
	const t_vector3& operator *= ( const T s )
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	//self-divide
	const t_vector3& operator /= ( const T s )
	{
		x /= s;
		y /= s;
		z /= s;

		return *this;
	}

	//add
	const t_vector3 operator + ( const t_vector3& b ) const
	{
		return t_vector3( x + b.x, y + b.y, z + b.z );
	}

	//subtract
	const t_vector3 operator - ( const t_vector3& b ) const
	{
		return t_vector3( x - b.x, y - b.y, z - b.z );
	}

	//post-multiply by a scalar
	const t_vector3 operator * ( const T s) const
	{
		return t_vector3( x*s, y*s, z*s );
	}

	//pre-multiply by a scalar
	friend inline const t_vector3 operator * ( const T s, const t_vector3& v )
	{
		return v * s;
	}

	//divide
	const t_vector3 operator / ( const T s ) const
	{
		return t_vector3( x/s, y/s, z/s );
	}

	//cross product
	const t_vector3 cross( const t_vector3& b ) const
	{
		return t_vector3( y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x );
	}

	//scalar dot product
	const T dot( const t_vector3& b ) const
	{
		return x*b.x + y*b.y + z*b.z;
	}

	//length squared
	const T length_squared() const
	{
		return this->dot(*this);
	}

	//length
	const T length() const
	{
		//NOTE:  cast the return value of
		//sqrt() from a double to a T
		return (T)sqrt( this->length_squared() );
	}

	//unit vector
	const t_vector3 unit() const
	{
		return (*this) / this->length();
	}

	//make this a unit vector
	const t_vector3& normalize()
	{
		(*this) /= this->length();

		return *this;
	}
};


//double-precision 3D vector
typedef t_vector3<double>	vec3d;


// demonstrate 3D friction over a flat surface
int main()
{
	//ascii text file for plots
	FILE* file = fopen( "transition3D.txt", "w" );
	//parameters
	const double rt2	= sqrt(2);
	const double rr2 = 1/rt2;
	const vec3d v0(0,1,1);		//initial velocity (directly uphill)
//	const vec3d v0(0.1,1,1);	//initial velocity (diagonally uphill)
	const vec3d n(0,-rr2,rr2);	//contact surface unit normal
	const double sn0= v0.dot(n);//should be 0
	const double m	= 1;		//mass
	const double ud = 0.75;		//dynamic friction coefficient
	const double us = 1.01;		//static friction coefficient
	const double h	= 0.02;		//time step
	const double g	= 9.81;		//gravitational acceleration
	const vec3d Fg(0,0,-m*g);	//gravitational force points in -z direction
	const double N	= fabs(Fg.dot(n));	//magnitude of normal force
	const vec3d Fn	= N * n;			//normal force
	const vec3d Fgt = Fg + Fn;			//tangential force due to gravity only
	const vec3d f0	= -ud*N*v0.unit();	//initial dynamic friction force
	const vec3d a0	= (f0 + Fgt) / m;	//initial acceleration along slope (downward)
	//stop time and position
	//when a0 aligned with v0
	const double a0_mag = a0.length();			//should be -12.139255666020
	const double ts	= - v0.dot(v0)/v0.dot(a0);	//should be 0.11649919906801
	const vec3d xs = v0*ts + 0.5*a0*ts*ts;
	const double xs_mag = xs.length();			//should be 0.082377373663789
	//dynamic state variables
	double t = 0;	//time
	vec3d x(0,0,0);	//position
	vec3d v = v0;	//velocity

		//calculate x and v at time t and print to file
		fprintf( file, "t \t\t\t\t\t\t x \t\t\t\t\t\t v\n" );	//column headings
		fprintf( file, "%.3f\t\t", t );							//time
		fprintf( file, "%.3f\t%.3f\t%.3f\t\t", x.x, x.y, x.z );	//position
		fprintf( file, "%.3f\t%.3f\t%.3f\n", v.x, v.y, v.z );	//velocity

		for( int i=0 ; i<100 ; i++ )
		{
			if( 0 == v.length() )//block is stationary
			{
				vec3d a(0,0,0);//assume it remains that way
				const double fgt_mag = Fgt.length();

				//can static friction keep it stationary?
				if( fgt_mag > us*N )//no, transition to dynamic friction
				{
					//IMPORTANT: since v=0, f is opposite Fgt
					const vec3d f = - ud * N * Fgt.unit();

					a = (f + Fgt)/m;//net acceleration vector

				}

				//integrate
				t += h;					//new time
				x += h*v + 0.5*h*h*a;	//new position
				v += h*a;				//new velocity

				//TEST:  give v a very small downward impulse
				//to make sure we transtion to dynamic friction
				if( t >= 0.25 )
				{
					v.y = -0.0001;
					v.z = -0.0001;
				}
			}
			else//block is moving, use dynamic friction
			{
				const vec3d f = - ud * N * v.unit();
				const vec3d a =	(f + Fgt) / m;

				//check if the block comes to rest
				//within the time interval [t,t+h]
				const double vv = v.dot(v);
				const double va = v.dot(a);
				const double hs = - vv / va;

				if( 0<=hs && hs<h )
				{
					//use hs for time step
					t += hs;
					x += hs*v + 0.5*hs*hs*a;
					v.init(0);//explicitly set v to 0
				}
				else
				{
					//use h for time step
					t += h;					//new time
					x += h*v + 0.5*h*h*a;	//new position
					v += h*a;				//new velocity
				}
			}

			fprintf( file, "%.3f\t\t", t );							//time
			fprintf( file, "%.3f\t%.3f\t%.3f\t\t", x.x, x.y, x.z );	//position
			fprintf( file, "%.3f\t%.3f\t%.3f\n", v.x, v.y, v.z );	//velocity
		}

		//close file
		fclose( file );

	return 0;
}

//EOF
