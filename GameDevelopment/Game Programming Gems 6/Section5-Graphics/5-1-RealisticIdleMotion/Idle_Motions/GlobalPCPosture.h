/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef GlobalPCPosture_H
#define GlobalPCPosture_H

/**
 * the precision used to save the PC data in an XML file (if the absolute PC value
 * is smaller that GLOBALPCPRECISION, the the value is set to 0.0f)
 */
#define GLOBALPCPRECISION 0.00001

/**
 * This class represents a posture by a GLOBALPCSIZE size vector of Principal Component values (with
 * global translation of the root joint).
 */
class GlobalPCPosture
{
public:

	/**
	 * Constructor.
	 * Constructs a new GlobalPCPosture object with all global PC values set to 0.
	 */
	GlobalPCPosture(void);

	/**
	 * Constructor.
	 * Constructs a new GlobalPCPosture object with the PC values initialized to
	 * the values in the array given as parameter. This array has to be of size 228.
	 *
	 * @param pcarray			array of size 228 with pc values
	 */
	GlobalPCPosture(float* pcarray);

	/**
	 * Copy constructor.
	 */
	GlobalPCPosture(GlobalPCPosture* p);

	/**
	 * Destructor.
	 */
	~GlobalPCPosture(void);

	/**
	 * Returns the value of a Principal Compenent with the index given as parameter. The 
	 * parameter should be in the interval [0,77]. If not, this method will return 0.0f.
	 *
	 * @param index				the PC index in the interval [0,77]
	 *
	 * @return					the corresponding PC value; 0.0f if the index is out of range
	 */
	float getValue(const int& index);

	/**
	 * Changes the value of a Principal Component. The index parameter should be in the
	 * interval [0,77]. If not, this method will do nothing.
	 *
	 * @param index				the PC index in the interval [0,77] that has to be changed
	 * @param value				the new value of the PC
	 */
	void setValue(const int& index, const float& value);

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
	 * Calculates the Euclidian distance (in PC space) from this posture to 
	 * another one.
	 *
	 * @param				the second posture
	 *
	 * @return				the euclidian distance of p from this posture
	 */
	float distanceFrom(GlobalPCPosture* p);

private:

	/**
	 * array of size 78 containing all global PC values
	 */
	float* pcvalues_;

	/**
	 * the 'changed' flag
	 */
	bool changed_;
};

#endif
