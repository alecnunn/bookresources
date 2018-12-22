// Heightfield class

#pragma once

#include <vector>

#include "Mathematics/Float.h"
#include "Mathematics/Vector.h"

using namespace Mathematics;


/// A heightfield with bilinear interpolation support for height and normal queries at arbitrary (x,y).
/// Internally the heightfield stores a 2D array of floating point height samples in world space.
/// The key functionality that this class implements are queries about height and normal at a point
/// (x,y), either in integer coordinates (discrete terrain samples), or in world coordinates (bilinearly interpolated).
/// Please note that the heightfield makes the strict assumption that it is in the x,y plane with +z being up,
/// and that the width,height of the heightfield correspond to vectors along the x,y axes.

class Heightfield
{
public:

    /// Initialize heightfield from raw sample data.
    /// This method constructs the 2D floating point samples array from normalized floating point
    /// heightfield samples, a vector for the center of the terrain in (x,y) world space, 
    /// and scale factors along the xy plane and the vertical z axis.
	/// NOTE: the normalized data has 0.5 subtracted from its height, then it is scaled by scale_z
	/// to give the actual heights in world coordinates stored internally.
	/// NOTE: if the width and height are not power of two plus one, the nearest higher
	/// value which satisfies this constraint is used for the actual width and height
	/// of the heightfield sample array.

    void initialize(const float data[], int width, int height, const Vector &center, float scale_xy, float scale_z)
    {
		// initialize primary data

		assert(width>1);
		assert(height>1);
		assert(width==height);

		m_width = nearestPowerOfTwoPlusOneAbove(width);
        m_height = nearestPowerOfTwoPlusOneAbove(height);
		m_scale = scale_xy;

		// initialize secondary data

		m_scale_inverse = 1.0f / m_scale;
		m_scale_half = 0.5f * m_scale;
		m_scale_squared = m_scale * m_scale;

        const float half_dx = m_scale * width * 0.5f;
        const float half_dy = m_scale * height * 0.5f;
		const float half_dz = scale_z * 0.5f;

        m_origin = Vector( center.x - half_dx, center.x - half_dy, center.z - half_dz);

		m_minimum = m_origin;
		vectorAtIndex(width-1, height-1, m_maximum);

		// load heightfield samples

		const int size = m_width * m_height;
		m_samples.resize(size);

		load(data, width, height, &m_samples[0], m_width, m_height, true, 0.5f, scale_z);

		// calculate minimum and maximum z extents

		m_minimum.z = FLT_MAX;
		m_maximum.z = -FLT_MAX;

		const int samples = m_width*m_height;
		for (int i=0; i<samples; i++)
		{
			if (m_minimum.z>m_samples[i])
				m_minimum.z = m_samples[i];

			if (m_maximum.z<m_samples[i])
				m_maximum.z = m_samples[i];
		}
    }

    /// Get the height in world coordinates at integer sample coordinates (x,y).

	float heightAtIndex(int x, int y) const
	{
        assert(x>=0);
        assert(y>=0);
        assert(x<m_width);
        assert(y<m_height);

		return m_samples[x+y*m_height];
	}

    float heightAtOffset(unsigned int offset) const
    {
        assert(offset>=0);
        assert(offset<m_samples.size());
        return m_samples[offset];
    }

    int offsetAtIndex(int x, int y) const
    {
        return x+y*m_height;
    }

    /// Get the height in world coordinates at integer sample coordinates (x,y) clamped to bounds.
    /// Note that this method is temporary and will be removed once we implement a guard band border
    /// in the heightfield sample array.

    float heightAtIndex_SAFE(int x, int y) const
	{
		if (x<0) x = 0;                 
		else if (x>=m_width) x = m_width - 1;
    	
		if (y<0) y = 0;
		else if (y>=m_height) y = m_height - 1;

        assert(x>=0);
        assert(y>=0);
        assert(x<m_width);
        assert(y<m_height);

		return m_samples[x+y*m_height];
	}

    /// Get the bilinearly interpolated height at world coordinates (x,y).
    /// Please be careful that you pass (x,y) coordinates that are inside the heightfield, this method does not clamp or clip!

