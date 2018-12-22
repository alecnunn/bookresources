#ifndef MATH_H
#define MATH_H

#define PI					3.1415926f
#define TWO_PI				6.2831853f

#define CIRCLE_SECTIONS		16
#define VERTICES_IN_CIRCLE	96
#define VERTICES_IN_BOX		24

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z, w; 
    DWORD color;   
};

inline void d3dxvector3_to_customvertex(
	D3DXVECTOR3 *v,
	DWORD color,
	CUSTOMVERTEX* out)
{
	out->x = v->x;
	out->y = v->y;
	out->z = v->z;
	out->w = 1.0f;
	out->color = color;
};

inline void d3dxvector2_rotate(
	D3DXVECTOR2 *v,
	float sine,
	float cosine,
	D3DXVECTOR2 *out)
{
	float y = (sine * v->x) + (cosine * v->y);
	out->x = (cosine * v->x) - (sine * v->y);
	out->y = y;
}

inline void line3d_to_vector(
	D3DXVECTOR3 *a,
	D3DXVECTOR3 *b,
	D3DXVECTOR3 *out)
{
	out->x = b->x - a->x;
	out->y = b->y - a->y;
	out->z = b->z - a->z;
}

inline BOOL d3dxvector3_compare(
	D3DXVECTOR3 *v1,
	D3DXVECTOR3 *v2,
	float fEpsilon )
{
	return ( (fabs(v1->x - v2->x) < fEpsilon) && 
			 (fabs(v1->y - v2->y) < fEpsilon) &&
			 (fabs(v1->z - v2->z) < fEpsilon) );
}

#endif // MATH_H