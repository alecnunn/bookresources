//----------------------------------------------------------------------------
//
//leaves.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl/gl.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include "mth_angle.h"
#include "mth_spherical.h"
#include "leaves.h"

#pragma comment( lib, "opengl32.lib" )

//··········································································//
//    Source :: Definitions
//··········································································//

#define AZI_SCALE 1.0f
#define INC_SCALE 1.0f

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

leaves::leaves()
{
	m_active = false;
	m_leaves = 0;
}
leaves::~leaves()
{
	delete[] m_leaves;
}

bool leaves::init(int num_leaves, const float * leaf_pos, const float * wind_dir, float scale_offset, float damp_fac)
{
	if(!leaf_pos || !wind_dir || (num_leaves < 6))
		return false;

	m_active = false;
	delete[] m_leaves;

	m_damp_fac = damp_fac;
	m_num_leaves = num_leaves;
	m_leaves = new node[m_num_leaves];
	if(!m_leaves)
		return false;

	//copy in wind
	{
		float sph[3];
		mth_cart_to_sph(sph,wind_dir);
		wind[0] = sph[0] * MTH_R_TO_D;
		wind[1] = sph[1] * MTH_R_TO_D;
	}
	//copy in positions
	int loop1, loop2;
	node * p_leaf;
	for(loop1 = 0; loop1 < m_num_leaves; loop1++)
	{
		p_leaf = &m_leaves[loop1];
		//assign positions
		memcpy((void*)p_leaf->pos,(void*)&leaf_pos[loop1*3],sizeof(float)*3);
		p_leaf->energy = 0.0f;
		//random direction
		p_leaf->dir[0] = (float)rand()/(float)RAND_MAX;
		p_leaf->dir[0] *= MTH_360;		
		p_leaf->dir[1] = (float)rand()/(float)RAND_MAX;
		p_leaf->dir[1] *= MTH_180;
	}

	//determine scales - largest leaf in the middle
	{
		//calculate center
		float center[3];
		center[0] = center[1] = center[2] = 0.0f;
		for(loop1 = 0; loop1 < m_num_leaves; loop1++)
		{
			p_leaf = &m_leaves[loop1];
			center[0] += p_leaf->pos[0];
			center[1] += p_leaf->pos[1];
			center[2] += p_leaf->pos[2];
		}
		center[0] /= (float)m_num_leaves;
		center[1] /= (float)m_num_leaves;
		center[2] /= (float)m_num_leaves;

		//find distances from the center
		//find the farthest vector from the center
		float vec[3];
		float max = 0;
		for(loop1 = 0; loop1 < m_num_leaves; loop1++)
		{
			p_leaf = &m_leaves[loop1];
			vec[0] = p_leaf->pos[0] - center[0];
			vec[1] = p_leaf->pos[1] - center[1];
			vec[2] = p_leaf->pos[2] - center[2];

			p_leaf->scale = vec[0]*vec[0] + 
							vec[1]*vec[1] + 
							vec[2]*vec[2];
			p_leaf->scale = (float)sqrt(p_leaf->scale);

			if(p_leaf->scale > max)
				max = p_leaf->scale;
		}

		for(loop1 = 0; loop1 < m_num_leaves; loop1++)
		{
			p_leaf = &m_leaves[loop1];
			p_leaf->scale /= max;
			p_leaf->scale += scale_offset;
		}
	}

	//determine neighbours - three closest except itself
	{
		float temp[3];
		node * p_temp;
		for(loop1 = 0; loop1 < m_num_leaves; loop1++)
		{
			p_leaf = &m_leaves[loop1];
			//initialize first neighbours - scale used to store distance
			if(loop1 < 3)
			{
				//initialize with 3,4,5				
				p_leaf->links[0].neighbour = &m_leaves[3];
				p_leaf->links[1].neighbour = &m_leaves[4];
				p_leaf->links[2].neighbour = &m_leaves[5];
			}
			else
			{
				//initialzie with 0,1,2
				p_leaf->links[0].neighbour = &m_leaves[0];
				p_leaf->links[1].neighbour = &m_leaves[1];
				p_leaf->links[2].neighbour = &m_leaves[2];
			}
			//sort
			
			temp[0] = p_leaf->links[0].neighbour->pos[0] - p_leaf->pos[0];
			temp[1] = p_leaf->links[0].neighbour->pos[1] - p_leaf->pos[1];
			temp[2] = p_leaf->links[0].neighbour->pos[2] - p_leaf->pos[2];
			p_leaf->links[0].n_scale = temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2];

			temp[0] = p_leaf->links[1].neighbour->pos[0] - p_leaf->pos[0];
			temp[1] = p_leaf->links[1].neighbour->pos[1] - p_leaf->pos[1];
			temp[2] = p_leaf->links[1].neighbour->pos[2] - p_leaf->pos[2];
			p_leaf->links[1].n_scale = temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2];

			temp[0] = p_leaf->links[2].neighbour->pos[0] - p_leaf->pos[0];
			temp[1] = p_leaf->links[2].neighbour->pos[1] - p_leaf->pos[1];
			temp[2] = p_leaf->links[2].neighbour->pos[2] - p_leaf->pos[2];
			p_leaf->links[2].n_scale = temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2];

			//note : p_leaf->links[0] is closest
			if(p_leaf->links[2].n_scale < p_leaf->links[1].n_scale)
			{
				//swap
				p_temp = p_leaf->links[1].neighbour;
				p_leaf->links[1].neighbour = p_leaf->links[2].neighbour;
				p_leaf->links[2].neighbour = p_temp;

				temp[0] = p_leaf->links[1].n_scale;
				p_leaf->links[1].n_scale = p_leaf->links[2].n_scale;
				p_leaf->links[2].n_scale = temp[0];
			}

			if(p_leaf->links[1].n_scale < p_leaf->links[0].n_scale)
			{
				//swap
				p_temp = p_leaf->links[0].neighbour;
				p_leaf->links[0].neighbour = p_leaf->links[1].neighbour;
				p_leaf->links[1].neighbour = p_temp;

				temp[0] = p_leaf->links[0].n_scale;
				p_leaf->links[0].n_scale = p_leaf->links[1].n_scale;
				p_leaf->links[1].n_scale = temp[0];
			}

			//check for closer neighbours
			for(loop2 = 0; loop2 < m_num_leaves; loop2++)
			{
				if(loop2 == loop1)
					continue;

				p_temp = &m_leaves[loop2];

				temp[0] = p_temp->pos[0] - p_leaf->pos[0];
				temp[1] = p_temp->pos[1] - p_leaf->pos[1];
				temp[2] = p_temp->pos[2] - p_leaf->pos[2];
				temp[0] = temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2];

				if(temp[0] < p_leaf->links[2].n_scale)
				{
					if(temp[0] < p_leaf->links[1].n_scale)
					{
						if(temp[0] < p_leaf->links[0].n_scale)
						{
							p_leaf->links[0].neighbour = p_temp;
							p_leaf->links[0].n_scale = temp[0];
						}
						else
						{
							p_leaf->links[1].neighbour = p_temp;
							p_leaf->links[1].n_scale = temp[0];
						}
					}
					else
					{
						p_leaf->links[2].neighbour = p_temp;
						p_leaf->links[2].n_scale = temp[0];
					}
				}//if(temp[0] > p_leaf->links[2])
			}//for(loop2 = 0; loop2 < m_num_leaves; loop2++)
			
			//determine neighbour scalars
			p_leaf->links[0].n_scale = (float)sqrt(p_leaf->links[0].n_scale);
			p_leaf->links[1].n_scale = (float)sqrt(p_leaf->links[1].n_scale);
			p_leaf->links[2].n_scale = (float)sqrt(p_leaf->links[2].n_scale);

			temp[0] = p_leaf->links[0].n_scale + p_leaf->links[1].n_scale + p_leaf->links[2].n_scale;
			if(!temp[0])
			{
				p_leaf->links[0].n_scale = p_leaf->links[1].n_scale = p_leaf->links[2].n_scale = 0.0f;
			}
			else
			{
				p_leaf->links[0].n_scale /= temp[0];
				p_leaf->links[1].n_scale /= temp[0];
				p_leaf->links[2].n_scale /= temp[0];
			}
		}//for(loop1 = 0; loop1 < m_num_leaves; loop1++)
	}//determine neighbours - three closest except itself
	
	m_active = true;
	return true;
}
void leaves::update(int milliseconds)
{
	if(!m_active)
		return;

	for(int loop1 = 0; loop1 < m_num_leaves; loop1++)
	{
		m_leaves[loop1].energy += 
			m_leaves[loop1].links[0].neighbour->energy * m_leaves[loop1].links[0].n_scale +
			m_leaves[loop1].links[1].neighbour->energy * m_leaves[loop1].links[1].n_scale +
			m_leaves[loop1].links[2].neighbour->energy * m_leaves[loop1].links[2].n_scale;
		m_leaves[loop1].energy /= 2;
		m_leaves[loop1].energy -= m_leaves[loop1].energy * m_damp_fac * (1000.0f - (float)milliseconds)/1000.0f;
	}
}
void leaves::draw(void (*draw_leaf)(void))
{
	if(!m_active || !draw_leaf)
		return;

	float final;	

	node * p_leaf;
	for(int loop1 = 0; loop1 < m_num_leaves; loop1++)
	{
		p_leaf = &(m_leaves[loop1]);
		glPushMatrix();

			glTranslatef(p_leaf->pos[0],p_leaf->pos[1],p_leaf->pos[2]);

			//apply azimuth
			final = wind[0] - p_leaf->dir[0];
			final *= p_leaf->energy * AZI_SCALE;
			//add random
			final += (final * (float)rand()/(float)RAND_MAX) - final/2.0f;
			glRotatef(p_leaf->dir[0] + final, 0.0f, -1.0f, 0.0f);			
			
			//apply inclination		
			final = wind[1] - p_leaf->dir[1];
			final *= p_leaf->energy * INC_SCALE;
			//add random
			final += (final * (float)rand()/(float)RAND_MAX) - final/2.0f;
			glRotatef(p_leaf->dir[1] + final, 1.0f, 0.0f, 0.0f);			

			glScalef(p_leaf->scale,p_leaf->scale,p_leaf->scale);

			draw_leaf();
		
		glPopMatrix();

		//uncomment to view network
		/*glColor3f(p_leaf->scale,p_leaf->scale,p_leaf->scale);

		glBegin(GL_LINES);

		glVertex3fv(p_leaf->pos);
		glVertex3fv(p_leaf->links[0].neighbour->pos);
		glVertex3fv(p_leaf->pos);
		glVertex3fv(p_leaf->links[1].neighbour->pos);
		glVertex3fv(p_leaf->pos);
		glVertex3fv(p_leaf->links[2].neighbour->pos);

		glEnd();*/
	}
}
void leaves::add_ripple(int index, float energy)
{
	if(m_active && (index >= 0) && (index < m_num_leaves))
		m_leaves[index].energy += energy;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//leaves.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
