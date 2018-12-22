GLPlanet readme

Keyboard commands for GLPlanet:
keypad 2, 4, 6, 8 - turn camera
keypad 7, 9       - roll camera
keypad +, -       - adjust error threshold to increase/decrease detail
w, s, a, d        - thrust forward, reverse, left, and right
Ctrl              - hold down for 100x thrust
spacebar          - full stop
p                 - toggle polygon mode (fill/wireframe)
t                 - toggle texture maps (on/off)
i                 - toggle mesh & planet info (on/off)
u                 - toggle mesh updates (on/off)
1-9 (non-keypad)  - jumps the camera close to the nth planet

Tips for running GLPlanet:
1) For starters, GLPlanet has almost exclusively been tested on NVidia GeForce-
   class cards. While it should run on other video cards, it may not run very
   well. It uses NVidia's hardware vertex buffer extensions if they're supported,
   which makes it run an order of magnitude faster on NVidia cards.

2) The ss.cfg file contains the star system configuration. Edit this to
   add/modify planets and moons. GLPlanet.exe looks for ss.cfg in the
   application's starting path, or the current working directory, when it is
   launched. It will not start if it cannot find that file.

3) The default ss.cfg creates a star system similar to our inner solar system.
   It contains 4 planets named Mercury, Venus, Earth, and Mars (in that
   order). The Earth-like planet has a moon. The camera is initially
   positioned in front of the third planet, with the moon off to the right.

4) The thrusters in GLPlanet cause simple acceleration, as they would in
   space. In otherwords, the camera will not slow down on its own. To make
   navigation easier, the space bar instantly stops the camera.

5) When you first start GLPlanet, you will be far enough away from the Earth-
   like planet and moon that normal thrusters will take too long to get there.
   Hold down the Ctrl key while flying around the planets, then release it
   when flying over the surface of a planet.

6) At the default error threshold (0.0015), the Earth-like planet has a
   manageable number of triangles, but the moon (due to the roughness of its
   surface) jumps way up near the max of 128K triangles, causing things to
   run a little slow. At any time, you may use the + or - keys (on the
   keypad) to change the error threshold. Keep in mind that impostored
   planets will not be updated until the camera moves enough to require
   them to be updated.

7) One interesting thing to do is to fly close to a planet, stop the camera
   at a viewpoint you like, then press 'u' to stop the ROAM mesh updates.
   The change in FPS indicates how much time the ROAM priority checks are
   taking. Then press 'p' to switch to wireframe mode and fly around the
   planet. This will show you how well the triangles are optimized for the
   viewpoint you picked.


Tips for building GLPlanet:
1) This project was written using Microsoft Visual C++ 6.0. You should be
   able to build it with older versions of MSVC++, or any other C++ compiler
   than compiles Windows programs, with a few changes. Other than standard
   Windows and C run-time libraries, the only libraries I link are
   opengl32.lib, glu32.lib, and dinput.lib.

2) Originally I tried to minimize the amount of Windows-specific code, but
   the use of certain WGL extensions made it difficult to contain. It
   shouldn't be difficult to port the key pieces of this software to Linux,
   but there are a number of supporting files that may be a pain to port.

3) The MSVC++ 6.0 project file has many of the key source files grouped by
   type for convenience. Here is a brief description of the files and the
   groups they are in.

ROAM Files
* ROAMTriangle.cpp and h - Encapsulates a ROAM triangle
* ROAMDiamond.cpp and h - Encapsulates a ROAM diamond
* ROAMSphere.cpp and h - Encapsulates a ROAM sphere, which manages the triangle and diamond lists, as well as the ROAM priority checks and mesh updates

Planetary Map Files
* PlanetaryMap.cpp and h - A base class for all other planetary maps
* HeightMap.cpp and h - A planetary map class representing a planet's height map (calls the fractal function and crater tree to retrieve height values)
* SurfaceMap.cpp and h - A planetary map class representing a planet's surface texture maps (loads the 1D textures from ss.cfg and pre-generates the global texture maps)

Solar System Files
* PlanetInfo.cpp and h - A utility class used for loading entries from ss.cfg
* Star.cpp and h - A star object, which is currently rendered with a billboarded glow map
* Planet.cpp and h - Brings together all of the ROAM and PlanetaryMap classes to encapsulate a planet or moon
* StarSystem.cpp and h - Organizes the star system, handling impostor updates and rendering

Supporting Files (not in a special group)
* WndClass.h - Contains some of my own home-made MFC-like wrappers around part of the Win32 API.
* ListClasses.h and ListTemplates.h - Contains some of my own home-made container classes. I tried using STL at one point (the MSVC++ implementation), but the performance was an order of magnitude slower, so I kept these classes around.
* Noise.cpp and h - Contains Perlin noise and fBm-based functions.
* Matrix.cpp and h - Contains Vector, Matrix, Quaternion, and 3DObject classes.
* PixelBuffer.cpp and h - Contains a pixel buffer class for procedural generation of textures.
* Texture.cpp and h - Contains a simple texture object class.
* Font.cpp and h - Contains a simple font class (uses wglUseFontBitmaps).
* Billboard.cpp and h - Contains a simple billboard class to handle billboard rendering.
* Impostor.cpp and h - Contains an impostor class that represents the bulk of the impostoring code.
* GLUtil.cpp and h - Handles the initialization of OpenGL extensions and rendering contexts.
* DXUtil.cpp and h - Handled the initialization of DirectInput.
* GameApp.cpp and h - Handles the startup of GLPlanet and the Windows-specific initialization.
* GameEngine.cpp and h - Contains the top-level rendering and input-handling functions.
* Master.cpp and h - The pre-compiled header.
