// Intersection routines

#pragma once

#include <vector>

#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"


namespace Mathematics
{
    /// %Intersection routines between various geometric objects in 3D

    namespace Intersection
    {
		/// Find intersection of Ray with Plane
        /// @returns true if ray intersects, false otherwise.
        /// note: intersection is one sided, parallel rays are ignored.

        inline bool findRayPlane(const Ray &ray, const Plane &plane, float &t)
        {
            const float vd = ray.direction.x * plane.normal.x + 
                             ray.direction.y * plane.normal.y + 
                             ray.direction.z * plane.normal.z; 

			if (vd>=0.0f)
				return false;			

            const float vo = - ( plane.normal.x * ray.origin.x + 
                                 plane.normal.y * ray.origin.y +
                                 plane.normal.z * ray.origin.z - 
                                 plane.d() );

            t = vo / vd;

            return true;
        }

		/// Find intersection between a Ray and a Sphere
        /// @returns true if ray intersects, false otherwise.
		/// note that this method always returns true if the ray originates inside the sphere.
		/// this means, unlike the other intersection methods it is not a one sided intersection test.

        inline bool findRaySphere(const Ray &ray, const Sphere &sphere, float &t)
		{
			const float dx = sphere.center.x - ray.origin.x;
			const float dy = sphere.center.y - ray.origin.y;
			const float dz = sphere.center.z - ray.origin.z;
		    
			const float d_squared = dx*dx + dy*dy + dz*dz;

			if (d_squared<sphere.radiusSquared)
			{
				// ray originates from inside sphere

				t = 0;
				return true;
			}

			const float closest = dx*ray.direction.x + dy*ray.direction.y + dz*ray.direction.z;

			if (closest<0) 
				return false;						// ray points faces away from sphere

			const float half_chord_squared = sphere.radiusSquared - d_squared + closest*closest;

			if (half_chord_squared<0) 
				return false;						// ray misses sphere

			t = closest - Float::sqrt(half_chord_squared);

			return true;
		}
        
		/// Test Ray for intersection with a Sphere
        /// @returns true if ray intersects, false otherwise.
		/// note that this method always returns true if the ray originates inside the sphere.
		/// that is, unlike the other intersection methods it is not a one sided intersection test.

        inline bool testRaySphere(const Ray &ray, const Sphere &sphere)
		{
			const float dx = sphere.center.x - ray.origin.x;
			const float dy = sphere.center.y - ray.origin.y;
			const float dz = sphere.center.z - ray.origin.z;
		    
			const float d_squared = dx*dx + dy*dy + dz*dz;

			if (d_squared<sphere.radiusSquared)
				return true;						// ray originates from inside sphere

			const float closest = dx*ray.direction.x + dy*ray.direction.y + dz*ray.direction.z;

			if (closest<0) 
				return false;						// ray points faces away from sphere

			const float half_chord_squared = sphere.radiusSquared - d_squared + closest*closest;

			if (half_chord_squared<0)
				return false;						// ray misses sphere

			return true;
		}

	}
}
