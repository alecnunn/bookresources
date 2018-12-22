/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef Joints_H
#define Joints_H

#include <string>
#include <vector>

#define JOINTAMOUNT_ROTATION		95
#define JOINTAMOUNT_TRANSLATION		1

/**
 * definition of the amount of global pc joints
 */
#define GLOBALPCJOINTS	25
#define GLOBALPCSIZE	GLOBALPCJOINTS*3 + 3

/**
 * A set of definitions and functions for joint types and conversions. Also includes a
 * tool for defining the order of the joints with respect to a Principal Component matrix
 */
namespace pcjoints {



	/**
	 * enumeration of human body joints
	 */
	typedef enum {	unknown_joint, 
					l_hip, 
					l_knee, 
					l_ankle, 
					l_subtalar, 
					r_hip, 
					r_knee, 
					r_ankle, 
					r_subtalar, 
					vl5,
					vl3,
					vl1,
					vt10,
					vt1,
					vc4,
					vc2,
					skullbase,
					l_sternoclavicular,
					l_shoulder,
					l_elbow,
					l_wrist,
					r_sternoclavicular,
					r_shoulder,
					r_elbow,
					r_wrist,
					root,
					sacroiliac,	// other joints that are not used in the PC representation
					l_midtarsal,
					l_metatarsal,
					r_midtarsal,
					r_metatarsal,
					vl4,
					vl2,
					vt12, vt11, vt9, vt8, vt7, vt6, vt5, vt4, vt3, vt2,
					vc7, vc6, vc5, vc3, vc1,
					l_eyelid_joint,
					r_eyelid_joint,
					l_eyeball_joint,
					r_eyeball_joint,
					l_eyebrow_joint,
					r_eyebrow_joint,
					temporomandibular,
					l_acromioclavicular,
					r_acromioclavicular,
					l_thumb1, l_thumb2, l_thumb3,
					l_index0, l_index1, l_index2, l_index3,
					l_middle0, l_middle1, l_middle2, l_middle3,
					l_ring0, l_ring1, l_ring2, l_ring3,
					l_pinky0, l_pinky1, l_pinky2, l_pinky3,
					r_thumb1, r_thumb2, r_thumb3,
					r_index0, r_index1, r_index2, r_index3,
					r_middle0, r_middle1, r_middle2, r_middle3,
					r_ring0, r_ring1, r_ring2, r_ring3,
					r_pinky0, r_pinky1, r_pinky2, r_pinky3
					} jointTp;

	/**
	 * Given a string description, this method returns the corresponding
	 * joint type name. If the string is not recognised, this method returns the
	 * 'unknown_joint' joint type.
	 *
	 * @param jointName				string representation of the joint name
	 *
	 * @return						the corresponding joint type; 'unknown_joint'
	 *								if the string is not recognised
	 */
	jointTp getJointType(const std::string& jointName);

	/**
	 * Given a joint type, this method returns the corresponding
	 * string description. If the string is not recognised, this method returns a
	 * string containing "unknown_joint".
	 *
	 * @param joint					the joint type
	 *
	 * @return						the corresponding string representation;
	 *								"unknown_joint" if the joint type is not recognised
	 */
	std::string getJointString(const jointTp& joint);

 	/**
	 * Initialises a vector of joint types with the order of the global PCs.
	 *
	 * @param jointMap				the vector to be filled with the data
	 */
	void initJointMap_global(std::vector<jointTp>& jointMap);

	/**
	 * Given a jointmap, and a joint type, this method
	 * returns the corresponding index of this joint in the vector that is 
	 * multiplied with the PC matrix.
	 *
	 * @param jointMap				the joint map
	 * @param joint					the joint identifier
	 *
	 * @return						the corresponding joint in the PC vector
	 */
	int getPCIndex(const std::vector<jointTp>& jointMap, const jointTp& joint);

	/**
	 * Given a joint map and an index in the PC vector ordering, 
	 * this method returns the corresponding joint name.
	 *
	 * @param jointMap				the joint map
	 * @param pcIndex				index in the PC vector ordering
	 *
	 * @return						the corresponding joint identifier
	 */
	jointTp getJoint(const std::vector<jointTp>& jointMap, const int& pcIndex);

	/**
	 * Returns a number according to pcjoints order given a joint type. If the joint
	 * type is unknown, this method returns -1.
	 *
	 * @param joint					the joint name
	 *
	 * @return						corresponding integer number
	 */
	int getJointNumber(const jointTp& joint);

	/**
	 * Returns the joint name given a number. If the number is out of range, this
	 * method returns 'unknown_joint'.
	 *
	 * @param nr					the joint number
	 * 
	 * @return						the corresponding joint name
	 */
	jointTp getJointFromNumber(const int& nr);
}

#endif