	float height(float x, float y) const
	{
		// the height returned here is in world units

		// (x1,y1) (x2,y1)
		// (x1,y2) (x2,y2)

		int x1 = Float::integer((x - m_origin.x) * m_scale_inverse);
		int y1 = Float::integer((y - m_origin.y) * m_scale_inverse);
		int x2 = x1 + 1;
		int y2 = y1 + 1;

		// a b
		// c d

		const float a = heightAtIndex_SAFE(x1,y1);
		const float b = heightAtIndex_SAFE(x2,y1);
		const float c = heightAtIndex_SAFE(x1,y2);
		const float d = heightAtIndex_SAFE(x2,y2);

		// calculate normalized distance from point sample at center of grid cell

		float cx,cy;
		vectorAtIndex(x1,y1,cx,cy);

		const float dx = (x - cx) * m_scale_inverse;
		const float dy = (y - cy) * m_scale_inverse;

		const float idx = 1 - dx;
		const float idy = 1 - dy;

		// calculate bilinear sample weightings

		const float weight_a = idx * idy;
		const float weight_b = dx * idy;
		const float weight_c = idx * dy;
		const float weight_d = dx * dy;

		// calculate bilinear height sample

		return weight_a*a + weight_b*b + weight_c*c + weight_d*d;
	}

    /// Get the bilinear interpolated normal vector at world coordinates (x,y)
    /// Note that the normal returned here will not be unit length, normalize it yourself if you need a unit vector!

	void normal(float x, float y, Vector &normal) const
	{
		// (x1,y1) (x2,y1)
		// (x1,y2) (x2,y2)

		int x1 = Float::integer((x - m_origin.x) * m_scale_inverse);
		int y1 = Float::integer((y - m_origin.y) * m_scale_inverse);
		int x2 = x1 + 1;
		int y2 = y1 + 1;

		// calculate normalized distance from point sample at center of grid cell

		float cx,cy;
		vectorAtIndex(x1,y1,cx,cy);

		const float dx = (x - cx) * m_scale_inverse;
		const float dy = (y - cy) * m_scale_inverse;

		const float idx = 1 - dx;
		const float idy = 1 - dy;

		// calculate bilinear sample weightings

		// a b
		// d c

		const float weight_a = idx * idy;
		const float weight_b = dx * idy;
		const float weight_c = dx * dy;
		const float weight_d = idx * dy;

		// sample grid heights

		//   i j
		// k l m n
		// o p q r
		//   s t

		const float i = heightAtIndex_SAFE(x1,   y1-1);
		const float j = heightAtIndex_SAFE(x2,   y1-1);
		const float k = heightAtIndex_SAFE(x1-1, y1);
		const float l = heightAtIndex_SAFE(x1,   y1);
		const float m = heightAtIndex_SAFE(x2,   y1);
		const float n = heightAtIndex_SAFE(x2+1, y1);
		const float o = heightAtIndex_SAFE(x1-1, y2);
		const float p = heightAtIndex_SAFE(x1,   y2);
		const float q = heightAtIndex_SAFE(x2,   y2);
		const float r = heightAtIndex_SAFE(x2+1, y2);
		const float s = heightAtIndex_SAFE(x1,   y2+1);
		const float t = heightAtIndex_SAFE(x2,   y2+1);

		// calculate interpolated normal

		normal.x = m_scale_half * ((k - m) * weight_a + (l - n) * weight_b + (p - r) * weight_c + (o - q) * weight_d);
		normal.y = m_scale_half * ((i - p) * weight_a + (j - q) * weight_b + (m - t) * weight_c + (l - s) * weight_d);
		normal.z = m_scale_squared * (weight_a + weight_b + weight_c + weight_d);
	}

    /// Get the normal vector integer sample coordinates (x,y).
    /// Note that the normal returned here will not be unit length, normalize it yourself if you need a unit vector!

	void normalAtIndex(int x, int y, Vector &normal) const
	{
		// a b
		// c 

		const float a = heightAtIndex_SAFE(x,y);          // temporary: pending guard band
		const float b = heightAtIndex_SAFE(x+1,y);
		const float c = heightAtIndex_SAFE(x,y+1);

		const Vector ab(m_scale, 0, b-a);
		const Vector ac(0, m_scale, c-a);
		
		normal = ab.cross(ac);
	}

    /// Get integer terrain sample index coordinates at world coordinates vector (point.x, point.y)

	void indexAtVector(const Vector &point, int &ix, int &iy) const
	{
		const float dx = (point.x - m_origin.x) * m_scale_inverse;
		const float dy = (point.y - m_origin.y) * m_scale_inverse;
		ix = Float::integer(dx);
		iy = Float::integer(dy);
	}

    /// Get integer terrain sample index coordinates at world coordinates vector (x,y)

	void indexAtVector(float x, float y, int &ix, int &iy) const
	{
		const float dx = (x - m_origin.x) * m_scale_inverse;
		const float dy = (y - m_origin.y) * m_scale_inverse;
		ix = Float::integer(dx);
		iy = Float::integer(dy);
	}

