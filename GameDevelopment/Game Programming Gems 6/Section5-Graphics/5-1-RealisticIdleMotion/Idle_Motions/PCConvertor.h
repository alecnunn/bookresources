/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef PCConvertor_H
#define PCConvertor_H

#include <string>
#include "joints.h"
#include "Posture.h"

class GlobalPCPosture;

/**
 * This class is a convertor between global PC posture objects (1x78), 
 * and skew sym posture objects.
 */
class PCConvertor
{
public:
	
	/**
	 * Constructor.
	 * Constructs a PC Convertor object and reads the PC values from the
	 * Specified file. If the file contains errors, the pc matrix points to NULL.
	 *
	 * @param globalPCFile		the path of the file containing the global 
	 *							PCA data
	 */
	PCConvertor(	const std::string& globalPCFile);

	/**
	 * Destructor.
	 */
	~PCConvertor(void);

	/**
	 * Converts a global PC posture into a skew sym posture, consisting of a 
	 * root translation and 25 joints.
	 *
	 * @param g					the global PC posture
	 * @param p					will contain the skew sym representation
	 */
	void toPosture(GlobalPCPosture* g, Posture* p);

	/**
	 * Converts a skew sym posture, consisting of a root translation and 25 joints, 
	 * into a global PC posture.
	 *
	 * @param p			the skew sym posture object
	 * @param g			will contain the global PC representation
	 */
	void toGlobalPC(Posture* p, GlobalPCPosture* g);

	/**
	 * Returns TRUE if the global PC matrix points to NULL.
	 *
	 * @return			true if the global PC matrix points to NULL; false
	 *					otherwise
	 */
	bool globalPCMatrixIsNull();

private:

	/**
	 * a matrix containing the global pc values
	 */
	float** globalpcmatrix_;

	/**
	 * map used for determining order of joints 
	 * for using the global PC correctly
	 */
	std::vector<pcjoints::jointTp> globalJointMap_;

	/**
	 * Memory cleanup of the PC matric.
	 */
	void clearMemory();

	/**
	 * Reads a square matrix from a file and puts it in a matrix. The size has
	 * to be specified as a parameter.
	 *
	 * @param filename	the name of the file that contains the matrix data
	 * @param matrix	this will contain the matrix data
	 * @param size		the row/column length of the matrix
	 */
	void readSquareMatrix(const std::string& filename, float** matrix, int size);

};

#endif
