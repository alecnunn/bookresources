/*****************************************************************
 * Copyright (c) 2002 TheyerGFX Pty Ltd (www.theyergfx.com)
 *****************************************************************
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************
 * Project		: Game Programming Methods
 *****************************************************************
 * File			: pntri.c
 * Language		: ANSI C
 * Author		: Mark Theyer
 * Created		: 13 Dec 2002
 *****************************************************************
 * Description	: API for Curved PN Triangles
 *****************************************************************/

/*
 * includes
 */

#include <pntri.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

/*
 * macros
 */

/*
 * typedefs
 */

typedef struct {
	int		p[3];
} Triangle;

/*
 * prototypes
 */

// control grid
int 		pn_tanpts( Point_f3d *p1, Point_f3d *p2, Point_f3d *n1, Point_f3d *n2, Point_f3d *out );
int 		pn_normal( Point_f3d *p1, Point_f3d *p2, Point_f3d *n1, Point_f3d *n2, Point_f3d *nout );
int 		pn_centre( Point_f3d *p, Point_f3d *out );

// bezier curves
Point_f3d	bez_pt( Point_f3d *b1, Point_f3d *b2, Point_f3d *b3, Point_f3d *b4, float len );
Point_f3d	bez_patch_pt( Point_f3d *net, float u, float v );
Point_f3d   bez_normal( Point_f3d *net, float u, float v );

// util's
Point_f3d	lenpoint( Point_f3d *p1, Point_f3d *p2, float len );
void		normalise( Point_f3d *n );

/*
 * variables
 */

/* 
 * globals variables...
 */

/*
 * functions
 */


/******************************************************************************
 * Function:
 * pnControlNet -- calculate a control grid for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pnControlNet (
	Point_f3d	*p_in,
	Point_f3d	*n_in,
	Point_f3d	*p_out
	)
{
	int			 p1, p2, p3;
	Point_f3d	 net[10];

	/* init (no validation) */
	p1 = 0;
	p2 = 1;
	p3 = 2;

	/* points */
	*p_out = p_in[p1]; p_out++;
	p_out += pn_tanpts( &p_in[p1], &p_in[p2], &n_in[p1], &n_in[p2], p_out );
	*p_out = p_in[p2]; p_out++;
	p_out += pn_tanpts( &p_in[p2], &p_in[p3], &n_in[p2], &n_in[p3], p_out );
	*p_out = p_in[p3]; p_out++;
	p_out += pn_tanpts( &p_in[p3], &p_in[p1], &n_in[p3], &n_in[p1], p_out );
	p_out += pn_centre( (p_out - 9), p_out );

	/* copy these points */
	memcpy( net, (p_out - 10), sizeof(net) );

	/* tack normals onto the end of the points list */
	*p_out = n_in[0]; p_out++;
	*p_out = n_in[1]; p_out++;
	*p_out = n_in[2]; p_out++;
	p_out += pn_normal( &p_in[p1], &p_in[p2], &n_in[p1], &n_in[p2], p_out );
	p_out += pn_normal( &p_in[p2], &p_in[p3], &n_in[p2], &n_in[p3], p_out );
	p_out += pn_normal( &p_in[p3], &p_in[p1], &n_in[p3], &n_in[p1], p_out );

	/* re-order */
	p_out -= 16;
	p_out[1] = net[8];
	p_out[2] = net[1];
	p_out[3] = net[7];
	p_out[4] = net[9];
	p_out[5] = net[2];
	p_out[6] = net[6];
	p_out[7] = net[5];
	p_out[8] = net[4];
	p_out[9] = net[3];
}