    /// Get world point vector (x,y) at integer terrain sample index (ix,iy)

	void vectorAtIndex(int ix, int iy, float &x, float &y) const
	{
		x = m_origin.x + ix * m_scale;
		y = m_origin.y + iy * m_scale;
	}

    /// Get world point vector (point.x,point.y) at integer terrain sample index (ix,iy).
    /// Note that this method sets point.z=0, it does NOT return the terrain height in the point vector!
    /// If you need this functionality please use the heightAtIndex method.

	void vectorAtIndex(int ix, int iy, Vector &point) const
	{
		point.x = m_origin.x + ix * m_scale;
		point.y = m_origin.y + iy * m_scale;
		point.z = 0;
	}

    /// Get world point vector (point.x,point.y) at integer terrain sample index (ix,iy).
    /// Note that this method sets the returned vector's z coordinate to 0.0f. 
    /// It does NOT return the terrain height in the vector! If you need to query
    /// terrain height at index please use the heightAtIndex method.

	Vector vectorAtIndex(int ix, int iy) const
	{
        return Vector(m_origin.x + ix * m_scale, m_origin.y + iy * m_scale, 0);
	}

    /// Get the origin of the heightfield

    Vector origin() const
    {
        return m_origin;
    }

	/// Get the minimum coordinate of the heightfield axis aligned bounding volume.
	/// note: minimum z coordinate not yet implemented.

	Vector minimum() const
	{
		return m_minimum;
	}

	/// Get the maximum coordinate of the heightfield axis aligned bounding volume.
	/// note: minimum z coordinate not yet implemented.

	Vector maximum() const
	{
		return m_maximum;
	}

    /// Get the width of the heightfield in samples.

	int width() const
	{
		return m_width;
	}

    /// Get the height of the heightfield in samples.

	int height() const
	{
		return m_height;
	}

    /// Get the scale of the heightfield

    float scale() const
    {
        return m_scale;
    }

	/// Check if vector is inside heightfield bounds
	/// note: checks if inside full 3d bounding box (m_minimum, m_maximum)

	bool inside(const Vector &vector) const
	{
		return ( vector.x>=m_minimum.x && vector.x<=m_maximum.x &&
				    vector.y>=m_minimum.y && vector.y<=m_maximum.y && 
				    vector.z>=m_minimum.z && vector.z<=m_maximum.z );
	}		

private:

	/// Calculate nearest power of two plus one value above the given integer (3,5,9,17,33,65,129,257,513,1025,2049 max)

	unsigned int nearestPowerOfTwoPlusOneAbove(unsigned int number)
	{
		int i = 0;
		unsigned int pow = 2;
		while (i<32)
		{
			if (number<=pow+1)
				return pow+1;
			pow<<=1;
			i++;
		}
		return 0;
	}

	/// Load source heightfield data into local heightfield data.
	/// This method automatically handles clipping to source data, so we can expand from non 2^n-1 size images.

	void load( const float source_data[], int source_width, int source_height, 
			    float destination_data[], int destination_width, int destination_height, 
				bool normalized = false, float offset = 0, float scale = 1 )
	{
		for (int y=0; y<destination_height; y++)
		{
			for (int x=0; x<destination_width; x++)
			{
				int sx = x;
				if (sx>=source_width)
					sx = source_width - 1;

				int sy = y;
				if (sy>=source_height)
					sy = source_height - 1;

				const int source_offset = sx + sy*source_width;
				const int destination_offset = x + y*destination_width;

				#ifdef _DEBUG
				if (normalized)
				{
					assert(source_data[source_offset]>=0);
					assert(source_data[source_offset]<=1);
				}
				#endif

				destination_data[destination_offset] = (source_data[source_offset] - offset) * scale;
			}
		}
	}

	// primary data

	Vector m_origin;			    ///< origin point the xy world coordinates of the height sample (0,0)
	Vector m_minimum;			    ///< minimum point of axis aligned bounding box
	Vector m_maximum;			    ///< maximum point of axis aligned bounding box
	float m_scale;			        ///< cell scale in xy
	int m_width;				    ///< width in samples
	int m_height;				    ///< height in samples
    std::vector<float> m_samples;	///< samples array (heights stored in world coordinates)

	// secondary data

	float m_scale_inverse;	        ///< inverse of xy cell scale
	float m_scale_half;				///< half of xy cell scale
	float m_scale_squared;			///< xy cell scale squared
};
