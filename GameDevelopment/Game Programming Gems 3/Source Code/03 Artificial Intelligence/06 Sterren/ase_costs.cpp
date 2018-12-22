/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: all cost functions grouped into one file and class
*/


#include "stdafx.h"
#include "ase_costs.h"

#include "ase_terrain.h"
#include "ase_threatboard.h"
#include "ase_losapproxboard.h"
#include "asincludes.h"

#include <cmath>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// statics
const ASE_TerrainBoard*          ASE_Costs::m_TerrainBoard   = 0;
const ASE_ThreatBoard*           ASE_Costs::m_ThreatBoard    = 0;
const ASE_LOSApproximationBoard* ASE_Costs::m_LOFApproxBoard = 0;


// methods
void ASE_Costs::SetTerrainBoard(const ASE_TerrainBoard* theTerrainBoard)
  {
    assert( theTerrainBoard );
    m_TerrainBoard = theTerrainBoard;
  }


void ASE_Costs::SetThreatBoard(const ASE_ThreatBoard* theThreatBoard)  
  {
    assert( theThreatBoard );
    m_ThreatBoard = theThreatBoard;
  }



void ASE_Costs::SetLOFApproximationBoard(const ASE_LOSApproximationBoard* theLOFApproxBoard)  
  {
    assert( theLOFApproxBoard );
    m_LOFApproxBoard = theLOFApproxBoard;
  }



float ASE_Costs::CellBasedNormalMovementCosts(_asNode* aFromNode, _asNode* aToNode)
  {
	  float cost;
    cost = m_TerrainBoard->GetMovementCostsCellBased(aFromNode->GetX(), aFromNode->GetY(), 
                                                     aToNode->GetX(),   aToNode->GetY());

	  return cost;
  }


float ASE_Costs::CellBasedRelativeMovementCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    float cost;
    cost = static_cast<float>
                   ( fabs(  m_TerrainBoard->GetCellValue(aFromNode->GetX(), aFromNode->GetY())
                          - m_TerrainBoard->GetCellValue(aToNode->GetX(), aToNode->GetY())
                         )
                   ) 
                 +1.0f;

	  return cost;
  }


float ASE_Costs::VectorBasedNormalMovementCosts(_asNode* aFromNode, _asNode* aToNode)
  {
	  float cost;
    cost = m_TerrainBoard->GetMovementCostsVectorBased(aFromNode->GetX(), aFromNode->GetY(), 
                                                       aToNode->GetX(),   aToNode->GetY());

	  return cost;
  }


float ASE_Costs::ThreatsNoCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return 0;
  }


float ASE_Costs::CellBasedThreatsSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eAnyLineOfFire == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 4;

    float costs;
    if ( !m_ThreatBoard->IsEmptyCell(aToNode->GetX(), aToNode->GetY()) )
      costs = kThreatCostFactor;
    else
      costs = 0;
    return costs;
  }


float ASE_Costs::VectorBasedThreatsSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eAnyLineOfFire == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 2;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float costs;
    costs = risk * traveltime;

    return costs;
  }


float ASE_Costs::CellBasedThreatsAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedThreatsAimingCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedThreatsAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    const float kThreatCostFactor = 0.6f;
    const float kMaxAim           = 5;
    const float kAimDecay         = 0.8f;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float aim;
    aim = aFromNode->GetThreatAimQuality();
    if ( risk == 0 )
      {
        // just do decay, travel time based
        aim *= static_cast<float>(pow(kAimDecay, traveltime));
      }    
    else
      {
        aim = min(kMaxAim, aim + traveltime); 
      }
    aToNode->SetThreatAimQuality(aim);

    float costs;
    costs = risk * traveltime * (1 + aim);

    return costs;
  }


float ASE_Costs::VectorBasedDistanceBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireDistance == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 4;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float costs;
    costs = risk * traveltime;

    return costs;
  }


float ASE_Costs::CellBasedDistanceBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    //! \note: assumes weapon effectivity decreases with linear distance, not with cell distance
    return VectorBasedDistanceBasedSimpleCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedDistanceBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireDistance == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 1.2f;
    const float kMaxAim           = 5;
    const float kAimDecay         = 0.8f;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float aim;
    aim = aFromNode->GetThreatAimQuality();
    if ( risk == 0 )
      {
        // just do decay, travel time based
        aim *= static_cast<float>(pow(kAimDecay, traveltime));
      }    
    else
      {
        aim = min(kMaxAim, aim + traveltime); 
      }
    aToNode->SetThreatAimQuality(aim);

    float costs;
    costs = risk * traveltime * (1 + aim);

    return costs;
  }


