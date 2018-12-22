/*
 * Code Sample 1- Catmull-Rom computation for a given X, Y or Z
 *
 * This is a function that, given coordinate family, ie: the X of our point, 2 points back, 1 point back and the next point,
 * we get the X result. Use this for X, Y and Z. See Code-sample 2 for usage.
 *
 */

inline float GetSpline(float inOurPoint, float inPercent, 
                 float inTSquared, float inTCubed,
                 float inBack2, float inBack1, float inNext)
{	
	return (0.5f * (2.0f * inBack1 + ((-inBack2 + inOurPoint) * inPercent) +
	           ((2.0f * inBack2 - 5.0f * inBack1 + 4.0f * inOurPoint - inNext) * inTSquared) + 
	           ((-inBack2 + 3.0f * inBack1 - 3.0f * inOurPoint) + inNext)* inTCubed)));
}




/*
 * Code Sample 2 - Advancing a point along a spline path
 *
 * This shows how to advance a Catmull-Rom spline given a percent (0.0F - 1.0F) complete it is.
 * Note: You need your goals point, the goal point 2 points behind you, 1 point behind you and 1 point ahead of you 
 * for the spline to work.
 */
 
void AdvanceSpline(float inPercent, GE3DPoint& ioPoint, const GE3DPoint& inBack2,
				   const GE3DPoint& inBack1, const GE3DPoint& inNext)
{ 
	inPercent = min(1.0F, inPercent);
	float theTSquared = inPercent * inPercent;
	float theTCubed = theTSquared * inPercent;
	
	/* Spline for X */
	ioPoint.mX = ::GetSpline(this->mPoint.mX, inPercent, theTSquared, theTCubed, 
		   	   					inBack2.mX, inBack1.mX, inNext.mX);
	
	/* Spline for Y */
	ioPoint.mY = ::GetSpline(this->mPoint.mY, inPercent, theTSquared, theTCubed, 
		   	   					inBack2.mY, inBack1.mY, inNext.mY);
	
	/* Spline for Z */
	ioPoint.mZ = ::GetSpline(this->mPoint.mZ, inPercent, theTSquared, theTCubed, 
								inBack2.mZ, inBack1.mZ, inNext.mZ);	
}		   




/*
 * Code Sample 3 - Cached spline calculations
 *
 * One handy way to optimize a spline path that isn't going to change due to map-to-terrain, or map-to-object situations
 * is to compute a cached version. If you cache the math, it can be pretty lightweight. First function shows HOW to cache 
 * the math into a floating point array. The next shows you how to use it at run time.
 */

/* Makes the original optimized calculations used later */
void MyPhysicsClass::ComputeOptimizedSplines(void)
{
	/* optimize for X */
	this->mOptimizedSplineCalcs[0] = (2.0f * this->mSplinePoints[1].mPoint.GetX());
	this->mOptimizedSplineCalcs[1] = (-this->mSplinePoints[0].mPoint.GetX() + this->mPoint.mPoint.GetX());
	this->mOptimizedSplineCalcs[2] = (2.0f * this->mSplinePoints[0].mPoint.GetX() - 5.0f * this->mSplinePoints[1].mPoint.GetX() + 4.0f * this->mPoint.mPoint.GetX() - this->mSplinePoints[2].mPoint.GetX());
	this->mOptimizedSplineCalcs[3] = (-this->mSplinePoints[0].mPoint.GetX() + 3.0f * this->mSplinePoints[1].mPoint.GetX() - 3.0f * this->mPoint.mPoint.GetX() + this->mSplinePoints[2].mPoint.GetX());

	/* optimize for Y */
	this->mOptimizedSplineCalcs[4] = (2.0f * this->mSplinePoints[1].mPoint.GetY());
	this->mOptimizedSplineCalcs[5] = (-this->mSplinePoints[0].mPoint.GetY() + this->mPoint.mPoint.GetY());
	this->mOptimizedSplineCalcs[6] = (2.0f * this->mSplinePoints[0].mPoint.GetY() - 5.0f * this->mSplinePoints[1].mPoint.GetY() + 4.0f * this->mPoint.mPoint.GetY() - this->mSplinePoints[2].mPoint.GetY());
	this->mOptimizedSplineCalcs[7] = (-this->mSplinePoints[0].mPoint.GetY() + 3.0f * this->mSplinePoints[1].mPoint.GetY() - 3.0f * this->mPoint.mPoint.GetY() + this->mSplinePoints[2].mPoint.GetY());

	/* optimize for Z */
	this->mOptimizedSplineCalcs[8] = (2.0f * this->mSplinePoints[1].mPoint.GetZ());
	this->mOptimizedSplineCalcs[9] = (-this->mSplinePoints[0].mPoint.GetZ() + this->mPoint.mPoint.GetZ());
	this->mOptimizedSplineCalcs[10] = (2.0f * this->mSplinePoints[0].mPoint.GetZ() - 5.0f * this->mSplinePoints[1].mPoint.GetZ() + 4.0f * this->mPoint.mPoint.GetZ() - this->mSplinePoints[2].mPoint.GetZ());
	this->mOptimizedSplineCalcs[11] = (-this->mSplinePoints[0].mPoint.GetZ() + 3.0f * this->mSplinePoints[1].mPoint.GetZ() - 3.0f * this->mPoint.mPoint.GetZ() + this->mSplinePoints[2].mPoint.GetZ());

	/* indicates we use these optimized splines in AdvancePosition. If at any time we want to rever to using the real ones, we just set this to false.  */
	this->mUseOptimizedSplineCalcs = true;
}

