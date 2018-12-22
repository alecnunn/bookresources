/* Copyright (C) Eric Lengyel, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Lengyel, 2001"
 */
#include "mtxlib.h"


const long maxDecalVertices = 256;
const float decalEpsilon = 0.25F;


struct ColorRGBA
{
	float		red;
	float		green;
	float		blue;
	float		alpha;
	
	ColorRGBA() {}
	
	ColorRGBA(float r, float g, float b, float a)
	{
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
};

struct Triangle
{
	unsigned short	index[3];
};


inline float DotProduct(const vector4& p, const vector3& q)
{
	return (p.x * q.x + p.y * q.y + p.z * q.z + p.w);
}


class Decal
{
	private:
		
		vector3		decalCenter;
		vector3		decalNormal;
		
		vector4		leftPlane;
		vector4		rightPlane;
		vector4		bottomPlane;
		vector4		topPlane;
		vector4		frontPlane;
		vector4		backPlane;
		
		long		decalVertexCount;
		long		decalTriangleCount;
		
		vector3		vertexArray[maxDecalVertices];
		vector2		texcoordArray[maxDecalVertices];
		ColorRGBA	colorArray[maxDecalVertices];
		Triangle	triangleArray[maxDecalVertices];
		
		bool AddPolygon(long vertexCount, const vector3 *vertex, const vector3 *normal);
		void ClipMesh(long triangleCount, const Triangle *triangle, const vector3 *vertex, const vector3 *normal);
		long ClipPolygon(long vertexCount, const vector3 *vertex, const vector3 *normal, vector3 *newVertex, vector3 *newNormal) const;
		static long ClipPolygonAgainstPlane(const vector4& plane, long vertexCount, const vector3 *vertex, const vector3 *normal, vector3 *newVertex, vector3 *newNormal);
	
	public:
		
		Decal(const vector3& center, const vector3& normal, const vector3& tangent, float width, float height, float depth);
};


Decal::Decal(const vector3& center, const vector3& normal, const vector3& tangent, float width, float height, float depth)
{
	decalCenter = center;
	decalNormal = normal;
	
	vector3 binormal = CrossProduct(normal, tangent);
	
	// Calculate boundary planes
	float d = DotProduct(center, tangent);
	leftPlane = vector4(tangent.x, tangent.y, tangent.z, width * 0.5F - d);
	rightPlane = vector4(-tangent.x, -tangent.y, -tangent.z, width * 0.5F + d);
	
	d = DotProduct(center, binormal);
	bottomPlane = vector4(binormal.x, binormal.y, binormal.z, height * 0.5F - d);
	topPlane = vector4(-binormal.x, -binormal.y, -binormal.z, height * 0.5F + d);
	
	d = DotProduct(center, normal);
	frontPlane = vector4(-normal.x, -normal.y, -normal.z, depth + d);
	backPlane = vector4(normal.x, normal.y, normal.z, depth - d);
	
	// Begin with empty mesh
	decalVertexCount = 0;
	decalTriangleCount = 0;
	
	// Add this point, determine which surfaces may be affected by this decal
	// and call ClipMesh() for each one.
	
	// Assign texture mapping coordinates
	float one_over_w = 1.0F / width;
	float one_over_h = 1.0F / height;
	for (long a = 0; a < decalVertexCount; a++)
	{
		vector3 v = vertexArray[a] - center;
		float s = DotProduct(v, tangent) * one_over_w + 0.5F;
		float t = DotProduct(v, binormal) * one_over_h + 0.5F;
		texcoordArray[a] = vector2(s, t);
	}
}

bool Decal::AddPolygon(long vertexCount, const vector3 *vertex, const vector3 *normal)
{
	long count = decalVertexCount;
	if (count + vertexCount >= maxDecalVertices) return (false);
	
	// Add polygon as a triangle fan
	Triangle *triangle = triangleArray + decalTriangleCount;
	decalTriangleCount += vertexCount - 2;
	for (long a = 2; a < vertexCount; a++)
	{
		triangle->index[0] = (unsigned short) count;
		triangle->index[1] = (unsigned short) (count + a - 1);
		triangle->index[2] = (unsigned short) (count + a);
		triangle++;
	}
	
	// Assign vertex colors
	float f = 1.0F / (1.0F - decalEpsilon);
	for (long b = 0; b < vertexCount; b++)
	{
		vertexArray[count] = vertex[b];
		const vector3& n = normal[b];
		float alpha = (DotProduct(decalNormal, n) / n.length() - decalEpsilon) * f;
		colorArray[count] = ColorRGBA(1.0F, 1.0F, 1.0F, (alpha > 0.0F) ? alpha : 0.0F);
		count++;
	}
	
	decalVertexCount = count;
	return (true);
}

void Decal::ClipMesh(long triangleCount, const Triangle *triangle, const vector3 *vertex, const vector3 *normal)
{
	vector3		newVertex[9];
	vector3		newNormal[9];
	
	// Clip one triangle at a time
	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];
		
