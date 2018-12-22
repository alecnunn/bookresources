       /////////////////////////////////////////////////////////
      ////-------------------------------------------------////
     ////          Fast Line-of-sight implementation      ////
    //// (c) Copyright 2001, Tom Vykruta                 ////
   ////              All Rights Reserved.               ////
  ////    *not to be used without author's permission* ////
 ////-------------------------------------------------////
/////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////
  // TransformMat.cpp  : Generic code for Line-Of-Sight. Works in 3D.
 //
/////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LOSGridDemo.h"
#include "TransformMat.h"
#include "ChildView.h"

extern Layer g_Layer;
extern CPaintDC* g_dc;

// This function would in reality do a more expensive ray-polygon test and return the impact point.
// For purposes of this demo it only draws a ray representing the element division
bool ElemDivisionCheck(const VECTOR3D& vFrom,const VECTOR3D& vTo, const int iElemX, const int iElemZ, VECTOR3D *pvImpact)
{
	VECTOR2D vTemp;
	vTemp = VECTOR2D((float)iElemX * ELEMWID, (float)iElemZ * ELEMWID);
	DrawLine2(vTemp, VECTOR2D(ELEMWID,ELEMWID), 2);

	return(FALSE);
}

// This is the meat of the package, where it all happens
// note: It is assumed that vFrom and vTo are already clipped to the layer's ABBox! 
// squiggly lines ~^~^ represent visualization code
bool Layer::DoesElemCollideRay(const VECTOR3D &vFrom, const VECTOR3D &vTo, VECTOR3D* pvImpact) const
{
	VECTOR2D _DEBUGvOrig(0,0);

	VECTOR2D vPathLu;
	VECTOR2D vFromLu, vToLu;
	int iNumSteps;
	float rXCurr, rZCurr;

	vFromLu.Set(vFrom.x/rELEMWID,vFrom.z/rELEMWID); // convert to lu to make things easier
	vToLu.Set(vTo.x/rELEMWID,vTo.z/rELEMWID);
	{
		VECTOR2D vLayerOrig;
		vLayerOrig.Set( (float)GetXOrigin(), (float)GetZOrigin() );
		vFromLu -= vLayerOrig;
		vToLu -= vLayerOrig;
	}

	vPathLu.Set(vToLu.x - vFromLu.x, vToLu.y - vFromLu.y);

// ~^~^~^~^~^~^~~^DEBUG CODE~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^
DrawLine2(VECTOR2D(vFromLu.x*ELEMWID, vFromLu.y*ELEMWID), VECTOR2D(vPathLu.x*ELEMWID, vPathLu.y*ELEMWID), 1);
// ~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^

	// We will invert/mirror/rotate the grid in such a way that our line is always going +X, +Y, where slope is < 0.5.
	TransformMatrix trans2dMat(vPathLu, vPathLu);
	trans2dMat.TransformToGridSpace(vFromLu.x, vFromLu.y);
	trans2dMat.TransformToGridSpace(vToLu.x, vToLu.y);
	trans2dMat.TransformToGridSpace(vPathLu.x, vPathLu.y);

// ~^~^~^~^~^~^~~^DEBUG CODE~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^
VECTOR2D vTemp, vTempTo;
DrawLine(VECTOR2D(vFromLu.x*ELEMWID, vFromLu.y*ELEMWID), VECTOR2D(vPathLu.x*ELEMWID, vPathLu.y*ELEMWID), 2);
// ~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^

	// It' possible we will extend beyond layer boundaries.. make sure we dont try to check these
	int iZAbsoluteStart = (int)floor(vFromLu.y);
	int iZAbsoluteEnd   = (int)ceil(vToLu.y);

	const int iXMod = 0; // which way do we step to get a normal division line?
	const int iZMod = 1;
	const int iXModAlt = -1; // which way do we step to get alternate divison line?
	const int iZModAlt = 0;

	int xShiftDivision, zShiftDivision;
	int xShiftDivisionAlt, zShiftDivisionAlt;
	xShiftDivision = 1; // offset vector from the origin of the element
	zShiftDivision = 0;
	xShiftDivisionAlt = 1;
	zShiftDivisionAlt = 1;
	trans2dMat.TransformOffsetToRealSpace(xShiftDivision,zShiftDivision);	
	trans2dMat.TransformOffsetToRealSpace(xShiftDivisionAlt,zShiftDivisionAlt);	

// ~^~^~^~^~^~^~~^visualization code~^~^~^~^~~^~^~^~^~^~^~^~~^
char message[128];
g_dc->Rectangle(40,270,240,320);
wsprintf(message, "NegX NegY FlipXY");
g_dc->TextOut(50, 280, message, strlen(message) );
wsprintf(message, "[ %i ][ %i ][ %i ]", (int)trans2dMat.matrix[TransformMatrix::tNegateX], 
	(int)trans2dMat.matrix[TransformMatrix::tNegateY],
	(int)trans2dMat.matrix[TransformMatrix::tInvertXY]);
g_dc->TextOut(50, 300, message, strlen(message) );

g_dc->Rectangle(40,330,240,395);
wsprintf(message, "Division Offsets (X,Y)");
g_dc->TextOut(50, 340, message, strlen(message) );
wsprintf(message, "Normal:    (%i, %i)", xShiftDivision, zShiftDivision );
g_dc->TextOut(50, 360, message, strlen(message) );
wsprintf(message, "Alternate: (%i, %i)", xShiftDivisionAlt, zShiftDivisionAlt );
g_dc->TextOut(50, 375, message, strlen(message) );
// ~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^

	// We have an entry point and exit point based on a layer's bounding box.
	// figure out how much we need to extend the vFrom->vTo ray by to get to the first element cross.  We do this by snapping the vert along the XZ
	// plane to the appropriate division line, then moving the other 2 components by same amount.

	float rStep, rStepY;
	float rYCurrWu;
	// Depending on which way we are stepping, pick the proper land division line to start on
	// and fraction of total ray we have to move by to get to the preious and last element crossing
	{
		float rShiftFract, rShiftFractEnd;
		float rXEnd, rZEnd;

		rXCurr = floor((double)vFromLu.x);
		rXEnd = ceil((double)vToLu.x);
		
		rShiftFract = (vFromLu.x - rXCurr);
		rShiftFract *= -1.0f / vPathLu.x;

		rShiftFractEnd = (vToLu.x - rXEnd);
		rShiftFractEnd *= -1.0f / vPathLu.x;

		// Extend the ray - it's possible we will extend them off the layer, catch that later
		rXCurr = vFromLu.x + vPathLu.x * rShiftFract;
		rZCurr = vFromLu.y + vPathLu.y * rShiftFract;

		rXEnd = vToLu.x + vPathLu.x * rShiftFractEnd;
		rZEnd = vToLu.y + vPathLu.y * rShiftFractEnd;

		rYCurrWu = vFrom.y + (vTo.y - vFrom.y) * rShiftFract;
		vPathLu.Set( rXEnd - rXCurr, rZEnd - rZCurr );

		// figure out what our DX and DY is
		float r1_Sections = 1.0f / vPathLu.dx;
		rStep = vPathLu.dy * r1_Sections;

		// shift the Y component once again by the same fraction that x and z where moved by
		rStepY = (vTo.y - vFrom.y) * (1.0f + -rShiftFract + rShiftFractEnd) * r1_Sections;

		// how many elements we iterate to reach the exit point
		iNumSteps = (int)(vPathLu.dx + 0.5f) + 1; // 0.5f is for rounding (float) inaccuracies!
	}

	// precompute 1/rstep for similar triangles when calculating height offset
	const float r1_StepHeight = rStepY * (1.f / rStep);

	int iZTest, iXTest;
	// to get around expensiv efloat->int conversions, using a float counter.. when it reaches 1, increment int
	iZTest = (int)floor(rZCurr); // this cannot be FTOI because negative numbers woudln't be handled correctly
	rZCurr = rZCurr - (float)iZTest;
	iXTest = (int)(rXCurr + SIGN(rXCurr) * 0.5f); // 0.5f is for rounding (float) inaccuracies!

///////////////////////////
	enum eTestPoint	{etpBelow = 0, etpAbove, etpBetween, etpUninitialized};
	eTestPoint ePosInitial = etpUninitialized; // our first test point lies abov eor below the layer..
	eTestPoint ePosNow; // current testpoint
	
	BOOL fAltDivision = FALSE;

	int iXModReal, iZModReal, iXModAltReal, iZModAltReal;
	// Figure out which way to step (in real grid space) to get the division line
	iXModReal = iXMod; // in local grid space, we step +Z to get division
	iZModReal = iZMod;
	trans2dMat.TransformToRealSpace(iXModReal, iZModReal);
	iXModAltReal = iXModAlt;// in local grid space, we step -X to get division
	iZModAltReal = iZModAlt;
	trans2dMat.TransformToRealSpace(iXModAltReal, iZModAltReal);

	int iXTestReal, iZTestReal;
	float rHeight;
	float rAltDivisionYCorrection = 0.0f;
	for (int i = 0; i < iNumSteps; )
	{
		// Get XZ coordinates transformed into real grid space
		iXTestReal = iXTest;
		iZTestReal = iZTest;
		trans2dMat.TransformToRealSpace(iXTestReal, iZTestReal);

		if (iZTest < iZAbsoluteStart || ((iZTest > iZAbsoluteEnd-1) && !fAltDivision) )
			goto skip;

		rHeight = rYCurrWu;
		
		float rHeight1, rHeight2;
		rHeight1 = VertHeight(iXTestReal, iZTestReal);
		if (fAltDivision)
			rHeight2 = VertHeight(iXTestReal + iXModAltReal, iZTestReal + iZModAltReal);
		else
			rHeight2 = VertHeight(iXTestReal + iXModReal, iZTestReal + iZModReal);

		ePosNow = (eTestPoint)(rHeight > rHeight1);

		if ( (eTestPoint)(rHeight > rHeight2) != ePosNow)
			ePosNow = etpBetween;

// ~^~^~^~^~^~^~~^DEBUG CODE~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^
	vTemp.Set((float)iXTest*ELEMWID, (float)iZTest*ELEMWID);
	vTempTo = VECTOR2D((fAltDivision ? iXModAlt : iXMod)*ELEMWID, (fAltDivision ? iZModAlt : iZMod)*ELEMWID);
	DrawLine(vTemp, vTempTo);
	
	vTemp.Set(iXTestReal*ELEMWID, iZTestReal*ELEMWID);
	vTempTo = VECTOR2D((fAltDivision ? iXModAltReal : iXModReal)*ELEMWID, (fAltDivision ? iZModAltReal : iZModReal)*ELEMWID);
	DrawLine2(vTemp, vTempTo);
// ~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^~^~^~^~^~^~^~~^

		if (ePosInitial == etpUninitialized)
			ePosInitial = ePosNow; // first time, initialize height, don't check verts
		else
		{
			// when i is 0, just initialize the etpPosNow
			if (ePosNow == etpBetween || ePosInitial  != ePosNow)
			{
				ePosInitial = ePosNow;
 				const int& xShift = (fAltDivision == FALSE) ? xShiftDivision : xShiftDivisionAlt;
				const int& zShift = (fAltDivision == FALSE) ? zShiftDivision : zShiftDivisionAlt;
				// In transformed space, to get the corner of elem, step back by 1X.  Transform this to get the real step.
				VECTOR3D vFromLocal = vFrom;
				vFromLocal -= this->GetOrigin();
				VECTOR3D vToLocal = vTo;
				vToLocal -= this->GetOrigin();
				if ( ElemDivisionCheck(vFromLocal,vToLocal,iXTestReal - xShift, iZTestReal - zShift, pvImpact) )
				{
					if (pvImpact)
						*pvImpact += this->GetOrigin();
					  return(TRUE);
				}
			}
		}

skip:
		if (fAltDivision)// Go through the same loop, but with this flag set as false
		{
			fAltDivision = FALSE;
			rYCurrWu += rAltDivisionYCorrection;
		}
		else
		{
			// move along our ray, to the next element division.
			rYCurrWu += rStepY;
			iXTest += 1;
			rZCurr += rStep;
			if ( rZCurr > 1.0f)
			{
				fAltDivision = TRUE; // we crossed a division that doesnt lie along the common axis..
				rZCurr -= 1.0f;
				iZTest += 1;
				// Since we are checking the alt division, but our height check is a measure at the regular division, we must figure out the
				// old height using similar triangles.  equation: (1.0 * rZCurr) / rStep;  gives us the horizontal fraction to step back by.
				rAltDivisionYCorrection = r1_StepHeight * rZCurr;
				rYCurrWu -= rAltDivisionYCorrection;
			}
			i++;
		}
	}
	return(FALSE);
}
