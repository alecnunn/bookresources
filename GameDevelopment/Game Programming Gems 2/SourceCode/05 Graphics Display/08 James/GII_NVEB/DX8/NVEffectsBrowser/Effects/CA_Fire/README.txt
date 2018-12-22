
Procedural Texture Animation:  Fire & Smoke


This example uses render-to-texture operations and a
4-sample pixel program to simulate a fire and smoke
effect.  As the animation progresses, the texture render
targets on which the animation is occurring are rendered to
a quad which covers the screen.  One texture holds the 
previous frame's image and is used as a source texture
in rendering the next frame.  The other texture is the
destination for rendering, and will hold the next frame's
image.  These two textures switch roles after each frame,
so that the previous target becomes the new source, and
rendering ping-pongs back and forth between the two.

At each frame, a source texture of "embers" (here 
produced by the popular "Game of Life" cellular automata
program) is rendered into the scene additively.  By left-
dragging the mouse you can rotate the object that the embers
appear on as they are rendered.

After this, the scene render target is used as a source
texture.  It is blurred and scrolled upward to produce the
flowing smoke effect.  The amount and direction of scrolling
is jittered from frame to frame to help stir up the smoke.

The smoke trails emerge from blurring and scrolling each
texel of the image by a small amount each frame.  To see the
progression of blurring, use the following key sequence:

  G          - to stop the animation
  Home or 7  - to start the animation over
  Space      - to step from frame to frame

As each texel is blurred, its color is multiplied by a
constant color value.  Hit '[' or ']' to decrease and
increase the magnitude of this value, or 'C' to toggle
between two different color values.

The simple operation of rendering back and forth while
blurring produces an endless animation which does not
repeat.  Aside from the source ember animation, this
requires only two texture surfaces in video memory.
There is no CPU work performed on the texels of the
animated texture.

Please address any questions you have about this demo
to me here at NVIDIA, or general questions to
devrelfeedback@nvidia.com
or your Developer Relations contact.

I look forward to seing what other procedural texture
animations people come up with!

Thanks and enjoy!
-Greg J.
gjames@nvidia.com







