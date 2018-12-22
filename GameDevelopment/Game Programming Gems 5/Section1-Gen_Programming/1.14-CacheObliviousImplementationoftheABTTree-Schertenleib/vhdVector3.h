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

#ifndef VHDVECTOR3_H
#define VHDVECTOR3_H

//remove warning conversion from double or float to vhtReal  
#pragma warning(disable:4244)

#define vhtReal float

#include <ostream>

/** 
* 
* @brief Standard 3-dimensional column vector
*
* @author Sebastien Schertenleib
* Sebastien.Schertenleib@epfl.ch 
* @version 0.0.1
* @remark 
*/
class  vhdVector3
{

public:
	// coordinates
    vhtReal x, y, z;        

public:
     vhdVector3();
     vhdVector3( vhtReal fX, vhtReal fY, vhtReal fZ );
	 vhdVector3( const vhdVector3& rkVector );

	 //vhtReal operator [] ( unsigned int index ) const;
	 vhtReal& operator [] ( int index ) const;

	 /** 
	  * Assigns the value of the other vector.
	  * @param rkVector The other vector
	  */
	 vhdVector3& operator = ( const vhdVector3& rkVector );
	 bool operator == ( const vhdVector3& rkVector ) const;
	 bool operator != ( const vhdVector3& rkVector ) const;
	

	 // arithmetic operations
	 vhdVector3 operator + ( const vhdVector3& rkVector ) const;
	 vhdVector3 operator - ( const vhdVector3& rkVector ) const;
	 vhdVector3 operator * ( vhtReal fScalar ) const;
	 vhdVector3 operator * ( const vhdVector3& rhs) const;
	 vhdVector3 operator / ( vhtReal fScalar ) const;
	 vhdVector3 operator - () const;

	 //! Friend multiplication operator for scalar-vector multiplication
	 friend vhdVector3 operator * ( vhtReal fScalar, const vhdVector3& rkVector );

	 // arithmetic updates
	 vhdVector3& operator += ( const vhdVector3& rkVector );
	 vhdVector3& operator -= ( const vhdVector3& rkVector );
	 vhdVector3& operator *= ( vhtReal fScalar );
	 vhdVector3& operator /= ( vhtReal fScalar );
	 vhdVector3& operator *= ( const vhdVector3& rkVector );
	 vhdVector3& operator /= (const vhdVector3& rkVector);
	


    /** Function for writing to a stream.
    */
     friend std::ostream& operator <<
        ( std::ostream& o, const vhdVector3& v )
    {
        o << "vhdVector3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return o;
    }



};



#include "vhdVector3.inl"


#endif //VHDVECTOR3_H