/******************************************************************************
 * Function:
 * pnControlNet3 -- calculate a control grid for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pnControlNet3 (
	Point_f3d	*p_in,
	Point_f3d	*n_in,
	Point_f3d	*p_out
	)
{
	int			 p1, p2, p3;
	int			 d1, d2, d3, d4, d5, d6;
	Point_f3d	 net[10];

	/* init (no validation) */
	p1 = 0;
	p2 = 1;
	p3 = 2;
	// offset 3 for p4 (quad)
	d1 = 4;
	d2 = 5;
	d3 = 6;
	d4 = 7;
	d5 = 8;
	d6 = 9;

	/* points */
	*p_out = p_in[p1]; p_out++;
	p_out += pn_tanpts( &p_in[p1], &p_in[p2], &n_in[d1], &n_in[d2], p_out );
	*p_out = p_in[p2]; p_out++;
	p_out += pn_tanpts( &p_in[p2], &p_in[p3], &n_in[d3], &n_in[d4], p_out );
	*p_out = p_in[p3]; p_out++;
	p_out += pn_tanpts( &p_in[p3], &p_in[p1], &n_in[d5], &n_in[d6], p_out );
	p_out += pn_centre( (p_out - 9), p_out );

	/* copy these points */
	memcpy( net, (p_out - 10), sizeof(net) );

	/* tack normals onto the end of the points list */
	*p_out = n_in[0]; p_out++;
	*p_out = n_in[1]; p_out++;
	*p_out = n_in[2]; p_out++;
	p_out += pn_normal( &p_in[p1], &p_in[p2], &n_in[p1], &n_in[p2], p_out );
	p_out += pn_normal( &p_in[p2], &p_in[p3], &n_in[p2], &n_in[p3], p_out );
	p_out += pn_normal( &p_in[p3], &p_in[p1], &n_in[p3], &n_in[p1], p_out );

	/* re-order */
	p_out -= 16;
	p_out[1] = net[8];
	p_out[2] = net[1];
	p_out[3] = net[7];
	p_out[4] = net[9];
	p_out[5] = net[2];
	p_out[6] = net[6];
	p_out[7] = net[5];
	p_out[8] = net[4];
	p_out[9] = net[3];
}


/******************************************************************************
 * Function:
 * pnControlNet4 -- calculate a control grid for a quad' patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pnControlNet4 (
	Point_f3d	*p_in,
	Point_f3d	*n_in,
	Point_f3d	*p_out
	)
{
	int			 p1, p2, p3, p4;
	int			 d1, d2, d3, d4, d5, d6, d7, d8;
	Point_f3d	 net[16];

	/* init (no validation) */
	p1 = 0;
	p2 = 1;
	p3 = 2;
	p4 = 3;
	d1 = 4;
	d2 = 5;
	d3 = 6;
	d4 = 7;
	d5 = 8;
	d6 = 9;
	d7 = 10;
	d8 = 11;

	/* points */
	*p_out = p_in[p1]; p_out++;
	p_out += pn_tanpts( &p_in[p1], &p_in[p2], &n_in[d1], &n_in[d2], p_out );
	*p_out = p_in[p2]; p_out++;
	p_out += pn_tanpts( &p_in[p2], &p_in[p4], &n_in[d3], &n_in[d4], p_out );
	*p_out = p_in[p4]; p_out++;
	p_out += pn_tanpts( &p_in[p4], &p_in[p3], &n_in[d5], &n_in[d6], p_out );
	*p_out = p_in[p3]; p_out++;
	p_out += pn_tanpts( &p_in[p3], &p_in[p1], &n_in[d7], &n_in[d8], p_out );
	p_out += pn_tanpts( &p_in[p1], &p_in[p4], &n_in[d8], &n_in[d4], p_out );
	p_out += pn_tanpts( &p_in[p3], &p_in[p2], &n_in[d7], &n_in[d3], p_out );

	/* copy these points */
	memcpy( net, (p_out - 16), sizeof(net) );

	/* tack normals onto the end of the points list */
	*p_out = n_in[0]; p_out++;
	*p_out = n_in[1]; p_out++;
	*p_out = n_in[2]; p_out++;
	*p_out = n_in[3]; p_out++;
	p_out += pn_normal( &p_in[p1], &p_in[p2], &n_in[p1], &n_in[p2], p_out );
	p_out += pn_normal( &p_in[p2], &p_in[p4], &n_in[p2], &n_in[p4], p_out );
	p_out += pn_normal( &p_in[p4], &p_in[p3], &n_in[p4], &n_in[p3], p_out );
	p_out += pn_normal( &p_in[p3], &p_in[p1], &n_in[p3], &n_in[p1], p_out );

	/* re-order */
	p_out -= 24;
	p_out[4]  = net[11];
	p_out[5]  = net[12];
	p_out[6]  = net[15];
	p_out[7]  = net[4];
	p_out[8]  = net[10];
	p_out[9]  = net[14];
	p_out[10] = net[13];
	p_out[11] = net[5];
	p_out[12] = net[9];
	p_out[13] = net[8];
	p_out[14] = net[7];
	p_out[15] = net[6];
}


