/* Copyright (C) Eric Lengyel, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Lengyel, 2000"
 */
#include "mtxlib.h"

class Frustum
{
	private:
		
		// Near and far plane distances
		float		nearDistance;
		float		farDistance;
		
		// Precalculated normal components
		float		leftRightX;
		float		leftRightZ;
		float		topBottomY;
		float		topBottomZ;
		
	public:
		
		// Constructor defines the frustum
		Frustum(float l, float a, float n, float f);
		
		// Intersection test returns true or false
		bool CylinderVisible(vector3 p1, vector3 p2, float radius) const;
};

Frustum::Frustum(float l, float a, float n, float f)
{
	// Save off near plane and far plane distances
	nearDistance = n;
	farDistance = f;
	
	// Precalculate side plane normal components
	float d = 1.0F / sqrt(l * l + 1.0F);
	leftRightX = l * d;
	leftRightZ = d;
	
	d = 1.0F / sqrt(l * l + a * a);
	topBottomY = l * d;
	topBottomZ = a * d;
}

bool Frustum::CylinderVisible(vector3 p1, vector3 p2, float radius) const
{
	// Calculate unit vector representing cylinder's axis
	vector3 dp = p2 - p1;
	dp.normalize();
	
	// Visit near plane first, N = (0,0,-1)
	float dot1 = -p1.z;
	float dot2 = -p2.z;
	
	// Calculate effective radius for near and far planes
	float effectiveRadius = radius * sqrt(1.0F - dp.z * dp.z);
	
	// Test endpoints against adjusted near plane
	float d = nearDistance - effectiveRadius;
	bool interior1 = (dot1 > d);
	bool interior2 = (dot2 > d);
	
	if (!interior1)
	{
		// If neither endpoint is interior, cylinder is not visible
		if (!interior2) return (false);
		
		// p1 was outside, so move it to the near plane
		float t = (d + p1.z) / dp.z;
		p1.x -= t * dp.x;
		p1.y -= t * dp.y;
		p1.z = -d;
	}
	else if (!interior2)
	{
		// p2 was outside, so move it to the near plane
		float t = (d + p1.z) / dp.z;
		p2.x = p1.x - t * dp.x;
		p2.y = p1.y - t * dp.y;
		p2.z = -d;
	}
	
	// Test endpoints against adjusted far plane
	d = farDistance + effectiveRadius;
	interior1 = (dot1 < d);
	interior2 = (dot2 < d);
	
	if (!interior1)
	{
		// If neither endpoint is interior, cylinder is not visible
		if (!interior2) return (false);
		
		// p1 was outside, so move it to the far plane
		float t = (d + p1.z) / (p2.z - p1.z);
		p1.x -= t * (p2.x - p1.x);
		p1.y -= t * (p2.y - p1.y);
		p1.z = -d;
	}
	else if (!interior2)
	{
		// p2 was outside, so move it to the far plane
		float t = (d + p1.z) / (p2.z - p1.z);
		p2.x = p1.x - t * (p2.x - p1.x);
		p2.y = p1.y - t * (p2.y - p1.y);
		p2.z = -d;
	}
	
	// Visit left side plane next
	// The normal components have been precalculated
	float nx = leftRightX;
	float nz = leftRightZ;
	
	// Compute p1 * N and p2 * N
	dot1 = nx * p1.x - nz * p1.z;
	dot2 = nx * p2.x - nz * p2.z;
	
	// Calculate effective radius for this plane
	float s = nx * dp.x - nz * dp.z;
	effectiveRadius = -radius * sqrt(1.0F - s * s);
	
	// Test endpoints against adjusted plane
	interior1 = (dot1 > effectiveRadius);
	interior2 = (dot2 > effectiveRadius);
	
	if (!interior1)
	{
		// If neither endpoint is interior, cylinder is not visible
		if (!interior2) return (false);
		
		// p1 was outside, so move it to the plane
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p1.x += t * (p2.x - p1.x);
		p1.y += t * (p2.y - p1.y);
		p1.z += t * (p2.z - p1.z);
	}
	else if (!interior2)
	{
		// p2 was outside, so move it to the plane
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p2.x = p1.x + t * (p2.x - p1.x);
		p2.y = p1.y + t * (p2.y - p1.y);
		p2.z = p1.z + t * (p2.z - p1.z);
	}
	
	// Visit right side plane next
	dot1 = -nx * p1.x - nz * p1.z;
	dot2 = -nx * p2.x - nz * p2.z;
	
	s = -nx * dp.x - nz * dp.z;
	effectiveRadius = -radius * sqrt(1.0F - s * s);
	
	interior1 = (dot1 > effectiveRadius);
	interior2 = (dot2 > effectiveRadius);
	
	if (!interior1)
	{
		if (!interior2) return (false);
		
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p1.x += t * (p2.x - p1.x);
		p1.y += t * (p2.y - p1.y);
		p1.z += t * (p2.z - p1.z);
	}
	else if (!interior2)
	{
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p2.x = p1.x + t * (p2.x - p1.x);
		p2.y = p1.y + t * (p2.y - p1.y);
		p2.z = p1.z + t * (p2.z - p1.z);
	}
	
	// Visit top side plane next
	// The normal components have been precalculated
	float ny = topBottomY;
	nz = topBottomZ;
	
	dot1 = -ny * p1.y - nz * p1.z;
	dot2 = -ny * p2.y - nz * p2.z;
	
	s = -ny * dp.y - nz * dp.z;
	effectiveRadius = -radius * sqrt(1.0F - s * s);
	
	interior1 = (dot1 > effectiveRadius);
	interior2 = (dot2 > effectiveRadius);
	
	if (!interior1)
	{
		if (!interior2) return (false);
		
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p1.x += t * (p2.x - p1.x);
		p1.y += t * (p2.y - p1.y);
		p1.z += t * (p2.z - p1.z);
	}
	else if (!interior2)
	{
		float t = (effectiveRadius - dot1) / (dot2 - dot1);
		p2.x = p1.x + t * (p2.x - p1.x);
		p2.y = p1.y + t * (p2.y - p1.y);
		p2.z = p1.z + t * (p2.z - p1.z);
	}
	
	// Finally, visit bottom side plane
	dot1 = ny * p1.y - nz * p1.z;
	dot2 = ny * p2.y - nz * p2.z;
	
	s = ny * dp.y - nz * dp.z;
	effectiveRadius = -radius * sqrt(1.0F - s * s);
	
	interior1 = (dot1 > effectiveRadius);
	interior2 = (dot2 > effectiveRadius);
	
	// At least one endpoint must be interior or cylinder is not visible
	return (interior1 | interior2);
}
