

This example uses the pixel processing of the GeForce3 
to run a height-based water simulation on the graphics
processor.  [Gomez'00] discusses the physics involved
in the context of running the simulation on a CPU,
but here, we perform the nearest-neighbor differencing
using DX8 vertex and pixel shaders on the GPU.

The physical state of the water is stored in three textures:
one for height, one for velocity, and one for force.  Each
texel represents the state of one point on the heigh-based
water grid.  These textures are alternately used as source
textures or render targets in rendering, so that the previous
frame's textures generate those for the next frame.

The previous frame's height generates a force texture, where
each pixel represents the rate of change of velocity at that
point.  This force texture is applied to the previous velocity
texture, with a uniform scale factor for momentum.  The result
is a new velocity texture which is applied to the previous
position to generate the position in the next frame.

In this way, a group of six (it could be five) textures
generates an endless animation of rippling water.

A DX8 pixel shader is also used to calculate an RGB normal map
from the grayscale height texture values.  This is used for
the true reflective bump mapping which produces the environment
reflection off of the rippling surface.  Our website has many
examples using normal maps and material about them.  You can 
display the RGB normal map and height textures by hitting '5'.
(Hit 'F1' or 'H' for additional keyboard controls - You can 
also control the physical parameters of the simulation)

This conversion of grayscale texture to RGB normal map can
be done in a single pass over the texture on a GeForce3.
This is great for rendering features into your environment, 
such as bullet holes on walls or animating geometry expressed
with dot-product bump mapping.

Other examples will follow, and don't hesitate to write to
devrelfeedback@nvidia.com with your comments or questions.


-Greg James
NVIDIA Technical Developer Relations


// *********************************************** //
References:

Gomez, Miguel, "Interactive Simulation of Water Surfaces"
  Game Programming Gems, ed. Mark A. DeLoura, Charles River Media,
  Inc., 2000, pp.187-199   ISBN: 1-58450-049-2

