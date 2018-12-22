/*============================================================================
	Title: TriMesh.cpp
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: A generic triangle mesh.
	Changes:
		07/10/2002 - Ignacio Castaño
			File added.

============================================================================*/

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file TriMesh.cpp
 * A generic triangle mesh.
**/


/*----------------------------------------------------------------------------
	Includes
----------------------------------------------------------------------------*/

#include "PAR.h"

#include "Vector.h"
#include "Sparse.h"

#include "TriMesh.h"





/*----------------------------------------------------------------------------
	Functions
----------------------------------------------------------------------------*/

/** Compute the area withing the given vectors. */
static REAL Vec3Area( const Vec3 &a, const Vec3 &b ) {
//	Vec3 c;
//	c.Cross( a, b );
//	return c.Length();
	REAL dot = Vec3DotProduct( a, b );
	return sqrt( a.LengthSquared() * b.LengthSquared() - SQ(dot) );
}


/** Project a point from the sphere to the plane. */
static void StereographicProjection( const Vec3 &in, Vec2 &out ) {
	REAL r2 = (in.z + 1) / (1 - in.z);
	out.x = 0.5 * in.x * (1 + r2);
	out.y = 0.5 * in.y * (1 + r2);
}

/** Project a point from the plane to the sphere. */
static void InverseStereographicProjection( const Vec2 &in, Vec3 &out ) {
	float r2 = SQ(in.x) + SQ(in.y);
	out.Set( in.x/(1+r2), in.y/(1+r2), r2/(1+r2)-0.5 );
}


/** Compute the Möbius transform of the given point. */
void BilinearTransform( Vec2 &z, const Vec2 &a, const Vec2 &b, const Vec2 &c, const Vec2 &d ) {
	Vec2 p, q;
	p.ComplexProduct( z, a );
	p.Add( p, b );
	q.ComplexProduct( z, c );
	q.Add( q, d );
	z.ComplexDivide( p, q );
}


/*----------------------------------------------------------------------------
	Methods
----------------------------------------------------------------------------*/

/* Create a TriMesh from an ImportMesh. */
bool PiTriMesh::Open( const IImportMesh * im ) {
	int i;

	int face_num = im->GetFaceNum();
	face_array.Resize( face_num );

	// Copy faces.
	for( i=0; i<face_num; i++ ) {
		face_array[i].v0 = im->GetFaceIndex( i, 0 );
		face_array[i].v1 = im->GetFaceIndex( i, 1 );
		face_array[i].v2 = im->GetFaceIndex( i, 2 );
	}

	int vertex_num = im->GetVertexNum();
	vertex_array.Resize( vertex_num );

	// Copy vertices.
	for( i=0; i<vertex_num; i++ ) {
		vertex_array[i].pos = im->GetVertexPosition( i );
		vertex_array[i].nor = im->GetVertexNormal( i );
		vertex_array[i].tex = im->GetVertexTexCoord( i );
		vertex_array[i].col = im->GetVertexColor( i );
	}

	ComputeBox();

	piDebug( "--- Mesh has %d vertices and %d faces.\n", vertex_num, face_num );

	return true;
}



/** Flip faces. Do this if the mesh has opposite handness. */
void PiTriMesh::FlipFaces( void ) {
	int face_num = face_array.Size();
	for( int i=0; i<face_num; i++ ) {
		piSwap( face_array[i].v1, face_array[i].v2 );
	}
}


/** Interchange Y and Z axis.*/
void PiTriMesh::InterchangeYZ( void ) {
	int vertex_num = vertex_array.Size();
	for( int i=0; i<vertex_num; i++ ) {
		piSwap( vertex_array[i].pos.y, vertex_array[i].pos.z );
	}

	piSwap( bounds.Mins.y, bounds.Mins.z );
	piSwap( bounds.Maxs.y, bounds.Maxs.z );

}


/** Scale mesh to fit the given box. Does not .*/
void PiTriMesh::FitCube( REAL size ) {

	// offset - scale
	Vec3 offset;
	bounds.GetCenter( offset );
	REAL scale = bounds.GetAbsMaxAxis();

	int vertex_num = vertex_array.Size();
	for( int i=0; i<vertex_num; i++ ) {
		vertex_array[i].pos -= offset;
		vertex_array[i].pos *= size/scale;
	}
}



