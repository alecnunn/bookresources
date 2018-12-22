//===============================================================================
// @ DistanceTime.h
// 
// Class to compute Hermite spline for distance-time functions.  Builds the spline
// based on a set of keys and speeds -- slow, fast, linear, step and smooth --
// set at each key.
// ------------------------------------------------------------------------------
// Copyright (C) 2004 by James M. Van Verth. All rights reserved.
//===============================================================================

#ifndef __DistanceTime__h__
#define __DistanceTime__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

// possible speed values at each key
enum SpeedType { kSlow = 0, kFast, kLinear, kStep, kSmooth };

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

// information stored for each distance-time key
struct KeyInfo
{
    float     mDistance;
    float     mTime;
	SpeedType mInSpeed;
	SpeedType mOutSpeed;
};

// class for building and evaluating Hermite curve
class DistanceTime
{
public:
    // constructor/destructor
    DistanceTime();
    ~DistanceTime();

    // set up
	bool Initialize( const KeyInfo* keys,
					 unsigned int count );

    // destroy
    void Clean();

    // evaluate distance-time function
    float Evaluate( float t );

    // render curve
    void Render();

protected:
    // finish solving linear system for smooth areas of curve
    void FinishSmoothSubcurve( unsigned int current, float* U, float* z, 
                               unsigned int matrixSize );

    float*      mPositions;     // sample positions
    float*      mInTangents;    // incoming tangents at each point
    float*      mOutTangents;   // outgoing tangents on each point
    float*          mTimes;     // time to arrive at each point
    unsigned int    mCount;     // number of points and times

private:
    // copy operations
    // made private so they can't be used
    DistanceTime( const DistanceTime& other );
    DistanceTime& operator=( const DistanceTime& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