		const vector3& v1 = vertex[i1];
		const vector3& v2 = vertex[i2];
		const vector3& v3 = vertex[i3];
		
		vector3 cross = CrossProduct(v2 - v1, v3 - v1);
		if (DotProduct(decalNormal, cross) > decalEpsilon * cross.length())
		{
			newVertex[0] = v1;
			newVertex[1] = v2;
			newVertex[2] = v3;
			
			newNormal[0] = normal[i1];
			newNormal[1] = normal[i2];
			newNormal[2] = normal[i3];
			
			long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
			if ((count != 0) && (!AddPolygon(count, newVertex, newNormal))) break;
		}
		
		triangle++;
	}
}

long Decal::ClipPolygon(long vertexCount, const vector3 *vertex, const vector3 *normal, vector3 *newVertex, vector3 *newNormal) const
{
	vector3		tempVertex[9];
	vector3		tempNormal[9];
	
	// Clip against all six planes
	long count = ClipPolygonAgainstPlane(leftPlane, vertexCount, vertex, normal, tempVertex, tempNormal);
	if (count != 0)
	{
		count = ClipPolygonAgainstPlane(rightPlane, count, tempVertex, tempNormal, newVertex, newNormal);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(bottomPlane, count, newVertex, newNormal, tempVertex, tempNormal);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(topPlane, count, tempVertex, tempNormal, newVertex, newNormal);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(backPlane, count, newVertex, newNormal, tempVertex, tempNormal);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(frontPlane, count, tempVertex, tempNormal, newVertex, newNormal);
					}
				}
			}
		}
	}
	
	return (count);
}

long Decal::ClipPolygonAgainstPlane(const vector4& plane, long vertexCount, const vector3 *vertex, const vector3 *normal, vector3 *newVertex, vector3 *newNormal)
{
	bool	negative[10];
	
	// Classify vertices
	long negativeCount = 0;
	for (long a = 0; a < vertexCount; a++)
	{
		bool neg = (DotProduct(plane, vertex[a]) < 0.0F);
		negative[a] = neg;
		negativeCount += neg;
	}
	
	// Discard this polygon if it's completely culled
	if (negativeCount == vertexCount) return (0);
	
	long count = 0;
	for (long b = 0; b < vertexCount; b++)
	{
		// c is the index of the previous vertex
		long c = (b != 0) ? b - 1 : vertexCount - 1;
		
		if (negative[b])
		{
			if (!negative[c])
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const vector3& v1 = vertex[c];
				const vector3& v2 = vertex[b];
				float t = DotProduct(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;
				
				const vector3& n1 = normal[c];
				const vector3& n2 = normal[b];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}
		}
		else
		{
			if (negative[c])
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const vector3& v1 = vertex[b];
				const vector3& v2 = vertex[c];
				float t = DotProduct(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;
				
				const vector3& n1 = normal[b];
				const vector3& n2 = normal[c];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}
			
			// Include current vertex
			newVertex[count] = vertex[b];
			newNormal[count] = normal[b];
			count++;
		}
	}
	
	// Return number of vertices in clipped polygon
	return (count);
}
