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


#  include <stdio.h>
#  include <stdlib.h>
#  include <errno.h>
#  include <windows.h>
#  include <wingdi.h>

#  include <GL/gl.h>

void	*LoadDIBitmap(char *filename, BITMAPINFO **info);
int	SaveDIBitmap(char *filename, BITMAPINFO *info, void *bits);
void	*ReadDIBitmap(BITMAPINFO **info);
int	PrintDIBitmap(HWND owner, BITMAPINFO *info, void *bits);

GLubyte	*ConvertRGB(BITMAPINFO *info, void *bits);
