

This demo uses the same height-based water simulation
code as the "ReflectiveBumpDynamic" or "True-Reflective
Bump Mapping" example.  For that example, the water
height is always a grayscale value.  All color channels
hold the same values, so they behave in unison.
In this case the RGB values of the water height are
different, so waves evolve independently in the red,
green, and blue channels.  No additional work over that
done for the grayscale water is necessary to achieve
the independent colors.

Also, the restoring force pulling all values to gray
is set to zero, causing most of the image to fall
to black.  To increase or decrease the force pulling
everything to gray, use the "X" or "Z" keys, respectively.

A GUI for the properties of the water system will be
available sometime in the future.  For now, hit "F1" 
or "H" for a list of keyboard controls.

Left-click and drag to draw a brush texture into the
image.

See the README for the "ReflectiveBumpDynamic" example
for an explanation of the water simulation.

All calculations for the RGB waves are done entirely
in the pixel hardware of the GeForce3.  The CPU
submits only single large quads to cover the rendering
surface.


-Greg James
NVIDIA Technical Developer Relations


References:

Gomez, Miguel, "Interactive Simulation of Water Surfaces"
  Game Programming Gems, ed. Mark A. DeLoura, Charles River Media,
  Inc., 2000, pp.187-199   ISBN: 1-58450-049-2

