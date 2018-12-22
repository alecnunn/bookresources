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



typedef struct {
	float fX, fZ;	// 2D XZ worldspace coordinate
} VecXZ_t;

typedef struct {
	VecXZ_t aVertices[4];		// Clockwise, where [0]=left side of Leading Edge
	VecXZ_t aUnitNormals[4];	// Clockwise, where [0]=Leading Edge
	float fSectorDist;			// Distance from Leading Edge's midpoint to Trailing Edge's midpoint
	float fTotalPriorDist;		//   = previous sector's fTotalPriorDist + fSectorDist
} Sector_t;


typedef struct {
	vector3 WorldPos3D;	// Vehicle's origin in 3D worldspace
	Sector_t *pSector;	// Points to the sector the vehicle origin is currently in
} Vehicle_t;


float CalcUnitDistIntoSector( float fPointX, float fPointZ, const Sector_t *pSector ) {
	VecXZ_t VLP, VTP;
	float fDotL, fDotT;

	// Compute vector from point on Leading Edge to P:
	VLP.fX = fPointX - pSector->aVertices[0].fX;
	VLP.fZ = fPointZ - pSector->aVertices[0].fZ;

	// Compute vector from point on Trailing Edge to P:
	VTP.fX = fPointX - pSector->aVertices[2].fX;
	VTP.fZ = fPointZ - pSector->aVertices[2].fZ;

	// Compute (VLP dot Leading Edge Normal):
	fDotL = VLP.fX*pSector->aUnitNormals[0].fX + VLP.fZ*pSector->aUnitNormals[0].fZ;

	// Compute (VTP dot Trailing Edge Normal):
	fDotT = VTP.fX*pSector->aUnitNormals[2].fX + VTP.fZ*pSector->aUnitNormals[2].fZ;

	// Compute unit distance into sector and return it:
	return ( fDotL / (fDotL + fDotT) );
}

float fOneOverTotalSectorDist;	// Pre-computed to be the inverse sum of
								//    Sector_t::fSectorDist for all sectors.

float CalcUnitDistDownTrack( const Vehicle_t *pVehicle ) {
	float fUnitDistIntoSector, fDistDownTrack;

	// Compute how far vehicle is into its sector:
	fUnitDistIntoSector = CalcUnitDistIntoSector(
						pVehicle->WorldPos3D.x,
						pVehicle->WorldPos3D.z,
						pVehicle->pSector
					);

	// The distance down the track is the full distance across all previous
	// sectors, plus the partial distance into our current sector:
	fDistDownTrack = pVehicle->pSector->fTotalPriorDist
				+ pVehicle->pSector->fSectorDist * fUnitDistIntoSector;

	// Finally, our unit distance down the track is our distance so far
	// divided by the track's total distance:
	return fDistDownTrack * fOneOverTotalSectorDist;
}

