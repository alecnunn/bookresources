/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef KeyFrame_H
#define KeyFrame_H

#include <string>

class Posture;
class PCConvertor;
class GlobalPCPosture;

typedef enum {skew,globalpc,unknown} baseTp;

/**
 * This class defines a generic key frame, that can have different representations,
 * notably skew sym and global Principal Components.
 */
class KeyFrame
{
public:

	/**
	 * Constructor.
	 * Constructs a key frame with NO DATA inside. You can set it to the neutral position
	 * by calling the neutral() method.
	 */
	KeyFrame();

	/**
	 * Constructor.
	 * Constructs a key frame according to the data in the skew sym posture.
	 *
	 * @param p						the skew sym posture
	 */
	KeyFrame(Posture* p);
	
	/**
	 * Constructor.
	 * Constructs a key frame according to the data in the global PC posture.
	 *
	 * @param p						the global PC posture
	 */
	KeyFrame(GlobalPCPosture* p);

	/**
	 * Copy constructor.
	 */
	KeyFrame(KeyFrame* k);

	/**
	 * Destructor.
	 */
	~KeyFrame();

	/**
	 * Returns the keyframe data in this object, represented by global PCs.
	 *
	 * @return						a reference to a global PC posture
	 */
	GlobalPCPosture* getGlobalPCPosture();

	/**
	 * Returns the keyframe data in this object, represented by skew sym rotations.
	 *
	 * @return						a reference to a skew sym posture
	 */
	Posture* getPosture();

	/**
	 * If any changes are made to any of the object representations, this method
	 * updates the changes in the other representations. This method can only handle one
	 * changed object!
	 * Normally, it shouldn't be necessary to call this method, since it is already 
	 * automatically called within other methods of this class if needed.
	 */
	void updateChanges();

	/**
	 * Sets the basic representation type of this object. 
	 *
	 * @param b						the basic representation type of this keyframe
	 */
	void setBaseType(baseTp b);

	/**
	 * Returns what the basic representation type is of this keyframe (skewsym or globalpc).
	 *
	 * @return						the basic representation type of this keyframe
	 */
	baseTp getBaseType();

	/**
	 * Sets this keyframe to the neutral (HAnim) posture .
	 */
	void neutral();

	/**
	 * Copies the data of k into this key frame.
	 *
	 * @param k						the key frame containing the data to be copied
	 */
	void copy(KeyFrame* k);

private:

	/**
	 * the skew sym posture representation
	 */
	Posture* posture_;

	/**
	 * the global PC posture representation
	 */
	GlobalPCPosture* globalPCs_;
	
	/**
	 * the basic representation type of this keyframe
	 */
	baseTp base_;

};

#endif
