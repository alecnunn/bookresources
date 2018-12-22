/*============================================================================
	Title: Raster.h
	Module: Pi/Raster
	Author: Ignacio Castaño
	Date: 14/01/2003
	Description: Triangle rasterization using affine interpolation.
============================================================================*/

/*----------------------------------------------------------------------------
	Includes
----------------------------------------------------------------------------*/

#include "Raster.h"
//#include "Templates.h"		// piSwap
//#include "MathLib.h"


/*----------------------------------------------------------------------------
	Package Implementation:
----------------------------------------------------------------------------*/

//IMPLEMENT_PACKAGE(Raster);


/*----------------------------------------------------------------------------
	Definitions
----------------------------------------------------------------------------*/

#define RA_EPSILON		0.00001f


/*----------------------------------------------------------------------------
	Structs
----------------------------------------------------------------------------*/

/** Structure to interpolate parameters along the edges */
struct Edge {
	
	void CalcDeltas( const RA_Vertex &top, const RA_Vertex &bot );
	void Step( void );
	void Clip( int y );
	void Interpolate( RA_Vertex &v, float m );

	RA_Vertex	v;
	RA_Vertex	dv;		// dy
};


/** Structure to interpolate parameters along the spans */
struct Span {

	void CalcDeltas( const RA_Vertex &left, const RA_Vertex &right );
	bool Clip( int w0, int w1 );
	void Draw( RA_Feature * dst, const RA_Vertex &dy, RA_SAMPLING_CALLBACK callback, void * param );

	int tx, tw;
	RA_Vertex	v;
	RA_Vertex	dv;		// dx
};



/*----------------------------------------------------------------------------
	Statics
----------------------------------------------------------------------------*/

/** Compute increment */
static inline void delta( float &d, float bot, float top, float ih ) {
	d = (bot - top) * ih;
}
static inline void delta( Vec3 &d, const Vec3 &bot, const Vec3 &top, float ih ) {
	delta( d.x, bot.x, top.x, ih );
	delta( d.y, bot.y, top.y, ih );
	delta( d.z, bot.z, top.z, ih );
}

/** Multiply and add */
static inline void mad( float &m, float top, float delta, float step ) {
	m = top + delta * step;
}
static inline void mad( Vec3 &m, const Vec3 &top, const Vec3 &delta, float step ) {
	mad( m.x, top.x, delta.x, step );
	mad( m.y, top.y, delta.y, step );
	mad( m.z, top.z, delta.z, step );
}
static inline void mad( RA_Vertex &dst, const RA_Vertex &src, const RA_Vertex &delta, float step ) {
	mad( dst.pos, src.pos, delta.pos, step );
	mad( dst.nor, src.nor, delta.nor, step );
	mad( dst.tan, src.tan, delta.tan, step );
	mad( dst.bin, src.bin, delta.bin, step );
	mad( dst.tex, src.tex, delta.tex, step );
	mad( dst.tx1, src.tx1, delta.tx1, step );
	mad( dst.col, src.col, delta.col, step );
	mad( dst.bar, src.bar, delta.bar, step );
}

/** Increment */
static inline void inc( float &v, float d ) {
	v += d;
}
static inline void inc( Vec3 &v, const Vec3 &d ) {
	inc( v.x, d.x );
	inc( v.y, d.y );
	inc( v.z, d.z );
}

static inline void sub( Vec3 &dst, const Vec3 &a, const Vec3 &b ) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
}
static inline void sub( RA_Vertex &dst, const RA_Vertex &a, const RA_Vertex &b ) {
	sub( dst.pos, a.pos, b.pos );
	sub( dst.nor, a.nor, b.nor );
	sub( dst.tan, a.tan, b.tan );
	sub( dst.bin, a.bin, b.bin );
	sub( dst.tex, a.tex, b.tex );
	sub( dst.tx1, a.tx1, b.tx1 );
	sub( dst.col, a.col, b.col );
	sub( dst.bar, a.bar, b.bar );
}


/*----------------------------------------------------------------------------
	Methods
----------------------------------------------------------------------------*/

