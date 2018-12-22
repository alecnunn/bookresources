#ifndef __GRASS_H__
#define __GRASS_H__
//----------------------------------------------------------------------------
//
//grass.h
//
//Copyright © Rishi Ramraj, 2004
//Grass effect
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

class grass
{
public:
	grass();
	~grass();
	
	bool init(const int width ,const int height, 
		const float damp_factor, const int spread_factor, 
		const float * windvector, float density = 1.0f);

	bool load_height_map( const char * path, float height_scale);
	bool load_scale_map( const char * path, float mul_fac, float add_fac);

	void add_ripple(int x,int y,float height);
	void update(int milliseconds);
	void draw(void (*draw_stalk)(void));
	void draw_scaled(void (*draw_stalk)(void));
	void draw_height(void (*draw_stalk)(void));
	void draw_scaled_height(void (*draw_stalk)(void));
	
private:
	
	//m_hmap - height map
	//m_smap - scale map
	float * m_grid, * m_hmap, * m_smap;
	float m_wind[3], m_density, m_damp_fac;
	bool m_active, m_b_height, m_b_scale;
	int m_width, m_height, m_spread_fac;	
	unsigned int m_srand;

	int gen_index(int x, int y){return(y*m_width + x);}
	bool load_map( const char * path, float **buf);	

};

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//grass.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__GRASS_H__