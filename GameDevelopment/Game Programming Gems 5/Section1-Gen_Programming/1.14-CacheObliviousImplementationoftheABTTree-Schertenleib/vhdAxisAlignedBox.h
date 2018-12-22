////////////////////////////////////////////////////////////////////////////
//	Copyright : Sebastien Schertenleib
//	          : VRLab-EPFL
//
//	Email :Sebastien.Schertenleib@epfl.ch
//
//	This source code is a part of the Game Programming Gems 5 Source Code
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
////////////////////////////////////////////////////////////////////////////


#ifndef VHDAXISALIGNEDBOX_H
#define VHDAXISALIGNEDBOX_H


#include "vhdVector3.h"
	

	
/** @brief A 3D box aligned with the x/y/z axes.
 *
 * This class represents a simple box which is aligned with the
 * axes. Internally it only stores 2 points as the extremeties of
 * the box, one which is the minima of all 3 axes, and the other
 * which is the maxima of all 3 axes. This class is typically used
 * for an axis-aligned bounding box (AABB) for collision and
 * visibility determination.
 *   
 * @author Sebastien Schertenleib
 * Sebastien.Schertenleib@epfl.ch 
 * @version 0.0.1
 */
class vhdAxisAlignedBox
{
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// non-STATIC SECTION:
	// non-static field and method declarations specific to
	// the particular class being defined
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// protected fields
	//
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
protected:
    vhdVector3 _vecMinimum;
    vhdVector3 _vecMaximum;
    bool _bNull;

    vhdVector3 _vecCorners[8];



	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Constructor & Destructor
	//
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:	
	vhdAxisAlignedBox()
    	{
			// Default to null box
			setExtents(-0.5, -0.5, -0.5,0.5, 0.5, 0.5 );
		    _bNull = true;
    	}
 	vhdAxisAlignedBox(float mx, float my, float mz,float Mx, float My, float Mz )
    	{
	    setExtents( mx, my, mz, Mx, My, Mz );
    	}

	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// protected methods
	//
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

protected:


    /** Internal method for updating corner points.
    */
    void _updateCorners(void)
    {
	    // The order of these items is, using right-handed co-ordinates:
	    // Minimum Z face, starting with Min(all), then anticlockwise
	    //   around face (looking onto the face)
	    // Maximum Z face, starting with Max(all), then anticlockwise
	    //   around face (looking onto the face)
	    _vecCorners[0] = _vecMinimum;
	    _vecCorners[1].x = _vecMinimum.x; _vecCorners[1].y = _vecMaximum.y; _vecCorners[1].z = _vecMinimum.z;
	    _vecCorners[2].x = _vecMaximum.x; _vecCorners[2].y = _vecMaximum.y; _vecCorners[2].z = _vecMinimum.z;
	    _vecCorners[3].x = _vecMaximum.x; _vecCorners[3].y = _vecMinimum.y; _vecCorners[3].z = _vecMinimum.z;            

	    _vecCorners[4] = _vecMaximum;
	    _vecCorners[5].x = _vecMinimum.x; _vecCorners[5].y = _vecMaximum.y; _vecCorners[5].z = _vecMaximum.z;
	    _vecCorners[6].x = _vecMinimum.x; _vecCorners[6].y = _vecMinimum.y; _vecCorners[6].z = _vecMaximum.z;
	    _vecCorners[7].x = _vecMaximum.x; _vecCorners[7].y = _vecMinimum.y; _vecCorners[7].z = _vecMaximum.z;            
    }        
    
    
	    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    //
	    // public methods
	    //
	    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

public:
    

    /** 
     * Gets the minimum corner of the box.
     *
     * @return the minimum corner of the box.
    */
    inline vhdVector3 getMinimum(void) const
    { 
	    return _vecMinimum; 
    }

    /** 
     * Gets the maximum corner of the box.
     *
     *
     * @return the maximum corner of the box.
     */
    inline vhdVector3 getMaximum(void) const
    { 
	    return _vecMaximum;
    }

    
    
     /** 
     * Sets both minimum and maximum extents at once.
     *
     * @params mx the minimum x corner of the box.
     * @params my the minimum y corner of the box.
     * @params mz the minimum z corner of the box.
     * @params Mx the maximum x corner of the box.
     * @params My the maximum y corner of the box.
     * @params Mz the maximum z corner of the box.
     */
  inline void setExtents(float mx, float my, float mz,float Mx, float My, float Mz )
    {
	    _bNull = false;

	    _vecMinimum.x = mx;
	    _vecMinimum.y = my;
	    _vecMinimum.z = mz;

	    _vecMaximum.x = Mx;
	    _vecMaximum.y = My;
	    _vecMaximum.z = Mz;

	    _updateCorners();
    }

    

    friend std::ostream& operator<<( std::ostream& o, vhdAxisAlignedBox aab )
    {
		o << "vhdAxisAlignedBox(min=" << aab._vecMinimum << ", max=" << aab._vecMaximum;
		o << ", corners=";
		for (int i = 0; i < 7; ++i)
		{
			o << aab._vecCorners[i] << ", ";
		}
		o << aab._vecCorners[7] << ")";
		return o;
    }

};



#endif
