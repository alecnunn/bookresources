The demo shows a lens flare rendered on top of a 2D background scene.
The lens flare moves when you left-click and drag in the window.
Right click for a menu of options. Lens flares can be re-generated
randomly or loaded from flare.txt.

Demo Files:
-----------
flare.exe       - demo executable
glut32.dll      - Glut DLL required to run demo

flare.c         - flare and demo code
flare.h         - lens flare API
flare.dsp       - Visual C++ workspace
flare.dsw       - Visual C++ project

*.raw           - texture files
flare.txt       - file-based flare definition

Flare.txt format:
-----------------
scale maxsize
texture_name position size ( A, R, G, B )
texture_name position size ( A, R, G, B )
...

scale and maxsize determine over-all scaling of the flare.
texture_name matches the texture .raw files. position is
between 0 and 1 relative to the light position. size is
the size of the flare element, ARGB is it's colour and alpha.

.raw file format:
-----------------
Raw files store 32-bit ARGB textures. First two bytes of the
file are 16-bit texture width, stored little-endian. Next two
bytes are texture height. Remainder of file are 32-bit ARGB
values stored little-endian (i.e. BGRA byte order).

