/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Pseudo-Random Number Generator
// (C) 2000 Guy W. Lecky-Thompson

class pseudorandom
{
	private:
		long lSeed;
		long lGen1;
      long lGen2;

	public:
		pseudorandom();
		~pseudorandom();

		long Rand(long lMax);
      void SRand(long lSeed);
};

