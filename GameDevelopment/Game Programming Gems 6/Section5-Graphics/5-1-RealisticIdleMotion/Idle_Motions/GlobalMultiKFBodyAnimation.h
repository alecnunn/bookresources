/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef GlobalMultiKFBodyAnimation_H
#define GlobalMultiKFBodyAnimation_H

class SinglePCAnimation;
class KeyFrame;
class KeyFrameBodyAnimation;

#include <vector>

/**
 * This class defines an animation based on seperate global principal components
 * that each have their own timekey-pc value relations.
 */
class GlobalMultiKFBodyAnimation 
{
public:

	/**
	 * Default constructor.
	 * Initializes the globalpc body animation object.
	 */
	GlobalMultiKFBodyAnimation(void);

	/**
	 * Destructor.
	 */
	~GlobalMultiKFBodyAnimation(void);

	/**
	 * Returns last timekey of this animation sequence in milliseconds.
	 *
	 * @return				last timekey of this animation sequence as float (ms)
	 */
	float getLastTimeKey() const;

	/**
	 * Returns first timekey of this animation sequence in milliseconds.
	 *
	 * @return				first timekey of this animation sequence as float (ms)
	 */
	float getFirstTimeKey() const;

	/**
	 * Adds a single pc animation to this multipc animation.
	 *
	 * @param s				the single pc animation to add
	 */
	void addSinglePCAnimation(SinglePCAnimation* s);

	/**
	 * Returns a single pc animation object of a specified pcnr. If the
	 * pc nr is not in the animation, this method returns NULL.
	 *
	 * @param pcnr			the pc number
	 *
	 * @return				the corresponding single pc animation
	 */
	SinglePCAnimation* getSinglePCAnimation(const int& pcnr);

	/**
	 * Returns a frame at a specified timekey.
	 *
	 * @param timekey		the timekey
	 *
	 * @return				the corresponding frame
	 */
	KeyFrame* getFrame(float timekey);

	/**
	 * Fills in a normal keyframe body animation with a specified framerate.
	 * Any animation data in the object passed on to this method will be erased.
	 *
	 * @param fps			the amount of frames per second
	 * @param k				the KF body animation object to be filled
	 */
	void convertToKFBA(const int& fps, KeyFrameBodyAnimation* k);

	/**
	 * Removes all key frames before a specified timekey (excluding this timekey).
	 *
	 * @param timekey		the timekey
	 */
	void removeKeyFramesBefore(const float& timekey);

	/**
	 * Clears this animation object.
	 */
	void clear();

	/**
	 * Clears the cache of this animation object.
	 */
	void clearCache();

private:

	/**
	 * the set of single pc animations
	 */
	SinglePCAnimation** pcset_;

	/**
	 * the size of the single pc animation set
	 */
	int size_;

	/**
	 * vector containing pointers to all the keyframes
	 */
	std::vector<KeyFrame*>* keyframes_cache_;

	/**
	 * vector containing all the timekey values in milliseconds as floats
	 */
	std::vector<float>* timekeys_cache_;

};

#endif // ifndef GlobalMultiKFBodyAnimation_H
