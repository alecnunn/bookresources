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
