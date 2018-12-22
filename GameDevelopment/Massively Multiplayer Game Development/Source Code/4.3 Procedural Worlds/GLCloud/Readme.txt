GLCloud readme

Keyboard commands for GLCloud:
keypad 2, 4, 6, 8 - turn camera
keypad 7, 9       - roll camera
keypad +, -       - adjust cloud cell size
w, s, a, d        - thrust forward, reverse, left, and right
Ctrl              - hold down for 100x thrust
spacebar          - full stop
m                 - toggle moving light source (rotates around cloud block)
r                 - toggle rotating cloud block (rotates in place)
i                 - toggle impostors
u                 - update cloud block (using new random seed)


Tips for running GLCloud:
1) GLCloud.exe runs all by itself without supporting files. However, it has
   only been tested on NVidia GeForce-class cards. It doesn't use any NVidia-
   specific extensions, but it still may not work on other cards.

2) The cloud block currently is a 3D block of 96x96x16 cells. That's 144K
   cells, which is enough to bring a number of systems to their knees. Keep
   in mind that they have to be lit as well as rendered, and lighting for
   clouds is 100% shadows. It would be ideal to split blocks like this into
   smaller blocks, perhaps with a dynamic LOD scheme.

3) Start by having fun flying through the volumetric clouds. If your system
   is not decent by 2002 standards, movement may be jerky. Try it with
   impostor rendering on and off. For some it may seem smoother with
   impostors turned off, despite the fact that the FPS is much higher with
   them turned on. In this case, each "jerk" occurs on a frame where the
   lighting has to be recalculated and the impostor updated. With smaller
   cloud blocks, this should not be an issue.

3) Next try backing away from the cloud block a little. Then press 'r' to
   start the cloud block rotating. Watch it rotate fully at least once,
   then press 'm' to watch the light rotate around a different axis. Note
   how the shadows crawl across the clouds. Press 'u' a few times to see
   different cloud blocks in this fashion.

4) Playing with the cloud cell size isn't very useful, as 3.0 seems to be
   the best setting for it. However, you may want to lower the cell size
   to 1.0 to give you a visual cue on how the cloud cells make up the block.
   (Note that larger cell sizes lead to more pixel overdraw, which runs slower.)

Tips for building GLCloud:
1) This project was written using Microsoft Visual C++ 6.0. You should be
   able to build it with older versions of MSVC++, or any other C++ compiler
   than compiles Windows programs, with a few changes. Other than standard
   Windows and C run-time libraries, the only libraries I link are
   opengl32.lib and glu32.lib.

2) Originally I tried to minimize the amount of Windows-specific code, and
   much of it should be contained within GameApp.cpp and GameEngine.cpp. It
   shouldn't be difficult to port this project to Linux.


Ideas for use:
Paired with a good dynamic LOD algorithm, this algorithm could be used to populate an entire planet with attractive volumetric clouds. With some modifications, it could be made to render gas giants as well. Of course, it may also be used for nebulae, fog, smoke, or anything else that requires semi-transparent volumetric rendering.

Keep in mind that to light the cloud cells, they must be sorted based on distance to the light source. Then to render them, they must be sorted based on distance to the camera. GLCloud uses a cheap hack to avoid sorting the block. Because the cells are in a fixed 3D grid, it is easy to find the most distant corner (or corners), and then use the triple-nested for loop, simply going along each axis in an order determined by the most distant corners.

Currently the individual cloud cells are drawn with the Z-buffer disabled. This is because they are essentially billboards whose edges touch, and the Z-buffer doesn't handle them properly. Interestingly enough, if the clouds are always impostored, it doesn't really matter because the Z-buffer should be disabled for rendering into the impostor (because the cloud cells are drawn in reverse Z order anyway).