/******************************************************************************
 * Function:
 * pnBezierPatch -- construct triangle patch points and normals
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnBezierPatch ( 
	Point_f3d	*net, 
	int			 lod, 
	Point_f3d	*pout,
	Point_f3d	*nout
	)
{
	Point_f3d	 b300,b030,b003;
	Point_f3d	 b210,b120;
	Point_f3d	 b201,b102;
	Point_f3d	 b021,b012;
	Point_f3d	 b111;
	Point_f3d	 n200,n020,n002;
	Point_f3d	 n110,n011,n101;
	float		 u,v,w;
	float		 u2,v2,w2;
	float		 u3,v3,w3;
	float		 inc;
	int			 p;
	int			 i, n;
	int			 step;

	/* init */
	b300 = net[0];
	b030 = net[9];
	b003 = net[6];
	b210 = net[2];
	b120 = net[5];
	b201 = net[1];
	b102 = net[3];
	b021 = net[8];
	b012 = net[7];
	b111 = net[4];
	n200 = net[10];
	n020 = net[11];
	n002 = net[12];
	n110 = net[13];
	n011 = net[14];
	n101 = net[15];

	/* init */
	inc  = 1.0f/(float)(lod + 1);
	u    = 0.0f;
	v    = 0.0f;
	n    = (lod + 2);
	step = n;
	p    = 0;
	while( step > 0 ) {
		for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
			w = (1.0f - u - v);
			u2 = (u * u);
			v2 = (v * v);
			w2 = (w * w);
			u3 = (u2 * u);
			v3 = (v2 * v);
			w3 = (w2 * w);
			/* points */
			pout[p].x = (b300.x * w3) + (b030.x * u3) + (b003.x * v3) +
				(b210.x * 3.0f * w2 * u) + (b120.x * 3.0f * w * u2) + 
				(b201.x * 3.0f * w2 * v) + (b021.x * 3.0f * u2 * v) + 
				(b102.x * 3.0f * w * v2) + (b012.x * 3.0f * u * v2) + 
				(b111.x * 6.0f * w * u * v);
			pout[p].y = (b300.y * w3) + (b030.y * u3) + (b003.y * v3) +
				(b210.y * 3.0f * w2 * u) + (b120.y * 3.0f * w * u2) + 
				(b201.y * 3.0f * w2 * v) + (b021.y * 3.0f * u2 * v) + 
				(b102.y * 3.0f * w * v2) + (b012.y * 3.0f * u * v2) + 
				(b111.y * 6.0f * w * u * v);
			pout[p].z = (b300.z * w3) + (b030.z * u3) + (b003.z * v3) +
				(b210.z * 3.0f * w2 * u) + (b120.z * 3.0f * w * u2) + 
				(b201.z * 3.0f * w2 * v) + (b021.z * 3.0f * u2 * v) + 
				(b102.z * 3.0f * w * v2) + (b012.z * 3.0f * u * v2) + 
				(b111.z * 6.0f * w * u * v);
			/* normals */
			nout[p].x = (n200.x * w2) + (n020.x * u2) + (n002.x * v2) +
				(n110.x * w * u) + (n011.x * u * v) + (n101.x * w * v);
			nout[p].y = (n200.y * w2) + (n020.y * u2) + (n002.y * v2) +
				(n110.y * w * u) + (n011.y * u * v) + (n101.y * w * v);
			nout[p].z = (n200.z * w2) + (n020.z * u2) + (n002.z * v2) +
				(n110.z * w * u) + (n011.z * u * v) + (n101.z * w * v);
			normalise( &nout[p] );
			p++;
		}
		v += inc;
		n--;
		step--;
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pnBezierPatch3 -- construct triangle patch points and normals
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnBezierPatch3 ( 
	Point_f3d	*net, 
	int			 nsides,
	int			 lod, 
	Point_f3d	*pout,
	Point_f3d	*nout
	)
{
	Point_f3d	 b300,b030,b003;
	Point_f3d	 b210,b120;
	Point_f3d	 b201,b102;
	Point_f3d	 b021,b012;
	Point_f3d	 b111;
	Point_f3d	 n200,n020,n002;
	Point_f3d	 n110,n011,n101;
	float		 u,v,w;
	float		 u2,v2,w2;
	float		 u3,v3,w3;
	float		 inc;
	int			 p;
	int			 i, n;
	int			 step;

	/* init */
	b300 = net[0];
	b030 = net[9];
	b003 = net[6];
	b210 = net[2];
	b120 = net[5];
	b201 = net[1];
	b102 = net[3];
	b021 = net[8];
	b012 = net[7];
	b111 = net[4];
	n200 = net[10];
	n020 = net[11];
	n002 = net[12];
	n110 = net[13];
	n011 = net[14];
	n101 = net[15];

	/* init */
	inc  = 1.0f/(float)(lod + 1);
	u    = 0.0f;
	v    = 0.0f;
	n    = (lod + 2);
	step = n;
	p    = 0;
	while( step > 0 ) {
		for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
			w = (1.0f - u - v);
			u2 = (u * u);
			v2 = (v * v);
			w2 = (w * w);
			u3 = (u2 * u);
			v3 = (v2 * v);
			w3 = (w2 * w);
			/* points */
			pout[p].x = (b300.x * w3) + (b030.x * u3) + (b003.x * v3) +
				(b210.x * 3.0f * w2 * u) + (b120.x * 3.0f * w * u2) + 
				(b201.x * 3.0f * w2 * v) + (b021.x * 3.0f * u2 * v) + 
				(b102.x * 3.0f * w * v2) + (b012.x * 3.0f * u * v2) + 
				(b111.x * 6.0f * w * u * v);
			pout[p].y = (b300.y * w3) + (b030.y * u3) + (b003.y * v3) +
				(b210.y * 3.0f * w2 * u) + (b120.y * 3.0f * w * u2) + 
				(b201.y * 3.0f * w2 * v) + (b021.y * 3.0f * u2 * v) + 
				(b102.y * 3.0f * w * v2) + (b012.y * 3.0f * u * v2) + 
				(b111.y * 6.0f * w * u * v);
			pout[p].z = (b300.z * w3) + (b030.z * u3) + (b003.z * v3) +
				(b210.z * 3.0f * w2 * u) + (b120.z * 3.0f * w * u2) + 
				(b201.z * 3.0f * w2 * v) + (b021.z * 3.0f * u2 * v) + 
				(b102.z * 3.0f * w * v2) + (b012.z * 3.0f * u * v2) + 
				(b111.z * 6.0f * w * u * v);
			/* normals */
			nout[p].x = (n200.x * w2) + (n020.x * u2) + (n002.x * v2) +
				(n110.x * w * u) + (n011.x * u * v) + (n101.x * w * v);
			nout[p].y = (n200.y * w2) + (n020.y * u2) + (n002.y * v2) +
				(n110.y * w * u) + (n011.y * u * v) + (n101.y * w * v);
			nout[p].z = (n200.z * w2) + (n020.z * u2) + (n002.z * v2) +
				(n110.z * w * u) + (n011.z * u * v) + (n101.z * w * v);
			normalise( &nout[p] );
			p++;
		}
		switch( nsides ) {
		case 1:
			v += 1.0f;
			if ( step > 1 ) {
				step = 1;
				n = 1;
			} else
				step--;
			break;
		case 2:
			v += inc;
			n = 1;
			step--;
			break;
		case 3:
			v += inc;
			n--;
			step--;
			break;
		}
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pnBezierPatch4 -- construct quad' patch points and normals
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnBezierPatch4 ( 
	Point_f3d	*net, 
	int			 nsides,
	int			 lod, 
	Point_f3d	*pout,
	Point_f3d	*nout
	)
{
	float		 u,v;
	float		 inc;
	int			 p;
	int			 i, j, n;

	/* init */
	inc   = 1.0f/(float)(lod + 1);
	n     = (lod + 2);
	p     = 0;
	for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
		for ( j=0, v=0.0f; j<n; j++, v+=inc ) {
			pout[p] = bez_patch_pt( net, u, v );
			nout[p] = bez_normal( net, u, v );
			p++;
		}
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pnTessellate -- create triangles for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnTessellate ( 
	Point_f3d	*p,
	int			 lod,
	short		*tout
	)
{
	int			 i, j, t;
	int			 levels;

	t = 0;
	i = 0;
	levels = (lod + 1);
	while( levels > 0 ) {
		for ( j=0; j<levels; j++, i++ ) {
			*tout = i;				tout++;
			*tout = i+1;			tout++;
			*tout = i+levels+1;		tout++;
			t++;
			*tout = i+1;			tout++;
			*tout = i+levels+2;		tout++;
			*tout = i+levels+1;		tout++;
			t++;
		}
		/* discard last triangle */
		tout -= 3;
		t--;
		i++;
		levels--;
	}

	return(t);
}


/******************************************************************************
 * Function:
 * pnTessellate3 -- create triangles for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnTessellate3 ( 
	Point_f3d	*p,
	int			 nsides,
	int			 lod,
	short		*tout
	)
{
	int			 i, j, t;
	int			 c;
	int			 levels;

	t = 0;
	i = 0;
	levels = (lod + 1);
	switch( nsides ) {
	case 1:
		c = (levels + 1);
		for ( j=0; j<levels; j++, i++ ) {
			*tout = i;		tout++;
			*tout = i+1;	tout++;
			*tout = c;		tout++;
			t++;
		}
		break;
	case 2:
		/* do first triangle */
		*tout = 0;					tout++;
		*tout = 1;					tout++;
		*tout = (levels + 1);		tout++;
		t++;
		for ( j=1, i=1; j<levels; j++, i++ ) {
			*tout = i;				tout++;
			*tout = i+1;			tout++;
			*tout = i+levels;		tout++;
			t++;
			*tout = i+levels;		tout++;
			*tout = i+1;			tout++;
			*tout = i+levels+1;		tout++;
			t++;
		}
		break;
	case 3:
		while( levels > 0 ) {
			for ( j=0; j<levels; j++, i++ ) {
				*tout = i;				tout++;
				*tout = i+1;			tout++;
				*tout = i+levels+1;		tout++;
				t++;
				*tout = i+1;			tout++;
				*tout = i+levels+2;		tout++;
				*tout = i+levels+1;		tout++;
				t++;
			}
			/* discard last triangle */
			tout -= 3;
			t--;
			i++;
			levels--;
		}
		break;
	}

	return(t);
}


