/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef PCMain_H
#define PCMain_H

#include <string>

class PCConvertor;

/**
 * This class initializes a static PC convertor object with the PCA data in the file
 * specified. 
 */
class PCMain
{
public:

	/**
	 * Initializes the pc convertor object with the PCA data in the files that are
	 * given as parameters.
	 *
	 * @param globalPCFile			the path of the file containing the global 
	 *								PCA data
	 */
	static void initialize(	const std::string& globalPCFile);
	
	/**
	 * Finalizes the pc convertor object and cleans up memory.
	 */
	static void finalize();

	/**
	 * the convertor object
	 */
	static PCConvertor* convertor;
};

#endif