float ASE_Costs::CellBasedDistanceBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    //! \note: assumes weapon effectivity decreases with linear distance, not with cell distance
    return VectorBasedDistanceBasedAimingCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireCount == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 8;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float costs;
    costs = risk * traveltime;

    return costs;
  }


float ASE_Costs::CellBasedCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedCountBasedSimpleCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireCount == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 1.2f;
    const float kMaxAim           = 5;
    const float kAimDecay         = 0.8f;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float aim;
    aim = aFromNode->GetThreatAimQuality();
    if ( risk == 0 )
      {
        // just do decay, travel time based
        aim *= static_cast<float>(pow(kAimDecay, traveltime));
      }    
    else
      {
        aim = min(kMaxAim, aim + traveltime); 
      }
    aToNode->SetThreatAimQuality(aim);

    float costs;
    costs = risk * traveltime * (1 + aim);

    return costs;
  }


float ASE_Costs::CellBasedCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedCountBasedAimingCosts(aFromNode, aToNode);
  }


float ASE_Costs::CellBasedDistanceAndCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedDistanceAndCountBasedSimpleCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedDistanceAndCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireCountAndDistance == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 4;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float costs;
    costs = risk * traveltime;

    return costs;
  }


float ASE_Costs::CellBasedDistanceAndCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedDistanceAndCountBasedAimingCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedDistanceAndCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    assert( ASE_ThreatBoard::eMinLineOfFireCountAndDistance == m_ThreatBoard->GetRiskModus() );

    const float kThreatCostFactor = 0.6f;
    const float kMaxAim           = 5;
    const float kAimDecay         = 0.8f;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_ThreatBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_ThreatBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float aim;
    aim = aFromNode->GetThreatAimQuality();
    if ( risk == 0 )
      {
        // just do decay, travel time based
        aim *= static_cast<float>(pow(kAimDecay, traveltime));
      }    
    else
      {
        aim = min(kMaxAim, aim + traveltime); 
      }
    aToNode->SetThreatAimQuality(aim);

    float costs;
    costs = risk * traveltime * (1 + aim);

    return costs;
  }


float ASE_Costs::CellBasedThreatsApproximateLOFSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedThreatsApproximateLOFSimpleCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedThreatsApproximateLOFSimpleCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    const float kThreatCostFactor = 4;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_LOFApproxBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_LOFApproxBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float costs;
    costs = risk * traveltime;

    return costs;
  }


float ASE_Costs::CellBasedThreatsApproximateLOFAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    return VectorBasedThreatsApproximateLOFAimingCosts(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedThreatsApproximateLOFAimingCosts(_asNode* aFromNode, _asNode* aToNode)
  {
    const float kThreatCostFactor = 0.6f;
    const float kMaxAim           = 5;
    const float kAimDecay         = 0.8f;

    float traveltime;
    traveltime = m_TerrainBoard->GetTravelTime(aFromNode->GetX(), aFromNode->GetY(), 
                                               aToNode->GetX(),   aToNode->GetY());

    float risk;
    risk  = m_LOFApproxBoard->GetRiskValue(aToNode->GetX(), aToNode->GetY());
    risk += m_LOFApproxBoard->GetRiskValue(aFromNode->GetX(), aFromNode->GetY());
    risk *= kThreatCostFactor;

    float aim;
    aim = aFromNode->GetThreatAimQuality();
    if ( risk == 0 )
      {
        // just do decay, travel time based
        aim *= static_cast<float>(pow(kAimDecay, traveltime));
      }    
    else
      {
        aim = min(kMaxAim, aim + traveltime); 
      }
    aToNode->SetThreatAimQuality(aim);

    float costs;
    costs = risk * traveltime * (1 + aim);

    return costs;
  }


float ASE_Costs::CellBasedNormalMovementHeuristic(_asNode* aFromNode, _asNode* aToNode)
  {
    int manhattandist;
    manhattandist =   abs(aFromNode->GetX() - aToNode->GetX())
                    + abs(aFromNode->GetY() - aToNode->GetY());
    return static_cast<float>(manhattandist);
  }


float ASE_Costs::CellBasedRelativeMovementHeuristic(_asNode* aFromNode, _asNode* aToNode)
  {
    return CellBasedNormalMovementHeuristic(aFromNode, aToNode);
  }


float ASE_Costs::VectorBasedNormalMovementHeuristic(_asNode* aFromNode, _asNode* aToNode)
  {
    float distx;
    float disty;
    distx  = static_cast<float>(aToNode->GetX() - aFromNode->GetX());
    distx *= distx;
    disty  = static_cast<float>(aToNode->GetY() - aFromNode->GetY());
    disty *= disty;
    return static_cast<float>(sqrt(distx + disty));
  }


float ASE_Costs::ThreatsAnyModeAnyMovementHeuristic(_asNode* aFromNode, _asNode* aToNode)
  {
    return 0;
  }