/******************************************************************************
 * Function:
 * pnTessellate4 -- create triangles for a quad' patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnTessellate4 ( 
	Point_f3d	*p,
	int			 nsides,
	int			 lod,
	short		*tout
	)
{
	int			 i, j, t;
	int			 s;
	int			 levels;

	t = 0;
	i = 0;
	levels = (lod + 1);
	for ( s=0; s<levels; s++ ) {
		for ( j=0; j<levels; j++, i++ ) {
			*tout = i;				tout++;
			*tout = i+levels+1;		tout++;
			*tout = i+1;			tout++;
			t++;
			*tout = i+levels+2;		tout++;
			*tout = i+1;			tout++;
			*tout = i+levels+1;		tout++;
			t++;
		}
		i++;
	}

	return(t);
}


/******************************************************************************
 * Function:
 * pnInterpolate -- interpolate values for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnInterpolate (
	float		*values,
	int			 nvalues,
	int			 lod,
	float		*vout
	)
{
	float		 u,v;
	float		 inc;
	int			 p;
	int			 j, k;
	int			 i, n;
	int			 step;

	/* init */
	inc  = 1.0f/(float)(lod + 1);
	u    = 0.0f;
	v    = 0.0f;
	n    = (lod + 2);
	step = n;
	p    = 0;
	while( step > 0 ) {
		for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
			/* interpolation of values (such as texture and colour) */
			for ( j=0, k=0; j<nvalues; j++, k+=3, p++ )
				vout[p] = values[k] + ((values[(k+1)] - values[k]) * u) + ((values[(k+2)] - values[k]) * v);
		}
		v += inc;
		n--;
		step--;
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pnInterpolate -- interpolate values for a triangle patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnInterpolate3 (
	float		*values,
	int			 nvalues,
	int			 nsides,
	int			 lod,
	float		*vout
	)
{
	float		 u,v;
	float		 inc;
	int			 p;
	int			 j, k;
	int			 i, n;
	int			 step;

	/* init */
	inc  = 1.0f/(float)(lod + 1);
	u    = 0.0f;
	v    = 0.0f;
	n    = (lod + 2);
	step = n;
	p    = 0;
	while( step > 0 ) {
		for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
			/* interpolation of values (such as texture and colour) */
			for ( j=0, k=0; j<nvalues; j++, k+=3, p++ )
				vout[p] = values[k] + ((values[(k+1)] - values[k]) * u) + ((values[(k+2)] - values[k]) * v);
		}
		switch( nsides ) {
		case 1:
			v += 1.0f;
			if ( step > 1 ) {
				step = 1;
				n = 1;
			} else
				step--;
			break;
		case 2:
			v += inc;
			n = 1;
			step--;
			break;
		case 3:
			v += inc;
			n--;
			step--;
			break;
		}
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pnInterpolate4 -- interpolate values for a quad' patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pnInterpolate4 (
	float		*values,
	int			 nvalues,
	int			 nsides,
	int			 lod,
	float		*vout
	)
{
	float		 u,v;
	float		 inc;
	int			 p;
	int			 j, k, t;
	int			 i, n;
	float		 v1,v2;

	/* init */
	inc  = 1.0f/(float)(lod + 1);
	u    = 0.0f;
	v    = 0.0f;
	n    = (lod + 2);
	p    = 0;
	for ( i=0, u=0.0f; i<n; i++, u+=inc ) {
		for ( j=0, v=0.0f; j<n; j++, v+=inc ) {
			/* interpolation of values (such as texture and colour) */
			for ( t=0, k=0; t<nvalues; t++, k+=4, p++ ) {
				v1 = values[k] + ((values[(k+1)] - values[k]) * u);
				v2 = values[(k+2)] + ((values[(k+3)] - values[(k+2)]) * u);
				vout[p] = v1 + ((v2 - v1) * v);
			}
		}
	}

	return(p);
}


