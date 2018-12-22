/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: class to compute and record sector-based line-of-sight info
*/


#ifndef _ASE_LOSSCAN_H_
#define _ASE_LOSSCAN_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_coordpair.h"


/*!
    ASE_LineOfSightScan uses 16 bits to store an approximation of true line of sight
    from surrounding positions to this position, for 2D terrain. 
    The terrain surrounding this position is partitioned in 8 sectors of 45 degrees 
    each.

    For each of the sectors, one of the following values is stored:
    - 00 == eNotObserved
    - 11 == eObservedFromFarDistance
    - 10 == eObservedFromMediumDistance
    - 01 == eObservedFromNearDistance
    where (obviously) eObservedFromFarDistance also implies eObservedFromMediumDistance,
    and eObservedFromMediumDistance implies eObservedFromNearDistance.

    Given a future threat position, a vector from that position to this position is
    computed, and a given the length and angle of the vector, the appropriate value
    is retrieved.
*/

class ASE_LineOfSightScan
{
  public:
    ASE_LineOfSightScan();  //!< inits to no cover

    enum ScanValue
      {
        eNotObserved                = 0,
        eObservedFromFarDistance    = 1,
        eObservedFromMediumDistance = 2,
        eObservedFromNearDistance   = 3
      };

    void         ComputeLOSApproximation
                       (int                  thisPositionX, 
                        int                  thisPositionY, 
                        const CoordPairList& thePositionsObservingThisPosition
                       );
    void         Clear();
    void         SetNoCoverAtAll();
    void         UpdateSector(unsigned int aSector, ScanValue aValue);

    ScanValue    GetObservationFromPosition(int thisPositionX,
                                            int thisPositionY,
                                            int theObserverX,
                                            int theObserverY
                                           ) const;

    static bool  IsANearDistanceSqrd(int theDistanceSquared);
    static bool  IsAMediumDistanceSqrd(int theDistanceSquared);
    static bool  IsAFarDistanceSqrd(int theDistanceSquared);
    static bool  IsOutOfReachSqrd(int theDistanceSquared);

    static unsigned int GetNumberOfSectors();


    static void  SetThreatReachDistance(int theDistance);

    static unsigned int GetSectorForLine(int deltaX, int deltaY);

  protected:
    typedef unsigned short int word;

    static word         GetValueForSector(word aValue, unsigned int aSector);
    static word         SetValueForSector(word aValue, word aSectorValue, unsigned int aSector);

    word m_LOSDirections;

  protected:
    static int                m_kOutOfReachDistance;
    static int                m_kFarDistance;
    static int                m_kMediumDistance;

    static const unsigned int m_kNumberOfSectors;
    static const float        m_kSectorWidthInDegrees;
};


#endif // _ASE_LOSSCAN_H_