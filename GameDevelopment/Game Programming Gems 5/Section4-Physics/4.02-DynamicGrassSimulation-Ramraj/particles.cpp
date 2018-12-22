//----------------------------------------------------------------------------
//
//particles.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include "particles.h"

#pragma comment( lib, "opengl32.lib" )

//··········································································//
//    Source :: Definitions
//··········································································//

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

bool particle_emitter::ResetEmitter(int Num_Particles)
{
	active = false;
	NumParticles = Num_Particles;
	Particles = new Particle [NumParticles];
	if(!Particles)
		return false;

	Particle * part;
	for (int loop=0;loop<NumParticles;loop++)
	{
		part = &(Particles[loop]);
		part->life=1.0f;
		part->fade=((float)rand()/(float)RAND_MAX)*fade+0.003f;
		
		part->x= x + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xi;
		part->y= y + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*yi;
		part->z= z + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zi;
		
		part->xg= xg;
		part->yg= yg;
		part->zg= zg;
		part->xi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xs;
		part->yi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*ys;
		part->zi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zs;
		
		part->xr= 0;
		part->yr= 0;
		part->zr= 0;
		part->xru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xr;
		part->yru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*yr;
		part->zru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zr;
	}

	active = true;
	return true;
}

bool particle_emitter::ResetEmitter(int Num_Particles, float * inipos)
{
	if(!inipos)
		return false;
	active = false;
	NumParticles = Num_Particles;
	Particles = new Particle [NumParticles];
	if(!Particles)
		return false;

	Particle * part;
	for (int loop=0;loop<NumParticles;loop++)
	{
		part = &(Particles[loop]);
		part->life=1.0f;
		part->fade=((float)rand()/(float)RAND_MAX)*fade+0.003f;
		
		part->x = inipos[loop];
		part->y = inipos[loop + 1];
		part->z = inipos[loop + 2];
		
		part->xg= xg;
		part->yg= yg;
		part->zg= zg;
		part->xi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xs;
		part->yi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*ys;
		part->zi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zs;
		
		if(irand)
		{
			part->xr = (float)rand()/(float)RAND_MAX * 360.0f;
			part->yr = (float)rand()/(float)RAND_MAX * 360.0f;
			part->zr = (float)rand()/(float)RAND_MAX * 360.0f;
		}
		else
		{
			part->xr= 0;
			part->yr= 0;
			part->zr= 0;
		}
		part->xru=(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xr;
		part->yru=(2*(((float)rand())/((float)RAND_MAX))-1.0f)*yr;
		part->zru=(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zr;
	}

	active = true;
	return true;
}

void particle_emitter::UpdateEmitter(int milliseconds, void (* Conditions)(Particle * cur_part))
{
	if(!active || !milliseconds)
		return;

	Particle * part;
	float delta_t = 1000.0f/(float)milliseconds;
	for (int loop=0;loop<NumParticles;loop++)
	{
		part = &(Particles[loop]);
		part->x+=Particles[loop].xi*slowdown*delta_t;
		part->y+=Particles[loop].yi*slowdown*delta_t;
		part->z+=Particles[loop].zi*slowdown*delta_t;

		part->xi+=Particles[loop].xg*delta_t;
		part->yi+=Particles[loop].yg*delta_t;
		part->zi+=Particles[loop].zg*delta_t;
		part->life-=Particles[loop].fade;

		part->xr += Particles[loop].xru;
		part->yr += Particles[loop].yru;
		part->zr += Particles[loop].zru;

		if (part->life<=0.0f)
		{
			part->life=1.0f;
			part->fade=((float)rand()/(float)RAND_MAX)*fade+0.003f;
		
			part->x= x + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xi;
			part->y= y + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*yi;
			part->z= z + float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zi;
		
			part->xg= xg;
			part->yg= yg;
			part->zg= zg;
			part->xi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xs;
			part->yi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*ys;
			part->zi=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zs;
		
			if(irand)
			{
				part->xr = (float)rand()/(float)RAND_MAX * 360.0f;
				part->yr = (float)rand()/(float)RAND_MAX * 360.0f;
				part->zr = (float)rand()/(float)RAND_MAX * 360.0f;
			}
			else
			{
				part->xr= 0;
				part->yr= 0;
				part->zr= 0;
			}
			part->xru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*xr;
			part->yru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*yr;
			part->zru=float(2*(((float)rand())/((float)RAND_MAX))-1.0f)*zr;
		}
		
		if(Conditions)
			Conditions(part);
	}
}

void particle_emitter::RenderEmitter(void (*Render)(Particle * cur_part))
{
	if(!active || !Render)
		return;

	for (int loop=0;loop<NumParticles;loop++)					// Loop Through All The Particles
	{
		//Set up scene
		glPushMatrix();		
			Render(&(Particles[loop]));
		glPopMatrix();
	}
}

Particle * particle_emitter::get_particle(int index)
{
	if(active && (index > 0) && (index < NumParticles))
		return &(Particles[index]);
	else
		return 0;
}
void particle_emitter::set_pos(float * pos)
{
	x = pos[0];
	y = pos[1];
	z = pos[2];
}
void particle_emitter::set_grav(float * grav)
{
	xg = grav[0];
	yg = grav[1];
	zg = grav[2];
}
void particle_emitter::set_spread(float * spread)
{
	xi = spread[0];
	yi = spread[1];
	zi = spread[2];
}
void particle_emitter::set_vel(float * vel)
{
	xs = vel[0];
	ys = vel[1];
	zs = vel[2];
}
void particle_emitter::set_rot_vel(float * vel)
{
	xr = vel[0];
	yr = vel[1];
	zr = vel[2];
}
void particle_emitter::set_fade(float f)
{
	fade = f;
}
void particle_emitter::set_slowdown(float s)
{
	slowdown = s;
}
void particle_emitter::set_init_rand_pos(bool ini_rand)
{
	irand = ini_rand;
}

//----------------------------------------------------------------------------
//particles.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
