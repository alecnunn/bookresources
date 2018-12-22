/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
#include "mtxlib.h"

#define LERP( fUnit, fV0, fV1 ) ( (1.0f-(fUnit))*fV0 + (fUnit)*fV1 )

typedef struct {
	float fR, fG, fB;		// RGB ambient light
} Ambient_t;

typedef struct {
	float fX, fZ;	// 2D XZ worldspace coordinate
} VecXZ_t;

typedef struct {
	VecXZ_t aVertices[4];	// Clockwise, where [0]=left side of Leading Edge
	VecXZ_t aUnitNormals[4];// Clockwise, where [0]=Leading Edge
	float fSectorDist;		// Distance from Leading Edge's midpoint to Trailing Edge's
	float fTotalPriorDist;	// = previous sector's fTotalPriorDist + fSectorDist

	Ambient_t LeadingAmbient;	// Ambient light at Leading Edge
	Ambient_t TrailingAmbient;	// Ambient light at Trailing Edge
} Sector_t;

typedef struct {
  vector3   WorldPos3D;      // Vehicle's origin in 3D worldspace
  Sector_t  *pSector;  // Points to the sector the vehicle origin is currently in
} Vehicle_t;


extern float CalcUnitDistIntoSector( float fPointX, float fPointZ, 
				     const Sector_t *pSector );


Ambient_t CalcAmbientLightLevel( const Vehicle_t *pVehicle ) {
	float fUnitDistIntoSector;
	Ambient_t *pLeadAmbient, *pTrailAmbient, RetAmbient;

	// Compute how far vehicle is into its sector:
	fUnitDistIntoSector = CalcUnitDistIntoSector(
						pVehicle->WorldPos3D.x,
						pVehicle->WorldPos3D.z,
						pVehicle->pSector
					);

	pLeadAmbient = &pVehicle->pSector->LeadingAmbient;
	pTrailAmbient = &pVehicle->pSector->TrailingAmbient;

	RetAmbient.fR = LERP( fUnitDistIntoSector, pLeadAmbient->fR, pTrailAmbient->fR );
	RetAmbient.fG = LERP( fUnitDistIntoSector, pLeadAmbient->fG, pTrailAmbient->fG );
	RetAmbient.fB = LERP( fUnitDistIntoSector, pLeadAmbient->fB, pTrailAmbient->fB );

	return RetAmbient;
}