/* uses a precomputed array to compute a fast spline */
inline float MyPhysicsClass::GetFastSpline(float* inArray, float inSquared, float inCubed, float inPercent)
{ 
	return (0.5f * (inArray[0] + (inArray[1] * inPercent) + (inArray[2] * theTSquared) + (inArray[3] * theTCubed)));
}

/* simplified Advance spline method using the cached system. */
void MyPhysicsClass::AdvanceSpline(float inPercent, GE3DPoint& ioPoint)
{ 	
	ioPoint.mX = this->GetFastSpline(mOptimizedSplines, theSqd, theCube, thePercent);
	
	/* Off set our array start, and compute! */
	ioPoint.mY = this->GetFastSpline(mOptimizedSplines + 4, theSqd, theCube, thePercent);
	
	/* Off set our array start, and compute! */
	ioPoint.mZ = this->GetFastSpline(mOptimizedSplines + 8, theSqd, theCube, thePercent);
}





/*
 * Code Sample 4 - Update method of the prescripted physics engine.
 *
 * Simple version of the prescripted physics update pump. 
 * Return false when the physics event has completed. 
 * Note: Its handy to derive this for classes and call the base-version ::Update.. catching the
 * false result to do different things once the event is over like kill a unit, or use some other
 * type of physics to stand them up, etc.
 */

bool
MyPhysicsClass::Update(uint32 inWorldTimeInMilliseconds, float inTickRate, const GETransformation& inCurrentPosition, 
							bool inUpdateUnitAnimation, bool inCheckSelfMovement)
{
	float		thePercent;	
	bool		theResult = true;
	bool		thePointMatches;
	
	/* Ensure our next point is the right one (time-wise) */
	AdjustToCorrectPointUsingTime(inWorldTime);
	
	/* any physics nodes? */
	if(mPhysicsNodes.empty() == false)
	{	
		GE3DPoint thePoint;
	
		/* get our point */
		inCurrentLocation.GetPosition(thePoint);
	
		/* get the first item */
		FPNode& theNode = mPhysicsNodes.front();
	
		/* compute the percent(it will be 0-1.0F because
	       of the "AdjustToCorrectPoint method" */
		thePercent = ((float)(inWorldTime - theNode.mStartTime)) * theNode.mInverseTotalTime;
	
		/* advance Orientation */
		theNode.AdvanceOrientation(thePercent, this->mQuatOrientation);
		outResultOrientation = mQuatOrientation;
		
		/* Advance our location */
		theNode.AdvanceSpline(thePercent, thePoint, theNode.mSplines[0], theNode.mSplines[1],theNode.mSplines[2]);
	
		/* Rotate it at the correct spot. (head/middle/etc) */
		ApplyRotation(theNode, outTransform, thePoint);
	}
	else
	{
		theResult = false;
	}
}




/*
 * Code Sample 5 - AdvanceOrientation with a ramp.
 *
 * Ramps are very useful in changing the percent complete.
 * 
 */

inline void MyPhysicsClass::AdvanceOrientation(float inPercent, Quaternion& outQ)
{	
	/* Change the incoming percent by using a ramp */
	inPercent = ProcessRamp(inPercent, this->mOrientationRamp, this->mOrientationRampModifier);

	/* slerp to my-lou my darling. */
	this->mStartQuat.Slerp(mQuaternion, inPercent, outQ);
}





/*
 * Code Sample 6 - Using the current quaternion to rotate about the desired point on the object.
 *
 * This is one version of changing how you rotate an object about a point.. ie: if you want to flip a guy, but make their
 * waist their center of gravity, yet their 3D Position is at their feet, use the rotation modifier, bring the point up their height to do it.
 * 
 */
 
void MyPhysicsClass::ApplyRotationTranslation(RTSPFakePhysicsNode* ioNode, GETransformation& ioFinalTransform, const GE3DPoint& ioPoint)
{
	GETransformation	theRotateCenter;
	GETransformation	theRotateCenterInverse;
	GETransformation	theTranslation;
	GETransformation	theRotation;
	GETransformation	theResult1;
	GETransformation	theResult2;
	GE3DPoint		thePoint;
	
	/* put it into transform. */
	this->mQuatOrientation.ConvertToTransform(theRotation);
	
	/* set the translation */
	theTranslation.SetPosition(inPoint);
			
	/* set the rotation center (adjusting for Z, not XY) */
	theRotateCenter.SetZ(this->mHeight * ioNode.mRotationModifier);
	
	/* make inverse (cache this and the main transform) */
	theRotateCenter.MakeInverse(theRotateCenterInverse);
	
	/* apply the rotation center to the world translation */
	theTranslation.Apply(theRotateCenter, theResult1);
	
	/* apply the world/rotated to the result */
	theResult1.Apply(theRotation, theResult2);
	
	/* convert from local space to world space. */
	theResult2.Apply(theRotateCenterInverse, ioFinalTransform);
}