/******************************************************************************
 * Function:
 * pn_tanpts -- calculate tangent points for a control grid
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pn_tanpts (
	Point_f3d	*p1,
	Point_f3d	*p2,
	Point_f3d	*n1,
	Point_f3d	*n2,
	Point_f3d	*out
	)
{
	float		 dot;
	float		 x, y, z;

	/* get vector */
	x = (p2->x - p1->x);
	y = (p2->y - p1->y);
	z = (p2->z - p1->z);

#define OFFSET 0.33333f

	dot = ((x * n1->x) + (y * n1->y) + (z * n1->z));
	out[0].x = (((p1->x * 2.0f) + p2->x) - (n1->x * dot)) * OFFSET;
	out[0].y = (((p1->y * 2.0f) + p2->y) - (n1->y * dot)) * OFFSET;
	out[0].z = (((p1->z * 2.0f) + p2->z) - (n1->z * dot)) * OFFSET;

	dot = ((x * n2->x) + (y * n2->y) + (z * n2->z));
	out[1].x = (((p2->x * 2.0f) + p1->x) + (n2->x * dot)) * OFFSET;
	out[1].y = (((p2->y * 2.0f) + p1->y) + (n2->y * dot)) * OFFSET;
	out[1].z = (((p2->z * 2.0f) + p1->z) + (n2->z * dot)) * OFFSET;

	return(2);
}


