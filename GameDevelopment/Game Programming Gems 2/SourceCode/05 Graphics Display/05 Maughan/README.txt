Texture Masking Demo

20/3/2001


This demo shows the benefits to be had from the texture masking technique.  Briefly, the demo uses the graphics hardware to generate a texture map representing the intensity of a lens flare.  This texturemap is then modulated with the lens flare to attenuate it depending upon how occluded the sun is in the world.

The demo lets you switch between Texture Masking and Locking the frame buffer - the more traditional way of doing this effect.  Note that there is a substantial win (as much as 2x on my system) from using the texture masking technique.  The win is also improved the larger the size of the framebuffer.  Note also that the lock method and the texture masking technique do not necessarily set exactly the same occlusion, so the intensity of the flare may vary from one technique to the other.  They could be brought more closely together with effort, but they are fundamentally different techniques.

Note that on some systems it may not be possible to lock the buffer at all (this is certainly true if you run the demo with antialiasing turned on).  In this case the demo will detect it and switch back to Texture Masking - another compelling reason for using Texture Masking!

Because this method is designed to increase parallelism between the GPU and the CPU, it works best on hardware with built in Transform and Lighting.

Additional menu options let you see the intensity and sun maps, as well as disable the reads during the locking method.  With this option you can see the difference in performance hit between simply locking the framebuffer and locking and reading the framebuffer.

The sun map represents the visible sun pixels.  The intensity map represents the calculated intensity value.


Drive the demo like so:
Left Mouse + Mouse Move = Rotate viewer
CTRL + Left Mouse + Mouse Move = Walk viewer
SHIFT + Left Mouse + Mouse Move = Strafe viewer
Page Up = Increase CPU work
Page Down = Decrease CPU work

Note that as you move the lens flare should flicker in the correct way.

To analyse the cost of the lock on system parallelism, do the following:
1. Disable the read during the lock from the menu
2. Set the texture masking method.
3. Increase the CPU work until the framerate just starts to drop
4. Switch to the lock method.  Note the large drop in framerate as the CPU work is no longer serialized.

To analyse the cost of the whole lock method versus the Texture Masking method:
1. Ensure reads are on during lock.
2. Switch between the two methods.


Please let me know if you have any problems running or compiling the demo.

Regards,

Chris Maughan
NVIDIA Corporation
(cmaughan@nvidia.com)