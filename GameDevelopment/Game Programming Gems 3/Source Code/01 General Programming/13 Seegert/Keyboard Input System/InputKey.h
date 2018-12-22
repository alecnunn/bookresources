#ifndef __INPUT_KEY_
#define __INPUT_KEY_

#include "Real Time Input.h"

/*
 * InputKey
 *
 * This simple class is the scan code and modifier
 * of a key.
 */

class InputKey
{
public:
							InputKey(uint8 inScanCode = 0, ISModifierState inModifier = kismsNone) 
								: mTimeStamp(0), mScanCode(inScanCode), mModifier(inModifier)
							{ }

	InputKey&				operator= (const InputKey& inKey)
							{
								this->mModifier  = inKey.mModifier;
								this->mScanCode	 = inKey.mScanCode;
								this->mTimeStamp = inKey.mTimeStamp;
								return *this;
							}

	inline bool				operator== (const InputKey& inThat) const
							{ 
								return ((this->mModifier == inThat.mModifier) && 
										(this->mScanCode == inThat.mScanCode)); 
							}

	inline bool				operator< (const InputKey& inThat) const
							{ 
								uint16 theFirst	 = this->Pack();
								uint16 theSecond = inThat.Pack();
								return (theFirst < theSecond); 
							}

	inline uint16			Pack(void) const
							{
								uint16 theResult;
								theResult = this->mModifier;
								theResult = theResult << 8;
								theResult = theResult | this->mScanCode;
								return theResult;
							}

	uint32					mTimeStamp;
	ISModifierState			mModifier;
	uint8					mScanCode;
};

/* typedefs */
typedef vector < InputKey > KeyListType;

#endif /*__INPUT_KEY_*/
