/* Copyright (C) Gari Biasillo, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gari Biasillo, 2001"
 */

// To determine where the car intersects the interface, a 
// line-to-plane intersection test is calculated; the car's 
// previous and current position specify the line and the starting 
// interface of the newly occupied sector defines the plane. Because 
// this test is only performed when the car enters a new sector, we 
// can guarantee that the line straddles the plane.


// Plane Equation = Ax + By + Cz + D = 0, where
// A = normal.x, B = normal.y, C = normal.z, D = dist
// x,y,z = vector3 to test against plane, therefore
// Dist to plane = DotProduct(vector3, normal) + dist
class CPlane
{
    public:
        vector3   normal;
        float     dist;
} ;

vector3 LineToPlane(const vector3& start, const vector3& end, const
                    CPlane& plane)
{
    float s, e, t;
    s = DotProduct(plane.normal, start) + plane.dist;
    e = DotProduct(plane.normal, end) + plane.dist;
    t = s / (s - e);
    vector3 delta = end - start;
    return (start + delta * t);
}

float TimeOfPointOnLine(const vector3& point, const vector3& start, 
                        vector3& end)
{
	vector3 delta = end - start;
	float length = delta.length();
	delta = point - start;
	float t = delta.length();
	return (t / length);
}
