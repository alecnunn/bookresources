
#ifndef _POLYCONVERTER_H
#define _POLYCONVERTER_H

#include <vector>
#include "MathStructs.h"

class CPolygon;

// Converts a triangle index array and vertex array to our own custom polygon structures
bool ConvertTrisToPolys(	DWORD dwTriCount, DWORD dwVertexCount, DWORD* pIndices, CVector* pVertices,
							std::vector<CPolygon*>& Polygons );

#endif	// _POLYCONVERTER_H