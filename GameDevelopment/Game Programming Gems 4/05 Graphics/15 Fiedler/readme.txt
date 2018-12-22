Sample programs for "Terrain Occlusion Culling with Horizons"

In this archive you will find

1. A java applet demonstrating binary tree approximations of the horizon line
2. A proof of concept implementation of horizon culling with a tiled terrain

Source code for both these samples is provided. feel free to use this source 
however you like, but take note that both it and the executables come with no 
warranty! It is incomplete, it has bugs, and it is *definately* not suitable 
for production use!

To run the java applet simply open "LeastSquaresLine.html":

- use the slider at the bottom of the screen to adjust the error threshold
- right click in the client area to toggle between sloped and horizontal lines.
- left click to generate a new horizon line

To view the horizon culling implementation run the "Terrain.exe" application:

- mouse left click and drag rotates and zooms the camera
- mouse right click and drag right/left adjusts the error threshold
- F1 toggles rendering the quadtree nodes
- F2 toggles rendering the horizon
- F3 toggles rendering the top down visualization of rendered tiles
- F4 toggles rendering the terrain
- INSERT toggles wireframe mode
- DELETE toggles fps display
- Up/DOWN controls number of tiles rendered
- SPACE rebuilds the quadtree top-down
- ENTER rebuilds the quadtree bottom-up
- ESCAPE quits 


enjoy!

Glenn "Gaffer" Fiedler
http://www.gaffer.org
gaffer@gaffer.org
