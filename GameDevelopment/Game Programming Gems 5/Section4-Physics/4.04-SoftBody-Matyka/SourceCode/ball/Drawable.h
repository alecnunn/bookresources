/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/



#ifndef __I_DRAWABLE_H_
#define __I_DRAWABLE_H_


/**
 * @short Property of some objects. It tells us that object can be drawed in OpenGL.
 *
 *
 * @detailed Contains one simple virtual procedure - glVisualize()
 * 
 * @author Maciej Matyka
 * http://panoramix.ift.uni.wroc.pl/~maq
 *
 */

class Drawable
{
public:

	Drawable(){}

	virtual void glVisualize(void) const { };	


};




#endif
