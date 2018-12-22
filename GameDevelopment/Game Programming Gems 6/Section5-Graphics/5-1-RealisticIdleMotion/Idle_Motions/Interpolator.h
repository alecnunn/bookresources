/**
 * Author:   $Author: egges $
 * Modified: $Date $
 * Revision: $Revision $
 * Release:  $Name $
 */

#ifndef Interpolator_H
#define Interpolator_H


/**
 * type used to distinguish between the different interpolation types
 */
typedef enum {linear, cosine} interpolTp;

/**
 * This class defines different numerical interpolation methods.
 */
class Interpolator
{
public:

	/**
	 * 2D Interpolation between two points, according to a specified
	 * interpolation type.
	 *
	 * @param x1			x-value of point 1
	 * @param y1			y-value of point 1
	 * @param x2			x-value of point 2
	 * @param y2			y-value of point 2
	 * @param x				the x-value for which the y-value has to be
	 *						calculated
	 * @param i				interpolation type
	 *
	 * @return				the y-value corresponding to the given x-value
	 */
	static float twoPointInterpol(const float& x1, const float& y1, 
								  const float& x2, const float& y2,
								  const float& x,
								  const interpolTp& i = cosine);
};

#endif // ifndef Interpolator_H
