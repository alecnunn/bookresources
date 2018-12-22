

This example renders objects with additive blending
for sort-independent transparency.  Various textures
can be loaded to simulate light scattering from a thin
transparent membrane or other effects.

With the first blueish texture which is loaded, the
object's faces brighten as they become more edge-on
to the viewer, which is a useful approach for CAD.
It's also similar to how a transparent jellyfish's
skin appears under bright light.

A vertex shader generates a texture coordinate based 
on the angle of the surface normal with the eye.  A
texture then translates this coordinate into a color
to use in rendering.  This allows edge-on faces to
appear bright while faces flat to the viewer appear
transparent.

The .grd files are gradient descriptions which you 
can load into Photoshop (I use 5.5) and modify.

Enjoy,
-Greg J.
NVIDIA Developer Relations

