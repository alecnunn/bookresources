/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	RANDOMNUMBERS_H
#define	RANDOMNUMBERS_H

// Random number object. Supports separate random number streams - 
// one per RandomNumber object, restarting of random number streams -
// through GetSeed()/SetSeed().

class RandomNumbers
{
public:
	typedef	unsigned int RandType;
	RandomNumbers(RandType seed = 12345678);
	void SetSeed(RandType newseed);
	RandType GetSeed() const;
	RandType GetRandom() const;

	// Default assignment operator, destructor and copy constructor are fine.
private:
	mutable RandType m_currentSeed;
};

#endif
