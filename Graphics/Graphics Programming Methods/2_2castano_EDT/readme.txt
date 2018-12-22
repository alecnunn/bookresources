
Filling Texture Holes Using the Euclidean Distance Map
------------------------------------------------------

Usage Guide:

Just enter 'EDT filename.png' on the command line or drop the image over the EDT.exe icon. EDT.exe will open four small windows, showing the original image, its 4SED transformation, its 8SED transoformation and its CSED transformation.

Guide to the code:

The interesting stuff is in the Scan.cpp file. The edt functions are implemented as members of the Image class and they are:

void Image::sed4( void );
void Image::sed8( void );
void Image::cuisenaire( void );
void Image::edt( void );

sed4 and sed8 are trivial implementations of the 4SED and 8SED algorithms respectively.

cuisenaire is an implementation of the CSED method as explained in: "Distance Transformations: Fast Algorithms And Applications To Medical Image Processing, O. Cuisenaire.". Note that the algorithm in his previous paper: "Fast and exact signed euclidean distance transformation with linear complexity" has some errors in the detection of voronoi corners.

The previous functions only compute the nearest feature-pixel for each empty pixel (hole). The edt method transforms the image according to that information and copies the nearest feature on each non-feature. This is done in a separate method, because the euclidean distance can be used for other purpouses, for example, to compute the greyscale edt, the skeleton or the voronoi regions.



Ignacio Castaño
castanyo@yahoo.es
