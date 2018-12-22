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


#ifndef _ASE_COSTS_H_
#define _ASE_COSTS_H_

#if _MSC_VER > 1000
#pragma once
#endif


class ASE_TerrainBoard;
class ASE_ThreatBoard;
class ASE_LOSApproximationBoard;
class _asNode;


class ASE_Costs
{
  public: // typedefs
    typedef float (*CostFunction)(_asNode *, _asNode *);

  public: // cost functions - movement
    static float CellBasedNormalMovementCosts(_asNode* aFromNode, _asNode* aToNode);
    static float CellBasedRelativeMovementCosts(_asNode* aFromNode, _asNode* aToNode);

    static float VectorBasedNormalMovementCosts(_asNode* aFromNode, _asNode* aToNode);

  public: // heuristics functions - movement
    static float CellBasedNormalMovementHeuristic(_asNode* aFromNode, _asNode* aToNode);
    static float CellBasedRelativeMovementHeuristic(_asNode* aFromNode, _asNode* aToNode);

    static float VectorBasedNormalMovementHeuristic(_asNode* aFromNode, _asNode* aToNode);

  public: // cost functions - threats
    static float ThreatsNoCosts(_asNode* aFromNode, _asNode* aToNode);
    static float CellBasedThreatsSimpleCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedThreatsSimpleCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedThreatsAimingCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedThreatsAimingCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedDistanceBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedDistanceBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedDistanceBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedDistanceBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedDistanceAndCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedDistanceAndCountBasedSimpleCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedDistanceAndCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedDistanceAndCountBasedAimingCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedThreatsApproximateLOFSimpleCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedThreatsApproximateLOFSimpleCosts(_asNode* aFromNode, _asNode* aToNode);

    static float CellBasedThreatsApproximateLOFAimingCosts(_asNode* aFromNode, _asNode* aToNode);
    static float VectorBasedThreatsApproximateLOFAimingCosts(_asNode* aFromNode, _asNode* aToNode);


  public: // heuristics functions - threats
    static float ThreatsAnyModeAnyMovementHeuristic(_asNode* aFromNode, _asNode* aToNode);


  public: // helper functions


  public: // setup functions
    static void SetTerrainBoard(const ASE_TerrainBoard* theTerrainBoard);
    static void SetThreatBoard(const ASE_ThreatBoard*   theThreatBoard);
    static void SetLOFApproximationBoard(const ASE_LOSApproximationBoard* theLOFApproxBoard);

  protected: // pointers to boards
    static const ASE_TerrainBoard*          m_TerrainBoard;
    static const ASE_ThreatBoard*           m_ThreatBoard;
    static const ASE_LOSApproximationBoard* m_LOFApproxBoard;
};


#endif // _ASE_COSTS_H_