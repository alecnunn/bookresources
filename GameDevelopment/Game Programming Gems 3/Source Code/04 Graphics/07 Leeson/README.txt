This is the sample source code for Game Programming Gems III. The code implements 3 subdivision schemes (linear, Loop and butterfly). The code is written primarily using STL so as to make the code easier to understand and interpret. The code is not fully optimised due to time constraints (hey I only has a week or so to write all this :-P). There is threfore significant room for more optimisation. The pre-compiled array based optimisations have been implimented however the culling methods have not. Hopefully I will get to providing a implimentation of these shortly. There is code to load in a mesh from .off and .noff type files into an half-edge mesh data structure and various methods to opperate on the half-edge mesh. There is a simple implimentation of weighted skinning its a bit different from Woodlands method in GPG I but it's essentially the same.

The program when run uses the following keys
	'-' - decrease subdivision level  
	'=' - increase subdivision level
	'w' - wire frame mode
	'l' - solid fill mode
	's' - smooth shading
	'f' - flat shading
	'z' - move in
	'x' - move out
	'e' - switch between spherical environment map and normal texturing
	']' - increase timer increment
	'[' - decrease timer increment
	'<' - increase view fustrum size
	'>' - decrease view fustrun size
	'o' - toggle object display
	't' - toggle texturing
The program runs with the following arguments
	subdiv <datafile> <level>
where datafile is the OFF or NOFF file to be loaded and level is the number of level of subdivision surface to generate. Anything more that 8 levels is not recommended. The code should compile fine on both Windows and unix systems.

