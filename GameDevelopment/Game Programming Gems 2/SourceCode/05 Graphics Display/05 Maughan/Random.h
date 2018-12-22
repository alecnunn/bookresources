/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef Random_INCLUDED
#define Random_INCLUDED

struct Random {

  explicit Random(double s = 1.0) : seed(s) {}

  operator double();

 private:

  double seed;

  static const double a;
  static const double m;
};

#endif