/* Create the adjacency information. */
void PiTriMesh::BuildAdjacency( void ) {

	// Allocate raw edges
	int edge_max = 3 * face_array.Size();
	edge_array.Resize( edge_max );

	// Read raw edges
	for( int f=0; f<face_array.Size(); f++ ) {
		int e0 = 3*f + 0;
		int e1 = 3*f + 1;
		int e2 = 3*f + 2;

		edge_array[e0].v0 = face_array[f].v1;
		edge_array[e0].v1 = face_array[f].v2;
		edge_array[e0].f0 = -1;
		edge_array[e0].f1 = -1;

		edge_array[e1].v0 = face_array[f].v2;
		edge_array[e1].v1 = face_array[f].v0;
		edge_array[e1].f0 = -1;
		edge_array[e1].f1 = -1;

		edge_array[e2].v0 = face_array[f].v0;
		edge_array[e2].v1 = face_array[f].v1;
		edge_array[e2].f0 = -1;
		edge_array[e2].f1 = -1;
		
		face_array[f].e0 = e0;
		face_array[f].e1 = e1;
		face_array[f].e2 = e2;
	}


	int * xrefs = new int[edge_max];

	// Weld raw edges
	int edge_num = WeldEdges( edge_array, xrefs, edge_max );
	
	edge_array.Resize( edge_num );

	// Remap face indices and set edge faces.
	for( int i=0; i<face_array.Size(); i++ ) {
		face_array[i].e0 = xrefs[face_array[i].e0];
		face_array[i].e1 = xrefs[face_array[i].e1];
		face_array[i].e2 = xrefs[face_array[i].e2];

		if( edge_array[face_array[i].e0].v0==face_array[i].v1 )
			edge_array[face_array[i].e0].f0 = i;
		else
			edge_array[face_array[i].e0].f1 = i;

		if( edge_array[face_array[i].e1].v0==face_array[i].v2 )
			edge_array[face_array[i].e1].f0 = i;
		else
			edge_array[face_array[i].e1].f1 = i;

		if( edge_array[face_array[i].e2].v0==face_array[i].v0 )
			edge_array[face_array[i].e2].f0 = i;
		else
			edge_array[face_array[i].e2].f1 = i;
	}

	delete [] xrefs;

	piDebug( "--- Mesh has %d edges.\n", edge_num );
}


/* Fast and dirty normal computation. */
void PiTriMesh::ComputeNormals( void ) {

	int v, vertex_num = vertex_array.Size();

	for( v=0; v<vertex_num; v++ ) {
		vertex_array[v].nor.Set( Vec3Origin );
	}

	int f, face_num = face_array.Size();
	for( f=0; f<face_num; f++ ) {

		Vec3 A, B;
		A.Sub( vertex_array[face_array[f].v1].pos, vertex_array[face_array[f].v0].pos );
		B.Sub( vertex_array[face_array[f].v2].pos, vertex_array[face_array[f].v0].pos );
		face_array[f].normal.Cross( A, B );

		vertex_array[face_array[f].v0].nor += face_array[f].normal;
		vertex_array[face_array[f].v1].nor += face_array[f].normal;
		vertex_array[face_array[f].v2].nor += face_array[f].normal;

		face_array[f].normal.Normalize();
	}

	for( v=0; v<vertex_num; v++ ) {
		vertex_array[v].nor.Normalize();
	}

}



/* Determine if the mesh is closed. (Each edge is shared by two faces) */
bool PiTriMesh::IsClosed( void ) const {

	int n = edge_array.Size();
	for( int i=0; i<n; i++ ) {
		if( edge_array[i].f0==-1 || edge_array[i].f1==-1 )
			return false;
	}

	return true;
}


