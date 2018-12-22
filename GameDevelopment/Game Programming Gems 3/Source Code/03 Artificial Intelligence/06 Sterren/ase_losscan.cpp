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


#include "stdafx.h"
#include "ase_losscan.h"

#include <cassert>
#include <cmath> // atan

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int                ASE_LineOfSightScan::m_kOutOfReachDistance    = -1;
int                ASE_LineOfSightScan::m_kFarDistance           = -1;
int                ASE_LineOfSightScan::m_kMediumDistance        = -1;

const unsigned int ASE_LineOfSightScan::m_kNumberOfSectors       =  8;
const float        ASE_LineOfSightScan::m_kSectorWidthInDegrees  = 360.0f / m_kNumberOfSectors;


ASE_LineOfSightScan::ASE_LineOfSightScan()
  : m_LOSDirections(0)
  { 
  }


unsigned int ASE_LineOfSightScan::GetSectorForLine(int deltaX, int deltaY)
  {
    assert( m_kSectorWidthInDegrees == 45.0f );
    // hardwired for 45 degrees sectors
    // sectors are numbered as follows: 0 degrees -> 0, 45 -> 1, 90 -> 2 ...

    static const float  kSmallPositiveNumber  = 1e-6F;
    static const double kPI                   = 4 * atan(1.0);
    static const float  kSector0Sector1Border = static_cast<float>(tan(22.5 / 180 * kPI));
    static const float  kSector1Sector2Border = static_cast<float>(tan(67.5 / 180 * kPI));

    float x;
    float y;
    x = static_cast<float>(deltaX);
    y = static_cast<float>(deltaY);

    unsigned int result;
    bool         bFlippedX;
    bool         bFlippedY;
    // assignment and comparison in one!!
    if ( bFlippedY = (y < 0) )
      y *= -1;
    if ( bFlippedX = (x < 0) )
      x *= -1;
   
    if ( x < kSmallPositiveNumber )
      {
        result = 2;
      }
    else
      {
        float fraction;
        fraction = y / x;
        if ( fraction < kSector0Sector1Border )
          result = 0;
        else
        if ( fraction < kSector1Sector2Border )
          result = 1;
        else
          result = 2;
      }

    if ( bFlippedX )
      {
        result = 4 - result;
      }

    if ( bFlippedY )
      {
        // flipped sign, so flip result
        result = (m_kNumberOfSectors - result) % m_kNumberOfSectors;
      }
    return result;
  }


void  ASE_LineOfSightScan::SetThreatReachDistance(int theDistance)
  {
    /*! partition the range as follows:
       out of reach       = from 1+ to beyond
       far                = from 2/3 to 1
       medium             = from 1/3 to 2/3
       near               = from 0   to 1/3
    */ 
    m_kOutOfReachDistance = theDistance + 1;
    m_kFarDistance        = m_kOutOfReachDistance * 2 / 3;
    m_kMediumDistance     = m_kOutOfReachDistance * 1 / 3;
  }


bool ASE_LineOfSightScan::IsANearDistanceSqrd(int theDistanceSquared)
  {
    assert( m_kMediumDistance > 0 );
    return ( theDistanceSquared < ( m_kMediumDistance * m_kMediumDistance) ); 
  }


bool ASE_LineOfSightScan::IsAMediumDistanceSqrd(int theDistanceSquared)
  {
    assert( m_kMediumDistance > 0 );
    assert( m_kFarDistance    > 0 );
    return (   ( theDistanceSquared <  ( m_kFarDistance    * m_kFarDistance)    )
            && ( theDistanceSquared >= ( m_kMediumDistance * m_kMediumDistance) )
           ); 
  }


bool ASE_LineOfSightScan::IsAFarDistanceSqrd(int theDistanceSquared)
  {
    assert( m_kFarDistance        > 0 );
    assert( m_kOutOfReachDistance > 0 );
    return (   ( theDistanceSquared <  ( m_kOutOfReachDistance * m_kOutOfReachDistance) )
            && ( theDistanceSquared >= ( m_kFarDistance        * m_kFarDistance)        )
           ); 
  }


bool ASE_LineOfSightScan::IsOutOfReachSqrd(int theDistanceSquared)
  {
    assert( m_kOutOfReachDistance > 0 );
    return ( theDistanceSquared >= ( m_kOutOfReachDistance * m_kOutOfReachDistance) );
  }


