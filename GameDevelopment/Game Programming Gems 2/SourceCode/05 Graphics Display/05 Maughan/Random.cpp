/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#include "Random.h"

#include <math.h>

const double Random::a = 16807.0;
const double Random::m = 2147483647.0;

Random::operator double() {

    double t = a * seed;
    seed = t - m * floor(t/m);

    return seed/m;
}
