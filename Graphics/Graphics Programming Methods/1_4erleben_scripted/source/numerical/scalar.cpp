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
#if(_MSC_VER == 1200)
#pragma warning(disable:4244)   // No warnings on precision truncation
#pragma warning(disable:4305)   // No warnings on precision truncation
#pragma warning(disable:4786)   // stupid symbol size limitation
#endif
#include <numerical/scalar.h>

/**
 * Precision correction.
 * This function forces the specified value to zero if it is numerical
 * less than the precision.
 *
 * @param value        The variable that should be forced. Upon return
 *                     the value would have been truncated to zero if
 *                     the original value was less than the numerial limit.
 */
void truncate(scalar & value){
  if((value>-precision)&&(value<precision))
    value = 0;
};