ASE_LineOfSightScan::word ASE_LineOfSightScan::GetValueForSector(ASE_LineOfSightScan::word aValue, unsigned int aSector)
  {
    assert( aSector < m_kNumberOfSectors );
    const word kMask = 3;

    word result;
    result = ( aValue >> (2 * aSector) ) & kMask;
    return result;
  }


ASE_LineOfSightScan::word ASE_LineOfSightScan::SetValueForSector(ASE_LineOfSightScan::word aValue, ASE_LineOfSightScan::word aSectorValue, unsigned int aSector)
  {
    assert( aSector       < m_kNumberOfSectors );
    assert( aSectorValue  < 4 );

    const word kMask    = (3 << ( 2 * aSector));
    word  shiftedSValue = (aSectorValue << ( 2 * aSector) );
    assert( shiftedSValue == ( kMask & shiftedSValue ) );

    word result;
    result = (aValue - ( aValue & kMask) + shiftedSValue );
    return result;
  }


ASE_LineOfSightScan::ScanValue ASE_LineOfSightScan::GetObservationFromPosition(int thisPositionX,
                                                                               int thisPositionY,
                                                                               int theObserverX,
                                                                               int theObserverY
                                                                              ) const
  {
    int deltaX;
    int deltaY;
    deltaX = theObserverX - thisPositionX;
    deltaY = theObserverY - thisPositionY;

    int distanceSquared;
    distanceSquared = deltaX * deltaX + deltaY * deltaY;

    ScanValue result;   
    if ( IsOutOfReachSqrd(distanceSquared) )
      result = eNotObserved;
    else
      {
        unsigned int sector;
        sector = GetSectorForLine(deltaX, deltaY);
 
        ScanValue los;
        los = static_cast<ScanValue>(GetValueForSector(m_LOSDirections, sector));

        if ( los == eNotObserved )
          result = eNotObserved;
        else
        if (   ( IsAFarDistanceSqrd(distanceSquared) )
            && ( los == eObservedFromFarDistance )
           )
          result = eObservedFromFarDistance;
        else
        if (   ( IsAMediumDistanceSqrd(distanceSquared) )
            && ( los <= eObservedFromMediumDistance )
           )
          result = eObservedFromMediumDistance;
        else
        if (   ( IsANearDistanceSqrd(distanceSquared) )
         // && ( los <= eObservedFromNearDistance )
           )
          result = eObservedFromNearDistance;
        else
          result = eNotObserved;
      }

    return result;
  }


void ASE_LineOfSightScan::Clear()
  {
    m_LOSDirections = 0;
  }


void ASE_LineOfSightScan::SetNoCoverAtAll()
  {
    m_LOSDirections = 0;
    for ( unsigned int sector = 0; sector < m_kNumberOfSectors; ++sector )
      {
        UpdateSector(sector, eObservedFromFarDistance); 
      }
  }


void ASE_LineOfSightScan::UpdateSector(unsigned int aSector, ScanValue aValue)
  {
    m_LOSDirections = SetValueForSector(m_LOSDirections, aValue, aSector);
  }


void ASE_LineOfSightScan::ComputeLOSApproximation
                            (int                  thisPositionX, 
                             int                  thisPositionY, 
                             const CoordPairList& thePositionsObservingThisPosition
                            )
  {
    // init all flags to false (not observed)
    m_LOSDirections = 0;

    // iterate over all observing positions
    CoordPairList::const_iterator position;
    for ( position =  thePositionsObservingThisPosition.begin();
          position != thePositionsObservingThisPosition.end();
          ++position
        )
      {
        int deltaX;
        int deltaY;
        deltaX = position->x - thisPositionX;
        deltaY = position->y - thisPositionY;

        int distanceSquared;
        distanceSquared = deltaX * deltaX + deltaY * deltaY;

        if ( IsOutOfReachSqrd(distanceSquared) )
          continue;

        unsigned int sector;
        sector = GetSectorForLine(deltaX, deltaY);

        word      value;
        ScanValue newvalue;
        value = GetValueForSector(m_LOSDirections, sector);

        if ( IsAFarDistanceSqrd(distanceSquared) )
          newvalue = eObservedFromFarDistance;
        else
        if ( IsAMediumDistanceSqrd(distanceSquared) )
          newvalue = eObservedFromMediumDistance;
        else
          newvalue = eObservedFromNearDistance;

        if (   ( value == 0 )
            || (   ( newvalue > 0 ) 
                && ( newvalue < value )
               )
           )
          UpdateSector(sector, newvalue);
      }
  }


unsigned int ASE_LineOfSightScan::GetNumberOfSectors()
  {
    return m_kNumberOfSectors;
  }

