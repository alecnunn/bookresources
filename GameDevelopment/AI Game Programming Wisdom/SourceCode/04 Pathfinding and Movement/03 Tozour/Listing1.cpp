/* Copyright (C) Paul Tozour, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Paul Tozour, 2001"
 */
function Subdivide(Polygon p, Obstacle o)
{
	if p is totally inside o { delete p and return }
	if p is totally outside o { return }

	// we now know p is partly inside and partly outside o
	if the surface area of p is below a minimum threshold
	{ delete p and return }

	// we now subdivide and call this function recursively
	// for each new node
	create new sub-polygons from p (one poly per vertex)
	for each sub-polygon
	{ Subdivide(the sub-polygon,o) }
}
