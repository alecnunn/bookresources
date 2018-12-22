/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef Posture_H
#define Posture_H

#include "joints.h"
#include <map>
#include <vector>

/**
 * struct to define a skew-symmetric representation of rotation
 */
typedef struct {
	float a,b,c;
} skewSymTp;

/**
 * struct to define a translation
 */
typedef struct {
	float x,y,z;
} translation;


/**
 * This class represents a posture by translation + rotation (skew sym) values for body
 * joints of type jointTp.
 */
class Posture
{
public:

	/**
	 * Constructor.
	 * Creates a new Posture object.
	 */
	Posture(void);

	/**
	 * Copy constructor.
	 */
	Posture(Posture* p);

	/**
	 * Destructor.
	 */
	~Posture(void);

	/**
	 * Gets the translation value of a joint and saves it in a translation object. If the
	 * joint cannot be found, no information is written in the translation object.
	 *
	 * @param joint			the joint of which the translation value should be obtained
	 * @param trans			the translation object
	 */
	void getTranslation(const pcjoints::jointTp& joint, translation& trans) const;

	/**
	 * Sets the translation value of a joint to a value specified in a translation object. 
	 * Subsequently, the 'changed' flag is set. If the joint cannot be found, this 
	 * method does nothing.
	 *
	 * @param joint			the joint of which the translation value should be changed
	 * @param trans			the translation object with the new values
	 */
	void setTranslation(const pcjoints::jointTp& joint, translation& trans);

	/**
	 * Gets the rotation value of a joint and saves it in a skewSymTp object. If the
	 * joint cannot be found, no information is written in the object.
	 *
	 * @param joint			the joint of which the rotation value should be obtained
	 * @param r				the skewSymTp object
	 */
	void getRotation(const pcjoints::jointTp& joint, skewSymTp& r) const;

	/**
	 * Sets the rotation value of a joint to a value specified in a rotation object.
	 * Subsequently, the 'changed' flag is set. If the joint cannot be found, this 
	 * method does nothing.
	 *
	 * @param joint			the joint of which the rotation value should be changed
	 * @param r				the rotation object with the new values
	 */
	void setRotation(const pcjoints::jointTp& joint, skewSymTp& r);

	/**
	 * Returns a list of all the joint names that have an active translation in this posture
	 * object.
	 *
	 * @return				a vector containing all the joint names that have an active
	 *						translation
	 */
	void getActiveTranslations(std::vector<pcjoints::jointTp>& v);

	/**
	 * Returns a list of all the joint names that have an active rotation in this posture
	 * object.
	 *
	 * @return				a vector containing all the joint names that have an active
	 *						rotation
	 */
	void getActiveRotations(std::vector<pcjoints::jointTp>& v);


	/**
	 * This method indicates whether the 'changed' flag is set. The 'changed 'flag is set 
	 * automatically if a value is changed after initialization of the object.
	 *
	 * @return				the status of the 'changed' flag
	 */
	bool changed();

	/**
	 * Resets the 'changed' flag to 'false'.
	 */
	void resetChanged();

	/**
	 * Sets this posture to the neutral position.
	 */
	void setNeutral();

	/**
	 * Returns a linearly interpolated value between two translations. location=0 returns trans1,
	 * location=1 returns trans2.
	 *
	 * @param trans1		the first translation
	 * @param trans2		the second translation
	 * @param location		desired location [0,1]
	 */
	static translation	interpolateTrans(	const translation& trans1,
											const translation& trans2,
											const float& location);

private:

	/**
	 * the translation map
	 */
	std::map<pcjoints::jointTp,translation> transMap_;

	/**
	 * the rotation map
	 */
	std::map<pcjoints::jointTp,skewSymTp> rotMap_;

	/**
	 * the 'changed' flag
	 */
	bool changed_;


};

#endif
