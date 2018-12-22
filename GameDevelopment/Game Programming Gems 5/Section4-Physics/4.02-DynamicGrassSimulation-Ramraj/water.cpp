//----------------------------------------------------------------------------
//
//water.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include "water.h"

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

water::water()
{
	m_active = false;
	m_grid = NULL;
}
water::~water()
{
	delete[] m_grid;
}

bool water::init(const int width , const int height ,const float damp_factor)
{
	delete[] m_grid;
	m_active = false;
	
	//Set up the variables
	if((width <= 0) || (height <= 0) || (damp_factor == 0))
		return false;	
	
	m_damp_fac = damp_factor;
	m_width = width;
	m_height = height;
	m_grid = new float [height * width];
	
	if(m_grid == NULL)
		return false;	
	
	//Sets the water heights to zero
	ZeroMemory(m_grid, sizeof(float) * width * height);

	m_active = true;
	return true;
}

void water::add_ripple(int x,int y,float height)
{
	if ( m_active && (x >= 0) && (y >= 0) && (x < m_width) && (y < m_height) )
		m_grid[gen_index(x,y)] += height;
}

void water::update(int milliseconds)
{
	if(!m_active)
		return;
	
	float y;
	int x,z,i;
	for(z=1; z<(m_height-1); z++)
	{
		for(x=1; x<(m_width-1); x++)
		{
			i = gen_index(x,z);
			y = ( (m_grid[i + 1] +
				   m_grid[i - 1] +
				   m_grid[i + m_width] +
				   m_grid[i - m_width] ) / 2) -
				  (m_grid[i] );
			m_grid[i]= y - y*m_damp_fac*(1000.0f - (float)milliseconds)/1000.0f;
		}
	}
}

void water::draw_sph(void)
{
	if(!m_active)
		return;

	int x, z, i;
	float vec[3];

	for(z=0; z<(m_height-1); z++)
	{
		glBegin(GL_TRIANGLE_STRIP);

		vec[2] = (float)z + 1;

		for(x = 0; x<m_width; x++)
		{
			vec[0] = (float)x;

			i = gen_index(x,z);
			vec[1] = m_grid[i];
			vec[2]--;
			glNormal3fv(vec);
			glVertex3fv(vec);

			vec[1] = m_grid[i + m_width];
			vec[2]++;
			glNormal3fv(vec);
			glVertex3fv(vec);
		}

		glEnd();
	}	
}

void water::draw_tex(void)
{
	if(!m_active)
		return;

	int x, z, i;
	float vec[3];

	for(z=0; z<(m_height-1); z++)
	{
		glBegin(GL_TRIANGLE_STRIP);

		vec[2] = (float)z + 1;

		for(x = 0; x<m_width; x++)
		{
			vec[0] = (float)x;

			i = gen_index(x,z);
			vec[1] = m_grid[i];
			vec[2]--;
			glTexCoord2f( (float)x/float(m_width) , (float)z/float(m_height) );
			glVertex3fv(vec);

			vec[1] = m_grid[i + m_width];
			vec[2]++;
			glTexCoord2f( (float)x/float(m_width) , (float)(z+1)/float(m_height) );
			glVertex3fv(vec);
		}

		glEnd();
	}	
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//water.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