/******************************************************************************
 * Function:
 * pn_normal -- calculate a quadratic normal for a control grid
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pn_normal (
	Point_f3d	*p1,
	Point_f3d	*p2,
	Point_f3d	*n1,
	Point_f3d	*n2,
	Point_f3d	*out
	)
{
	float		 dot, ndot;
	float		 x, y, z;
	float		 nx, ny, nz;
	float		 v;

	/* get vector */
	x = (p2->x - p1->x);
	y = (p2->y - p1->y);
	z = (p2->z - p1->z);

	/* .. */
	nx = (n1->x + n2->x);
	ny = (n1->y + n2->y);
	nz = (n1->z + n2->z);

	/* .. */
	ndot = ((x * nx) + (y * ny) + (z * nz));
	dot  = ((x * x) + (y * y) + (z * z));
	v = (ndot/dot) * 2.0f;
	out[0].x = nx - (v * x);
	out[0].y = ny - (v * y);
	out[0].z = nz - (v * z);

	normalise( &out[0] );

	return(1);
}


/******************************************************************************
 * Function:
 * pn_centre -- calculate centre point for a PN triangle patch control grid
 * 
 * Description:
 *
 * Returns:
 * 
 */

int pn_centre (
	Point_f3d	*p,
	Point_f3d	*out
	)
{
	Point_f3d	 e,v;

#define PUSHOUT				0.5f

	e.x = p[1].x + p[2].x + p[4].x + p[5].x + p[7].x + p[8].x;
	e.y = p[1].y + p[2].y + p[4].y + p[5].y + p[7].y + p[8].y;
	e.z = p[1].z + p[2].z + p[4].z + p[5].z + p[7].z + p[8].z;
	e.x *= 0.166666f; // * 1/6
	e.y *= 0.166666f; // * 1/6
	e.z *= 0.166666f; // * 1/6
	v.x = p[0].x + p[3].x + p[6].x;
	v.y = p[0].y + p[3].y + p[6].y;
	v.z = p[0].z + p[3].z + p[6].z;
	v.x *= 0.333333f; // * 1/3
	v.y *= 0.333333f; // * 1/3
	v.z *= 0.333333f; // * 1/3
	out->x = e.x + ((e.x - v.x) * PUSHOUT);
	out->y = e.y + ((e.y - v.y) * PUSHOUT);
	out->z = e.z + ((e.z - v.z) * PUSHOUT);

	return(1);
}


