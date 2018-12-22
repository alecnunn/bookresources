Fresnel Demo
============

Explanation
-----------
Fresnel reflections are part of a lighting model that is not included in the standard hardware T&L pipeline.  Some objects, such as plastic or glass, have reflections that are much stronger when viewed perpendicular to the normal than when they are viewed straight on.  This vertex shader visualizes the "Fresnel factor" by setting the color equal to one minus the dot product of the normal and view direction.  It is then squared to enhance the edges even more.  Instead of being directly visualized, a vertex/pixel shader will typically use this factor to bias the amount of specular reflection it uses to calculate the color.  To make the Fresnel factor even more accurate per pixel, the vertex shader calculates the view direction and normal and outputs these as texture coordinates, which the graphics pipeline will interpolate per pixel and hand off to the pixel shader to finish the calculation. (Based on an article by Chris Brennan from the ShaderX book).

A problem that I ran into with this shader is that DX shaders clamp pixel shader inputs to [0,1.0] whereas OpenGL will allow negative inputs.  Thus, all vectors that get sent into the pixel shader need to be munged into the [0,1.0] range and then back again.

Change from CgToWm3
-------------------
None.