#ifndef AnimController_H
#define AnimController_H

#pragma warning(disable:4786)
#include <Map>

#define MAX_NAME 20

// --------------------------------------------------------------------------

// Bit flags categorizing animation by part of body it controls.
enum EnumAnimCategory {
	kACat_Invalid	= (0 << 0),

	kACat_LeftLeg			= (1 << 0),
	kACat_RightLeg			= (1 << 1),
	kACat_Waist				= (1 << 2),
	kACat_Tail				= (1 << 3),
	kACat_LowerReserved1	= (1 << 4),	// Replace when needed.
	kACat_LowerReserved2	= (1 << 5),	// Replace when needed.
	kACat_LowerReserved3	= (1 << 6),	// Replace when needed.
	kACat_LowerReserved4	= (1 << 7),	// Replace when needed.
	kACat_LowerBody			= kACat_LeftLeg | kACat_RightLeg | kACat_Waist | kACat_Tail | kACat_LowerReserved1 | kACat_LowerReserved2 | kACat_LowerReserved3 | kACat_LowerReserved4,

	kACat_Torso				= (1 << 8),
	kACat_RightArm			= (1 << 9),
	kACat_LeftArm			= (1 << 10),
	kACat_Head				= (1 << 11),
	kACat_UpperReserved1	= (1 << 12), // Replace when needed.
	kACat_UpperReserved2	= (1 << 13), // Replace when needed.
	kACat_UpperReserved3	= (1 << 14), // Replace when needed.
	kACat_UpperReserved4	= (1 << 15), // Replace when needed.
	kACat_UpperBody			= kACat_Torso | kACat_RightArm | kACat_LeftArm | kACat_Head | kACat_UpperReserved1 | kACat_UpperReserved2 | kACat_UpperReserved3 | kACat_UpperReserved4,

	kACat_FullBody	= 0xffffffff,
};

// --------------------------------------------------------------------------

struct AnimFileStruct
{
	char				szFileName[MAX_NAME];
	unsigned long		nPriority;
	EnumAnimCategory	eCategory;

	//
	// Other animation file data here...
	//
};

struct AnimInstanceStruct
{
	AnimInstanceStruct(AnimFileStruct* _pAnimFileStruct)
	{
		pAnimFileStruct = _pAnimFileStruct;
	}

	AnimFileStruct*	pAnimFileStruct;
	
	//
	// Other animation instance data here...
	//
};

// Animations sorted by category.
typedef std::map<unsigned long, AnimInstanceStruct, std::greater<unsigned long> > ANIM_INSTANCE_MAP;


// --------------------------------------------------------------------------

class CAnimController 
{
public:

	bool	PlayAnim(AnimFileStruct* pRequestedAnim);
	void	Print();

protected:

	ANIM_INSTANCE_MAP	m_mapAnimInstances;
};

// --------------------------------------------------------------------------

#endif

