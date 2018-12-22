#ifndef __DTW_H__
#define __DTW_H__

#include "FeatureExtraction.h"

/// Compute the minimum score using DTW for 2 sequences of features
///
/// @param sample vector of feature
/// @param samplesize number of vector
/// @param reference vector to compare to
/// @param referencesize number of reference's vector
///
/// @return minimum distance between two sequences
float DTW(features *sample, int samplesize, features *reference, int referencesize);

#endif //__DTW_H__