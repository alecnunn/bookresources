//GAPBILExample
//Copyright John Manslow
//29/09/2001

#ifndef _CWorld_
#define _CWorld_

class CWorld
{
public:

	CWorld(const unsigned long, const unsigned long);
	~CWorld();

	void Initialise(void);
	void Draw(CDC*);

	unsigned long ulWorldSizeX;
	unsigned long ulWorldSizeY;

	unsigned long ulCharacterLocationX;
	unsigned long ulCharacterLocationY;

	int **ppnWorld;
};

#endif
