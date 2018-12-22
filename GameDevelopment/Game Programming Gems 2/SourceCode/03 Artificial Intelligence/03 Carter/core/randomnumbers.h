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
