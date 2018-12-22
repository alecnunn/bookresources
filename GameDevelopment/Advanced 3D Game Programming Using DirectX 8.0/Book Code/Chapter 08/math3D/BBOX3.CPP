/******************************************************************************
 * Title: bbox3.cpp
 * Desc: code for axis aligned bounding boxes
 *       
 * Author: Cuban
 * Started: 2/3/99
 *****************************************************************************/

#include "bbox3.h"
#include "lineSeg3.h"

bbox3::bbox3()
{
	min = point3( 50000, 50000, 50000 );
	max = point3( -50000, -50000, -50000 );

	// top, bottom, back, front, right, left
	planeList[0] = plane3( point3( 0.f, 1.f, 0.f ), max );
	planeList[1] = plane3( point3( 0.f,-1.f, 0.f ), min );
	planeList[2] = plane3( point3( 0.f, 0.f, 1.f ), max );
	planeList[3] = plane3( point3( 0.f, 0.f,-1.f ), min );
	planeList[4] = plane3( point3( 1.f, 0.f, 0.f ), max );
	planeList[5] = plane3( point3(-1.f, 0.f, 0.f ), min );
	plane3 planeList[6];


}


void bbox3::UpdateBounds( const point3 &in )
{
	if( in.x > max.x ) max.x = in.x;
	if( in.y > max.y ) max.y = in.y;
	if( in.z > max.z ) max.z = in.z;

	if( in.x < min.x ) min.x = in.x;
	if( in.y < min.y ) min.y = in.y;
	if( in.z < min.z ) min.z = in.z;
}


void bbox3::ReCalcPlanes()
{
	planeList[0].d = planeList[0].n * max;
	planeList[1].d = planeList[1].n * min;
	planeList[2].d = planeList[2].n * max;
	planeList[3].d = planeList[3].n * min;
	planeList[4].d = planeList[4].n * max;
	planeList[5].d = planeList[5].n * min;
}


bool bbox3::TestLSeg( lineSeg3 *inLine, point3 *hitLoc )
{
	lineSeg3 front, back, line = *inLine;
	pListLoc loc;
	for( int i=0; i<6; i++ )
	{
		loc = planeList[i].TestPList( &line.v0, 2 );
		switch( loc )
		{
		case PLIST_FRONT:
			// fail.
			return false;
			break;

		case PLIST_BACK:
			// continue on.  basically do nothing.
			break;

		case PLIST_SPLIT:
			// split and keep the back part
			planeList[i].Split( line, &front, &back );
			line = back;
			break;

		case PLIST_COPLANAR:
			// fail.
			return false;
			break;

		}
	}
	*hitLoc = line.v0;
	return true;

}