/** Calculate vertical increments along the edge */
void Edge::CalcDeltas( const RA_Vertex &top, const RA_Vertex &bot ) {

	// edge deltas
	float ith = 1.0f / (bot.tex[1] - top.tex[1]);
	delta( dv.pos, bot.pos, top.pos, ith );
	delta( dv.nor, bot.nor, top.nor, ith );
	delta( dv.tan, bot.tan, top.tan, ith );
	delta( dv.bin, bot.bin, top.bin, ith );
	delta( dv.tex, bot.tex, top.tex, ith );
	delta( dv.tx1, bot.tx1, top.tx1, ith );
	delta( dv.col, bot.col, top.col, ith );
	delta( dv.bar, bot.bar, top.bar, ith );


	// sub pixel adjustments
	int ity = (int)ceil( top.tex[1] );
	float typrestep = (float)ity - top.tex[1];
	mad( v.pos, top.pos, dv.pos, typrestep );
	mad( v.tex, top.tex, dv.tex, typrestep );
	mad( v.nor, top.nor, dv.nor, typrestep );
	mad( v.tan, top.tan, dv.tan, typrestep );
	mad( v.bin, top.bin, dv.bin, typrestep );
	mad( v.tx1, top.tx1, dv.tx1, typrestep );
	mad( v.col, top.col, dv.col, typrestep );
	mad( v.bar, top.bar, dv.bar, typrestep );
}


/** Perform a step along the edge */
void Edge::Step( void ) {
	inc( v.pos, dv.pos );	// linear or cubic.
	inc( v.tex, dv.tex );	// linear, but necessary.
	inc( v.nor, dv.nor );	// linear or quadratic.
	inc( v.tan, dv.tan );	// linear only?
	inc( v.bin, dv.bin );	// linear only?
	inc( v.tx1, dv.tx1 );	// linear.
	inc( v.col, dv.col );	// linear.
	inc( v.bar, dv.bar );	// linear and necessary.
}


/** Interpolate parameters along the edge */
void Edge::Interpolate( RA_Vertex &mv, float m ) {
	mad( mv.pos, v.pos, dv.pos, m );
	mad( mv.tex, v.tex, dv.tex, m );
	mad( mv.nor, v.nor, dv.nor, m );
	mad( mv.tan, v.tan, dv.tan, m );
	mad( mv.bin, v.bin, dv.bin, m );
	mad( mv.tx1, v.tx1, dv.tx1, m );
	mad( mv.col, v.col, dv.col, m );
	mad( mv.bar, v.bar, dv.bar, m );
}


/** Clip to the top. */
void Edge::Clip( int y ) {
	mad( v.pos, v.pos, dv.pos, (float)-y );
	mad( v.tex, v.tex, dv.tex, (float)-y );
	mad( v.nor, v.nor, dv.nor, (float)-y );
	mad( v.tan, v.tan, dv.tan, (float)-y );
	mad( v.bin, v.bin, dv.bin, (float)-y );
	mad( v.tx1, v.tx1, dv.tx1, (float)-y );
	mad( v.col, v.col, dv.col, (float)-y );
	mad( v.bar, v.bar, dv.bar, (float)-y );
}


/** Calculate horizontal increments along the edge */
void Span::CalcDeltas( const RA_Vertex &L, const RA_Vertex &R ) {

	tx = (int)ceil( L.tex[0] );
	tw = (int)ceil( R.tex[0] ) - tx;

	float itw = 1 / (R.tex[0] - L.tex[0]);
	delta( dv.pos, R.pos, L.pos, itw );
	delta( dv.tex, R.tex, L.tex, itw );
	delta( dv.nor, R.nor, L.nor, itw );
	delta( dv.tan, R.tan, L.tan, itw );
	delta( dv.bin, R.bin, L.bin, itw );
	delta( dv.tx1, R.tx1, L.tx1, itw );
	delta( dv.col, R.col, L.col, itw );
	delta( dv.bar, R.bar, L.bar, itw );

	float txprestep = (float) tx - L.tex[0];
	mad( v.pos, L.pos, dv.pos, txprestep );
	mad( v.tex, L.tex, dv.tex, txprestep );
	mad( v.nor, L.nor, dv.nor, txprestep );
	mad( v.tan, L.tan, dv.tan, txprestep );
	mad( v.bin, L.bin, dv.bin, txprestep );
	mad( v.tx1, L.tx1, dv.tx1, txprestep );
	mad( v.col, L.col, dv.col, txprestep );
	mad( v.bar, L.bar, dv.bar, txprestep );
}


/** Clip the span */
bool Span::Clip( int w0, int w1 ) {

	if( tx > w1 || tx+tw<w0 || tw<=0 ) 
		return false;

	if( tx < w0 ) { 
		int d = tx-w0;
		tx = w0;
		tw += d;
		mad( v.pos, v.pos, dv.pos, (float)-d );
		mad( v.tex, v.tex, dv.tex, (float)-d );
		mad( v.nor, v.nor, dv.nor, (float)-d );
		mad( v.tan, v.tan, dv.tan, (float)-d );
		mad( v.bin, v.bin, dv.bin, (float)-d );
		mad( v.tx1, v.tx1, dv.tx1, (float)-d );
		mad( v.col, v.col, dv.col, (float)-d );
		mad( v.bar, v.bar, dv.bar, (float)-d );
	}
	if( tx+tw > w1 ) {
		tw = w1-tx;
	}
	return true;
}


