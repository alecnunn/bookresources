#ifndef __PARTICLES_H__
#define __PARTICLES_H__
//----------------------------------------------------------------------------
//
//particles.h
//
//Copyright © Rishi Ramraj, 2004
//Particle Emitter
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//··········································································//
//    header :: Definitions
//··········································································//

//··········································································//
//    header :: Structures
//··········································································//

//··········································································//
//    header :: Class Defs
//··········································································//

//··········································································//
//    header :: Function Defs
//··········································································//

struct Particle
{
	float	life;
	float	fade;
	
	float	x;
	float	y;
	float	z;
	
	float	xi;
	float	yi;
	float	zi;
	float	xg;
	float	yg;
	float	zg;

	float	xr;
	float	yr;
	float	zr;
	float	xru;
	float	yru;
	float	zru;
};

class particle_emitter
{
public:
	particle_emitter()
	{
		Particles = 0;
		slowdown=x=y=z=xg=yg=zg=xi=yi=zi=xs=ys=zs=xr=yr=zr=fade = 0.0f;		
		active = false;
		irand = false;
	}	
	~particle_emitter()
	{
		delete [] Particles;
	}
	
	bool ResetEmitter(int Num_Particles);
	bool ResetEmitter(int Num_Particles, float * inipos);
	void UpdateEmitter(int milliseconds, void (* Conditions)(Particle * cur_part) = 0);
	void RenderEmitter(void (*Render)(Particle * cur_part));

	//accessors
	Particle * get_particle(int index);
	void set_pos(float * pos);
	void set_grav(float * grav);
	void set_spread(float * spread);//i
	void set_vel(float * vel);
	void set_rot_vel(float * vel);
	void set_fade(float f);
	void set_slowdown(float s);
	void set_init_rand_pos(bool ini_rand);

private:

	bool active;

	//A few control variables
	//Note that the xi,yi and zi scale the initial position
	//xs, ys, and zs scale the initial velocity
	//fade lets you adjust how long the particles last
	float slowdown, x, y, z, xg, yg, zg, xi, yi, zi, xs, ys, zs, xr, yr, zr, fade;
	bool irand;
	Particle * Particles;

	int NumParticles;
};

//----------------------------------------------------------------------------
//particles.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__PARTICLES_H__