/******************************************************************************
 * Function:
 * bez_pt -- calculate a point on a bezier curve with 4 control points
 * 
 * Description:
 *
 * Returns:
 * 
 */

Point_f3d bez_pt ( 
	Point_f3d  *b1, 
	Point_f3d  *b2,
	Point_f3d  *b3,
	Point_f3d  *b4,
	float		len
	)
{
	Point_f3d	pt;

	Point_f3d		*p0, *p1, *p2, *p3;
	float			 t,t2,t3;

	/* init */
	p0 = b1;
	p1 = b2;
	p2 = b3;
	p3 = b4;
	t  = len;
	t2 = t * t;
	t3 = t2 * t;

	/* calc' */
	pt.x = p0->x + (p0->x * -3.0f * t) + (p1->x * 3.0f * t) + 
			(p0->x *  3.0f * t2) + (p1->x * -6.0f * t2) + (p2->x *  3.0f * t2) + 
			(p0->x * -1.0f * t3) + (p1->x *  3.0f * t3) + (p2->x * -3.0f * t3) + (p3->x * t3);
	pt.y = p0->y + (p0->y * -3.0f * t) + (p1->y * 3.0f * t) + 
			(p0->y *  3.0f * t2) + (p1->y * -6.0f * t2) + (p2->y *  3.0f * t2) + 
			(p0->y * -1.0f * t3) + (p1->y *  3.0f * t3) + (p2->y * -3.0f * t3) + (p3->y * t3);
	pt.z = p0->z + (p0->z * -3.0f * t) + (p1->z * 3.0f * t) + 
			(p0->z *  3.0f * t2) + (p1->z * -6.0f * t2) + (p2->z *  3.0f * t2) + 
			(p0->z * -1.0f * t3) + (p1->z *  3.0f * t3) + (p2->z * -3.0f * t3) + (p3->z * t3);

	return( pt );
}


/******************************************************************************
 * Function:
 * bez_patch_pt -- calculate a point on quad' patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

Point_f3d bez_patch_pt ( 
	Point_f3d  *net, 
	float		u,
	float		v
	)
{
	Point_f3d	 b1, b2, b3, b4, b5, b6, b7, b8;
	Point_f3d	 b9, b10, b11, b12, b13, b14, b15, b16;
	Point_f3d	 p1, p2, p3, p4, p5;

	/* init */
	b1  = net[0];
	b2  = net[1];
	b3  = net[2];
	b4  = net[3];
	b5  = net[4];
	b6  = net[5];
	b7  = net[6];
	b8  = net[7];
	b9  = net[8];
	b10 = net[9];
	b11 = net[10];
	b12 = net[11];
	b13 = net[12];
	b14 = net[13];
	b15 = net[14];
	b16 = net[15];

	/* patch side a */
	p1 = bez_pt( &b1, &b2, &b3, &b4, u );
	/* patch side b */
	p2 = bez_pt( &b5, &b6, &b7, &b8, u );
	/* patch side c */
	p3 = bez_pt( &b9, &b10, &b11, &b12, u );
	/* patch side d */
	p4 = bez_pt( &b13, &b14, &b15, &b16, u );
	p5 = bez_pt( &p1, &p2, &p3, &p4, v );

	return( p5 );
}


