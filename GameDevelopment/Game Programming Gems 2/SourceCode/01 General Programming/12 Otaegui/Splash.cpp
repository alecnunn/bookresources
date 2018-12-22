/* Copyright (C) Javier F. Otaegui, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Javier F. Otaegui, 2001"
 */
int LoadSplashGraphics( lpvoid Params )
{
	Surface *pMySurface;
	pMySurface = (Surface *) Params;

	//  ...
	//  (load the graphic from the file)

	return 1;
}

int Splash()
{
	Surface MySurface;

	// Push the function
	gReloadSurfacesStack.Push( &LoadSplashGraphics, &MySurface );

	// Do not forget to load graphics for the first time
	LoadSplashGraphics( &MySurface );

	// ... the subroutine functionality.

	// Pop the function
	gReloadSurfaceStack.Pop();
}

