/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef KeyFrameBodyAnimation_H
#define KeyFrameBodyAnimation_H

#include <vector>
#include <string>
#include "KeyFrame.h"
#include "Interpolator.h"
#include "Posture.h"

class XMLNode;

/**
 * This class contains an animation of keyframes (that can be either skew sym
 * postures or global PC postures).
 */
class KeyFrameBodyAnimation 
{
public:

	/**
	 * Constructor.
	 * Constructs an empty key frame body animation.
	 */
	KeyFrameBodyAnimation();

	/**
	 * Copy constructor.
	 */
	KeyFrameBodyAnimation(KeyFrameBodyAnimation* kfba);

	/**
	 * Destructor.
	 */
	~KeyFrameBodyAnimation(void);

	/**
	 * Sorts the key frames so that the time keys are in chronological order.
	 */
	void sort();

	/**
	 * Returns the amount of key frames in this animation sequence.
	 *
	 * @return					an integer value representing the amount of
	 *							key frames in this sequence
	 */
	int getFrameAmount() const;

	/**
	 * Returns a key frame at a certain frame index.  If the index is out of range, this
	 * method returns NULL.
	 *
	 * @param index				the frame index
	 *
	 * @return					the corresponding key frame
	 */
	KeyFrame* getFrame(const int& index) const;

	/**
	 * Returns the last time key of this animation.
	 *
	 * @return					the timekey value of the last key frame in this animation
	 */
	float getLastTimeKey() const;

	/**
	 * Returns the first time key of this animation.
	 *
	 * @return				the timekey value of the first key frame in this animation
	 */
	float getFirstTimeKey() const;

	/**
	 * Changes the base type of all key frames to the specified type.
	 *
	 * @param b					the desired new base type for all key frames
	 */
	void setToBase(baseTp b);

	/**
	 * Returns a vector of floats with all the time keys of all the key frames
	 * in this animation. The time keys are chronologically sorted.
	 *
	 * @return					a float vector containing all the timekeys
	 *							of all the key frames in this animation
	 */
	std::vector<float> getAllTimeKeys();

	/**
	 * Adds a key frame to this animation at a certain time key and clears the cache.
	 * If the timekey is already present in the animation, this method does nothing.
	 *
	 * @param k					the key frame to be added to the animation
	 * @param timekey			the desired time offset for the key frame
	 */
	void addKeyFrame(KeyFrame* k, float timekey);

	/**
	 * This method clears the animation cache. This class keeps track of frames that
	 * were interpolated if necessary and places them in a cache (also for memory
	 * management reasons). After the cache is cleared, the interpolated frames are
	 * no longer accessible.
	 */
	void clearCache();

	/**
	 * Removes the time offset of this animation, so that the animation starts at
	 * timekey t=0.
	 */
	void removeTimeOffset();

	/**
	 * Fits this animation to a set of new keyframes.
	 *
	 * @param newKFs			the list of new keyframes that the animation should
	 *							go through
	 * @param frameNrs			the list of frame nrs in the animation corresponding to
	 *							the new key frames
	 */
	void retarget(std::vector<KeyFrame*>* newKFs, std::vector<int>* frameNrs);

	/**
	 * Fits an animation segment so that it starts or ends at a new key frame.
	 *
	 * @param frame0			the first frame of the animation segment to be fitted
	 * @param frame1			the last frame of the animation segment to be fitted
     * @param newkf				a pointer to the new KeyFrame
	 * @param offsetOfFrame1	if true; the animation will end at the new key frame;
	 *							otherwise it will begin at the new keyframe
	 */
	void retarget_part(int frame0, int frame1, KeyFrame* newkf, bool offsetOfFrame1, const bool& keepTrans = false);

	/**
	 * Applies a translation offset to all the keyframes in this animation. 
	 *
	 * @param t					the translation offset
	 */
	void applyTransOffset(const translation& t);

	/**
	 * Removes all the keyframes that have a timekey < the timekey of the keyframe
	 * coming just before the specified timekey.
	 *
	 * @param timekey		the specified timekey
	 */
	void removeKeyFramesBefore(const float& timekey);

	/**
	 * Removes all the keyframes that have a timekey > the specified timekey.
	 *
	 * @param timekey		the specified timekey
	 */
	void removeKeyFramesAfter(const float& timekey);

	/**
	 * Removes all the keyframes from this animation and clears the cache.
	 */
	void clear();

	/**
	 * Given a frame number, this method will return the corresponding timekey.
	 * If the frame number is out of range, this method returns -1.0f.
	 *
	 * @param framenr		the frame number
	 *
	 * @return				the corresponding timekey; -1.0 if out of range
	 */
	float getFrameTimeKey(const int& framenr) const;


private:

	/**
	 * vector containing pointers to all the keyframes
	 */
	std::vector<KeyFrame*>* keyframes_;

	/**
	 * vector containing all the timekey values in milliseconds as floats
	 */
	std::vector<float>* timekeys_;

	/**
	 * vector containing pointers to all the keyframes
	 */
	std::vector<KeyFrame*>* keyframes_cache_;

	/**
	 * vector containing all the timekey values in milliseconds as floats
	 */
	std::vector<float>* timekeys_cache_;

};

#endif