/** Draw the horizontal span */
void Span::Draw( RA_Feature * dst, const RA_Vertex &dy, RA_SAMPLING_CALLBACK callback, void * param ) {

	dst += tx;
	
	while( tw-- ) {

		callback( param, &v, &dv, &dy, dst );

		inc( v.pos, dv.pos );
		inc( v.tex, dv.tex );
		inc( v.nor, dv.nor );
		inc( v.tan, dv.tan );
		inc( v.bin, dv.bin );
		inc( v.tx1, dv.tx1 );
		inc( v.col, dv.col );
		inc( v.bar, dv.bar );
		dst++;
	}
}


/*void ComputeDeltaY( dy, v[a], m, dx, w, h ) {
	
	// Compute midle vertex:
	RA_Vertex m;
	e[0].Interpolate( m, v[b].tex[1] - v[a].tex[1] );

	// total x and y increments:
	float w = m.tex[0] - v[a].tex[0];
	float h = m.tex[0] - v[a].tex[0];

	// Compute deltas of the longest span:
	Span longest; 
	longest.CalcDeltas( v[b], m );

	mad( m
	
}*/


/*----------------------------------------------------------------------------
	More statics
----------------------------------------------------------------------------*/

/** */
static void DrawLinearTriangle( RA_Image * img, RA_Triangle v, RA_SAMPLING_CALLBACK callback, void * param ) {

	int a=0, b=1, c=2;

	// swap indices:
	if( v[a].tex[1] > v[b].tex[1] ) piSwap( a, b );
	if( v[a].tex[1] > v[c].tex[1] ) piSwap( a, c );
	if( v[b].tex[1] > v[c].tex[1] ) piSwap( b, c );
	
	Edge e[3];
	e[0].CalcDeltas( v[a], v[c] );
	e[1].CalcDeltas( v[a], v[b] );
	e[2].CalcDeltas( v[b], v[c] );

	if( fabs(e[1].dv.tex[0]-e[2].dv.tex[0]) < RA_EPSILON ) return;		// degenerate triangle.

	float middle_x = v[a].tex[0] + e[0].dv.tex[0] * (v[b].tex[1] - v[a].tex[1]);


	// Compute DY
	RA_Vertex m;
	e[0].Interpolate( m, v[b].tex[1] - v[a].tex[1] );
	
	Span longest; longest.CalcDeltas( v[b], m );
	RA_Vertex dy;
	mad( dy, e[0].dv, longest.dv, (v[a].tex[0] - v[c].tex[0])/(v[c].tex[1] - v[a].tex[1]) );

	// DX could be computed here for the whole triangle, but is instead
	// computed again on each scanline.



	int y = (int)ceil( v[a].tex[1] );
	if( y < 0 ) {
		e[0].Clip( y );
		e[1].Clip( y );
		
		int yc = (int) ceil( v[b].tex[1] );
		if( yc<0 ) e[2].Clip( yc );

		y = 0;
	}
	RA_Feature * dst = img->features + y * img->width;
	
	// for each section of the triangle
	for( int i=0; i<2; i++ ) {
		
		// calculate section parameters
		int top_y, L, R;
		if( i==0 ){
			top_y = (int)ceil( v[b].tex[1] );
			if( top_y > img->height ) top_y = img->height;
			if( middle_x > v[b].tex[0] ) { L = 1; R = 0; }
			else						 { L = 0; R = 1; }
		}
		else {
			top_y = (int)ceil(v[c].tex[1]);
			if( top_y > img->height ) top_y = img->height;
			if( middle_x > v[b].tex[0] ) { L = 2; R = 0; }
			else						 { L = 0; R = 2; }
		}

		// do this section
		while( y < top_y ) {

			Span s;
			s.CalcDeltas( e[L].v, e[R].v );

			if( s.Clip( 0, img->width ) )
				s.Draw( dst, dy, callback, param );

			e[L].Step();
			e[R].Step();
			dst += img->width;
			y++;
		}
	}
}




/*----------------------------------------------------------------------------
	Public functions
----------------------------------------------------------------------------*/

/** Process the given triangle. */
RASTER_API void RA_Process( RA_Image * img, RA_Triangle v, int flags, RA_SAMPLING_CALLBACK cb, void * param ) {

	if( flags==0 ) {
		// Set barycentric coordinates
		v[0].bar.Set( 1, 0, 0 );
		v[1].bar.Set( 0, 1, 0 );
		v[2].bar.Set( 0, 0, 1 );

		DrawLinearTriangle( img, v, cb, param );
	}
}