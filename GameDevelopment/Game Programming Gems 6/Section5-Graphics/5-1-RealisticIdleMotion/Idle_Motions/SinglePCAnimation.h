/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef SinglePCAnimation_H
#define SinglePCAnimation_H

#include <vector>
#include "Interpolator.h"

/**
 * This class defines a key frame animation for a single Principal Component.
 */
class SinglePCAnimation
{
public:

	/**
	 * Constructor.
	 * Constructs an empty single PC animation for a specified pc number.
	 *
	 * @param pcnr			the pc number of the animation
	 */
	SinglePCAnimation(const int& pcnr);

	/**
	 * Destructor.
	 */
	~SinglePCAnimation(void);

	/**
	 * Returns the value of this PC at a specified time key. If the time key is not
	 * in the key frame list, the value is interpolated using the specified method.
	 *
	 * @param timekey		the time key of desired value
	 * @param i				the interpolation to be used (by default goniometric)
	 *
	 * @return				the corresponding PC value; 0.0 if the time key is
	 *						out of range
	 */
	float getValue(const float& timekey, const interpolTp& i = cosine);

	/**
	 * Appends a key frame to this animation. This method will only work if the
	 * time key is greater than the last time key in the animation.
	 *
	 * @param timekey		the timekey of the key frame that has to be appended
	 * @param value			the corresponding PC value
	 */
	void appendKeyFrame(const float& timekey, const float& value);

	/**
	 * Returns the last time key of this animation.
	 *
	 * @return				the timekey value of the last key frame in this animation
	 */
	float getLastTimeKey() const;

	/**
	 * Returns the first time key of this animation.
	 *
	 * @return				the timekey value of the first key frame in this animation
	 */
	float getFirstTimeKey() const;

	/**
	 * Returns the amount of key frames that are in this animation.
	 *
	 * @return				the amount of key frames in this animation
	 */
	int getFrameAmount();

	/**
	 * Returns the PC number of this animation.
	 *
	 * @return				the PC number of this animation
	 */
	int getPCNr();

	/**
	 * Removes all the keyframes that have a timekey < the timekey of the keyframe
	 * coming just before the specified timekey.
	 *
	 * @param timekey		the specified timekey
	 */
	void removeKeyFramesBefore(const float& timekey);

	/**
	 * Removes all keyframes from this animation.
	 */
	void clear();

private:

	/**
	 * the PC number corresponding to this animation
	 */
	int pcnr_;

	/**
	 * pointer to a vector of floating points defining the PC values of 
	 * all key frames
	 */
	std::vector<float>* pcvalues_;

	/**
	 * pointer to a vector of floating points defining the time keys of 
	 * all key frames
	 */
	std::vector<float>* timekeys_;

};

#endif