/******************************************************************************
 * Function:
 * bez_normal -- calculate a normal on quad' patch
 * 
 * Description:
 *
 * Returns:
 * 
 */

Point_f3d bez_normal ( 
	Point_f3d  *net, 
	float		u,
	float		v
	)
{
	Point_f3d	 n1, n2, n3, n4;
	Point_f3d	 c1, c2, c3, c4;
	Point_f3d	 a, b, c, d;
	Point_f3d	 u1, u2, u3, v1, v2, v3;
	Point_f3d	 n;
	float		 uf, vf;

	/* init */
	n1 = net[16];
	n2 = net[17];
	n3 = net[18];
	n4 = net[19];
	c1 = net[20];
	c2 = net[21];
	c3 = net[22];
	c4 = net[23];
	
	/* u */
	if ( u == 0.5f ) {
		u1 = c1;
		u2 = c3;
	} else {
		if ( u < 0.5f ) {
			a  = n1; 
			b  = c1;
			uf = (u * 2.0f);
			c  = n3;
			d  = c3;
		} else {
			a  = c1;
			b  = n2;
			uf = (u - 0.5f) * 2.0f;
			c  = c3;
			d  = n4;
		}
		u1.x = a.x + (b.x - a.x) * uf;
		u1.y = a.y + (b.y - a.y) * uf;
		u1.z = a.z + (b.z - a.z) * uf;
		u2.x = c.x + (d.x - c.x) * uf;
		u2.y = c.y + (d.y - c.y) * uf;
		u2.z = c.z + (d.z - c.z) * uf;
	}

	/* v */
	if ( v == 0.5f ) {
		v1 = c4;
		v2 = c2;
	} else {
		if ( v < 0.5f ) {
			a  = n1; 
			b  = c4;
			vf = (v * 2.0f);
			c  = n2;
			d  = c2;
		} else {
			a  = c4;
			b  = n3;
			vf = (v - 0.5f) * 2.0f;
			c  = c2;
			d  = n4;
		}
		v1.x = a.x + (b.x - a.x) * vf;
		v1.y = a.y + (b.y - a.y) * vf;
		v1.z = a.z + (b.z - a.z) * vf;
		v2.x = c.x + (d.x - c.x) * vf;
		v2.y = c.y + (d.y - c.y) * vf;
		v2.z = c.z + (d.z - c.z) * vf;
	}

	u3.x = u1.x + (u2.x - u1.x) * v;
	u3.y = u1.y + (u2.y - u1.y) * v;
	u3.z = u1.z + (u2.z - u1.z) * v;
	v3.x = v1.x + (v2.x - v1.x) * u;
	v3.y = v1.y + (v2.y - v1.y) * u;
	v3.z = v1.z + (v2.z - v1.z) * u;

	// add
	n.x = (u3.x + v3.x); 
	n.y = (u3.y + v3.y); 
	n.z = (u3.z + v3.z); 
	normalise( &n );

	return( n );
}


/******************************************************************************
 * Function:
 * lenpoint -- get a point along a vector
 * 
 * Description:
 *
 * Returns:
 * 
 */

Point_f3d lenpoint(
	Point_f3d	*p1,
	Point_f3d	*p2,
	float		 len
	)
{
	Point_f3d	pt;
	float		x, y, z;

	x = (p2->x - p1->x) * len;
	y = (p2->y - p1->y) * len;
	z = (p2->z - p1->z) * len;

	pt.x = (p1->x + x);
	pt.y = (p1->y + y);
	pt.z = (p1->z + z);

	return( pt );
}


/******************************************************************************
 * Function:
 * normalise -- normalise a vector
 * 
 * Description:
 *
 * Returns:
 * 
 */

void normalise ( 
	Point_f3d  *n 
	)
{
	float		nlen;

	nlen = 1.0f/(float)sqrt((double) ((n->x * n->x) + (n->y * n->y) + (n->z * n->z)) );
	n->x *= nlen;
	n->y *= nlen;
	n->z *= nlen;
}