/*
 * Code Sample 7 - A Simple tornado physics event.
 *
 * This is how simple something like making a tornado throw someone around can be. 
 * In this sample, the YourFavoriteTornadoClass derives from the base prescripted physics class.
 * Note: It's simplified for print, so be sure to experimient with lots of details!
 * 
 */

void YourFavoriteTornadoClass::Init(EEComplexUnit* inUnit, const GE3DPoint& inCenter, uint32 inTime, float inRadius, float inVelocity)
{
	/* using velocity and distance, get the time */
	unsigned long theTimePerIteration = ::FloatToUnsignedLong((kPI * (2.0F * inRadius)) / inVelocity) *  1000;
	
	/* compute the iterations needed */
	long theTornadoWaypointsPerPI = 35;
	float theFloatIters = ((float)inTime /     (float)theTimePerIteration);
	float theInterval = (theTimePerIteration / theTornadoWaypointsPerPI);
	float theIncPerLoop = (1.0F / theTornadoWaypointsPerPI);
	
	/* set the starting point. */
	thePNode.SetStartingQuat(inCurrentQuat);
	theRotationTransform.SetPosition(inCenter);
	theNewPoint = inStartPoint;
	
	/* animation - You can specify a panic animation */
	thePNode.SetAnimation(inAnimation);
	
	/* set the point */
	thePNode.SetPoint(theNewPoint);
	
	/* Start NEW POINT */
	long theNextYPRCounter = 0;
	long theZCounter = 0;
	
	/* Rotation amount we'll use */
	float theRotationAmount = -((kTwoPI / 35.0F) * 2.0F);
	theZCounter = (theTornadoWaypointsPerPI / 4);
	
	/* Loop over all the points */
	for(theFLoop = 0.0F; 
	    theFLoop < theFloatIters; 
	    theFLoop += theIncPerLoop, 
	    theZCounter--,	theNextYPRCounter--)
	{	
		/* Cache the last Z for use later */
		theLastZ = theNewPoint.GetZ();
	
		/* Reset yaw/pitch/roll increments? */
		if(theNextYPRCounter == 0)
		{	
			/* setup next */
			theNextYPRCounter = 4;
	
			/* Inverse instead of dividing 3 times */
			theInverse = 1.0F / (float)theNextYPRCounter;
	
			/* compute the change range */
			theYaw = GetRandom(-TwoPI, TwoPI) * theInverse;	
			thePitch = GetRandom(-TwoPI, TwoPI) * theInverse;
			theRoll = GetRandom(-TwoPI, TwoPI) * theInverse;
		}
	
		/* Orientation setup */
		thePNode.SetOriYPR(theOri.mYaw + theYaw, theOri.mPitch + thePitch, theOri.mRoll + theRoll);
							
		/* Vector/destination point */		
		theRotationTransform.RotateByYaw(theRotationAmount);
		theRotationYaw = theRotationTransform.GetYaw();
	
		/* Tip: Tweak radius and time to make a funnel */
		/* Compute the local point spacing in relation to the
	      radius and transform into world space */
		theLocal.SetY(inRadius);		
		theRotationTransform.Apply(theLocal, theWorld);
	
		/* Rotate world. */
		theFloatPoint = theWorld;		
		theFloatPoint.RotatePointAroundZero(theRotationYaw);
		theFloatPoint.SetZ(theWorld.GetZ());
		theNewPoint = inCenter;
		theNewPoint += theFloatPoint;
	
		/* Get a new Z? */
		if(theZCounter == 0) 
		{	
			/* reset it */
			theZCounter = (theTornadoWaypointsPerPI / 4);
				
			/* compute Z Change (0.0F = TERRAIN Z) */
			theZChange = GetRandom(0.5F, 5.0F) + 0.0F; 
			theZChange = (theZChange - theLastZ ) / (float)theZCounter;
		}
		
		/* Set the position, and add the Z to our current Z */
		thePNode.SetPoint(theNewPoint.GetX(), theNewPoint.GetY(), theLastZ + theZChange);
	
		/* Time this will take. */
		theCurrentTime = thePNode.SetTime(theCurrentTime, theCurrentTime + theInterval);
	
		/* add to our list of waypoints we'll use later */
		theNodes.push_back(thePNode);
	}
	
	/* add those waypoints to the physics event, your done! */
	this->AddPoints(theNodes);
}
