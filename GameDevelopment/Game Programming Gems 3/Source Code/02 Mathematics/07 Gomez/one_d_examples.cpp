//
// Copyright Miguel Gomez, 2002
//
#include <cstdio>
#include <cmath>


//return the sign of v
inline double sgn( const double v )
{
	if( v>0 )
		return 1;	//positive
	else if( v<0 )
		return -1;	//negative
	else
		return 0;	//zero, neutral
}


int main()
{
	//(EXAMPLE 1) Compare numerical output from
	//Euler Method and interpolated Taylor Method
	{
		//ascii text file for plots
		FILE* fE = fopen( "euler1D.txt", "w" );
		FILE* fT = fopen( "taylor1D.txt", "w" );
		//parameters
		const double v0	= 1;	//initial velocity
		const double ud = 1;	//dynamic friction coefficient
		const double h	= 0.02;	//time step
		const double g	= 9.81;	//gravitational acceleration
		const double a0	= -ud * g;//accel until it stops
		//stop time and position
		const double ts	= -v0 / a0;
		const double xs = v0*ts + 0.5*a0*ts*ts;
		//dynamic state variables
		double t = 0;	//time
		double x = 0;	//position
		double v = v0;	//velocity
		int i;

		// --- Euler's Method --- //
		//calculate x and v at time t and print to file
		fprintf( fE, "t\tx\tv\n" );						//column headings
		fprintf( fE, "%.5f\t%.5f\t%.5f\n", t, x, v );	//initial values

		for( i=0 ; i<100 ; i++ )
		{
			const double a = -sgn(v) * ud * g;//acceleration

			t += h;		//new time
			x += v*h;	//new position
			v += a*h;	//new velocity

			fprintf( fE, "%.5f\t%.5f\t%.5f\n", t, x, v );
		}

		// --- Interpolated Taylor Method --- //
		//reset initial values
		t = 0;
		x = 0;
		v = v0;

		//calculate x and v at time t and print to file
		fprintf( fT, "t\tx\tv\n" );						//column headings
		fprintf( fT, "%.5f\t%.5f\t%.5f\n", t, x, v );	//initial values

		for( i=0 ; i<100 ; i++ )
		{
			//acceleration is only due to friction and is opposite v
			const double a  = -sgn(v) * ud * g;

			if( v != 0 )//prevent divide by 0
			{
				//check if the block comes to rest
				//within the time interval [t,t+h]
				const double hs = -v / a;

				if( 0<=hs && hs<h )
				{
					//use hs for time step
					t += hs;
					x += hs*v + 0.5*hs*hs*a;
					v = 0;//explicitly set v to 0
				}
				else
				{
					//use h for time step
					t += h;					//new time
					x += h*v + 0.5*h*h*a;	//new position
					v += h*a;				//new velocity
				}
			}
			else//demonstrate stability of this scheme
			{
				t += h;					//new time
				x += h*v + 0.5*h*h*a;	//new position
				v += h*a;				//new velocity
			}

			fprintf( fT, "%.5f\t%.5f\t%.5f\n", t, x, v );
		}

		//close files
		fclose( fE );
		fclose( fT );
	}

	//(EXAMPLE 2) Demonstrate transitions from
	//dynamic -> static -> dynamic friction.
	//A block slides up a ramp at angle theta,
	//x-axis points up the slope along surface.
	{
		FILE* file = fopen( "transition1D.txt", "w" );
		//parameters
		const double v0	= sqrt(2);	//initial velocity
		const double ud = 0.75;		//dynamic friction coefficient
		const double us = 1.0;		//static friction coefficient
		const double h	= 0.02;		//time step
		const double g	= 9.81;		//gravitational acceleration
		const double theta = atan(1);//slope (PI/4 = 45 deg)
		const double a0	= - g*(ud*sin(theta) + cos(theta));//accel until it stops
		//stop time and position
		const double ts	= -v0 / a0;
		const double xs = v0*ts + 0.5*a0*ts*ts;
		//dynamic state variables
		double t = 0;	//time
		double x = 0;	//position
		double v = v0;	//velocity

		//calculate x and v at time t and print to file
		fprintf( file, "t\tx\tv\n" );//column headings

		for( int i=0 ; i<100 ; i++ )
		{
			if( 0 == v )//block is stationary
			{
				double a = 0;//assume it remains that way

				//can static friction keep it stationary?
				if( tan(theta) > us )//no, transition to dynamic friction
				{
					//IMPORTANT: since v=0, friction acts opposite
					//the acceleration due to gravity (upward)!
					a=	g*ud*sin(theta)	//friction
						- g*cos(theta);	//gravity along slope

				}

				//integrate
				t += h;					//new time
				x += h*v + 0.5*h*h*a;	//new position
				v += h*a;				//new velocity

				//TEST:  give v a very small downward impulse
				//to make sure we transtion to dynamic friction
				if( t >= 0.25 )
				{
					v = -0.0001;
				}
			}
			else//block is moving, use dynamic friction
			{
				const double a=	- g*sgn(v)*ud*sin(theta)//friction (opposite v)
								- g*cos(theta);			//gravity along slope

				//check if the block comes to rest
				//within the time interval [t,t+h]
				const double hs = -v / a;

				if( 0<=hs && hs<h )
				{
					//use hs for time step
					t += hs;
					x += hs*v + 0.5*hs*hs*a;
					v = 0;//explicitly set v to 0
				}
				else
				{
					//use h for time step
					t += h;					//new time
					x += h*v + 0.5*h*h*a;	//new position
					v += h*a;				//new velocity
				}
			}

			fprintf( file, "%.5f\t%.5f\t%.5f\n", t, x, v );
		}

		//close file
		fclose( file );
	}

	return 0;
}

//EOF
