////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002
//  Department of Computer Science Copenhagen University (DIKU).
//
//  All Rights Reserved.
//
//  This software is provided as is WITHOUT ANY WARRANTY; Use it on
//  you own risk.
//
//  Please send remarks, questions and bug reports to kenny@diku.dk
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _SCALAR_INCLUDED_
#define _SCALAR_INCLUDED_

/**
 * The definition of a scalar. This makes it easy to
 * change the underlying datatype later on 
 */
typedef double scalar;

/**
 * Some commonly used scalar constants.
 */
const scalar pi              = 3.1415926535897932384626433832795;
const scalar infinity        = 1e38;
const scalar precision       = 10e-15; //numeric_limits<scalar>::epsilon();
const scalar tiny            = 10e-20;
const scalar degree          = pi/180; //One degree in radians.
void truncate(scalar & value);

#endif //_SCALAR_INCLUDED_