/* Conformal parameterization in the sphere. */
void PiTriMesh::ConformalSphericalMap( int v0, int v1, int v2 ) {

	int vertex_num = vertex_array.Size();
	int face_num = face_array.Size();
	int edge_num = edge_array.Size();

	SparseMatrix D( vertex_num );
	SparseVector a( vertex_num );
	SparseVector b( vertex_num );
	SparseVector x( vertex_num );
	SparseVector y( vertex_num );

	for( int e=0; e<edge_num; e++ ) {

		Vec3 &P = vertex_array[ edge_array[e].v0 ].pos;
		Vec3 &Q = vertex_array[ edge_array[e].v1 ].pos;

		Face &RPQ = face_array[edge_array[e].f0];		// faces
		Face &SPQ = face_array[edge_array[e].f1];

		int r, s;	// opposite indices.

		if( RPQ.e0==e ) r = RPQ.v0;
		else if( RPQ.e1==e ) r = RPQ.v1;
		else { piCheck( RPQ.e2==e ); r = RPQ.v2; }

		if( SPQ.e0==e ) s = SPQ.v0;
		else if( SPQ.e1==e ) s = SPQ.v1;
		else { piCheck( SPQ.e2==e ); s = SPQ.v2; }

		Vec3 &R = vertex_array[ r ].pos;
		Vec3 &S = vertex_array[ s ].pos;
		
		Vec3 RP, RQ, SP, SQ, PQ;

		RP.Sub( P, R );
		RQ.Sub( Q, R );
		SP.Sub( P, S );
		SQ.Sub( Q, S );
		PQ.Sub( Q, P );

		REAL cotR = Vec3DotProduct( RP, RQ ) / Vec3Area( RP, RQ );
		REAL cotS = Vec3DotProduct( SP, SQ ) / Vec3Area( SP, SQ );
		REAL Dpq = - 0.5 * ( cotR + cotS );

		// Insert value.
		D.SetElem( edge_array[e].v0, edge_array[e].v1, Dpq );
		D.SetElem( edge_array[e].v1, edge_array[e].v0, Dpq );
	}

	int v;

	// Fill the diagonal
	for( v=0; v<vertex_num; v++ ) {
		REAL Dpp = -D.AddColumn( v );
		D.SetElem( v, v, Dpp );
	}


	

	// You have to select one face!!! by now it's f = 0
	Face & face = face_array[0];

	a.Zero();
	b.Zero();


	Vec3 &A = vertex_array[face.v0].pos;
	Vec3 &B = vertex_array[face.v1].pos;
	Vec3 &C = vertex_array[face.v2].pos;

	Vec3 AB, AC, E, EC;
	AB.Sub( B, A );
	AC.Sub( C, A );

	REAL f = Vec3DotProduct( AC, AB ) / AB.LengthSquared();
	E.Mad( A, AB, f );

	EC.Sub( C, E );


	REAL scale = 1500;
	a[face.v0] = scale * -1/AB.Length();		// Q = A
	b[face.v0] = scale * (1-f)/EC.Length();

	a[face.v1] = scale * 1/AB.Length();		// Q = B
	b[face.v1] = scale * f/EC.Length();

	a[face.v2] = 0;							// Q = C
	b[face.v2] = scale * -1/EC.Length();




	// Initial solution
	x.Zero();
	y.Zero();

	piDebug( "--- Solving equation, please wait...\n" );

	// Solve the system
	int iter;
	iter = ConjugateGradientSolve( D, a, x, 0.00001f );
	piDebug( "--- Real part solved in %d iterations.\n", iter );
	iter = ConjugateGradientSolve( D, b, y, 0.00001f );
	piDebug( "--- Imaginary part solved in %d iterations.\n", iter );

	{
		Vec2 z0( x[v0], y[v0] );
		Vec2 z1( x[v1], y[v1] );
		Vec2 z2( x[v2], y[v2] );

		Vec2 a( 1, 0 );
		Vec2 b( -z0.x, -z0.y );
		Vec2 c;
		c.ComplexDivide( Vec2( z1.x-z0.x, z1.y-z0.y ), Vec2( z1.x-z2.x, z1.y-z2.y ) );
		Vec2 d;
		d.ComplexProduct( z2, c );
		d.Negate();


		// Generate texcoords
		for( v=0; v<vertex_num; v++ ) {

			Vec2 z( x[v], y[v] );

			BilinearTransform( z, a, b, c, d );
			InverseStereographicProjection( z, vertex_array[v].tex );
			vertex_array[v].tex.Normalize();
		}

		// point at infinity.
		vertex_array[v2].tex.Set( 0, 0, 1 );
	}
}



/* Weld edges. */
int PiTriMesh::WeldEdges( PiArray<Edge> &p, int * xrefs, int N ) {

	const int NIL = -1;									// linked list terminator symbol
	int	outputCount = 0;                                // # of output vertices
	int hashSize = NextPowerOfTwo(N);					// size of the hash table
	int * hashTable = new int[hashSize + N];			// hash table + linked list
	int * next = hashTable + hashSize;					// use bottom part as linked list

	memset( hashTable, NIL, hashSize*sizeof(int));		// init hash table (NIL = 0xFFFFFFFF so memset works)

	for (int i = 0; i < N; i++) {

		const Edge & e = p[i];
		uint32 hashValue = GetEdgeHashValue(e) & (hashSize-1);
		int offset = hashTable[hashValue];

		// traverse linked list
		while( offset != NIL && !EdgeEquiv( p[offset], e) )
			offset = next[offset];

		xrefs[i] = offset;

		// no match found - copy vertex & add to hash
		if( offset == NIL ) {

			// save xref
			xrefs[i] = outputCount;
            
			// copy vertex
			p[outputCount] = e;

			// link to hash table
			next[outputCount] = hashTable[hashValue];

			// update hash heads and increase output counter
			hashTable[hashValue] = outputCount++;
		}
	}

	// cleanup
	delete [] hashTable;

	// number of output vertices
	return outputCount;
}



/** Compute the bounding box. */
void PiTriMesh::ComputeBox( void ) {
	int vertex_num = vertex_array.Size();
	bounds.ClearBounds();
	for( int i=0; i<vertex_num; i++ ) {
		bounds.AddPointToBounds( vertex_array[i].pos );
	}
}


int PiTriMesh::GetLowestVertex( int axis ) {
	int vertex_num = vertex_array.Size();
	int v=0;
	for( int i=1; i<vertex_num; i++ ) {
		if( vertex_array[i].pos[axis] < vertex_array[v].pos[axis] )
			v = i;
	}
	return v;
}

int PiTriMesh::GetMediumVertex( int axis ) {
	int vertex_num = vertex_array.Size();
	int v=0;
	float c = bounds.GetCenter( axis );
	for( int i=1; i<vertex_num; i++ ) {
		if( fabs(vertex_array[i].pos[axis]-c) < fabs(vertex_array[v].pos[axis]-c) )
			v = i;
	}
	return v;
}

int PiTriMesh::GetHighestVertex( int axis ) {
	int vertex_num = vertex_array.Size();
	int v=0;
	for( int i=1; i<vertex_num; i++ ) {
		if( vertex_array[i].pos[axis] > vertex_array[v].pos[axis] )
			v = i;
	}
	return v;
}

