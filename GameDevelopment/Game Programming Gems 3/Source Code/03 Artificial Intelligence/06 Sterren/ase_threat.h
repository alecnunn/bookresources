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
 *     purpose: class to represent threat and terrain covered by threat
*/


#ifndef _ASE_THREAT_H_
#define _ASE_THREAT_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include <vector>


class ASE_Threat
{
  public:
    ASE_Threat(unsigned int aLocationIdx);

    typedef std::vector<unsigned int> PositionsUnderFire;

    void                      SetPositionsUnderFire(const PositionsUnderFire& thePositionsUnderFire);
    const PositionsUnderFire* GetPositionsUnderFire() const
                                { return &m_PositionsUnderFire; }

    bool                      OccupiesPosition(unsigned int aLocationIdx) const;
    bool                      IsAbleToFireAtLocation(unsigned int aLocationIdx) const;

    unsigned int              GetLocation() const
                                { return m_Position; }

  protected:
    unsigned int       m_Position;

    PositionsUnderFire m_PositionsUnderFire;
};


#endif // _ASE_THREAT_H_