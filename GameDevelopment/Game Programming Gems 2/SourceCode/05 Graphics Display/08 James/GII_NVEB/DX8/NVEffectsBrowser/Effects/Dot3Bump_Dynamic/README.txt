
3/28/01
Dot3Bump_Dynamic

This example uses a few render-to-texture passes to animate a grayscale texture.  This texture is used as a height map to create an RGB normal map, again using a single render-to-texture pass and a DX8 Pixel Shader program v1.1.  This normal map is used in Dot-Product (or Dot3) bumpmapping to render a coarsely tesselated sphere lit by a directional light.

The animated RGB normal map is rendered in the background.

It is important to note that the CPU is not involved in writing to any texture surfaces.  All the animation is done in the pixel hardware using DX8 vertex and pixel shaders.  No textures are ever locked or updated by the CPU.  The endless non-repeating animation evolves continuously by rendering from one texture to the next.

The simulation which produces the "waves"  (hit ']' for a while, then '>' to smooth the wave tops!) is a height-based water physics simulation.  Other versions of this which animate smooth waves and water droplets on a surface will be posted to our developer website soon.  (or you could experiment with changing the "sub_x2" to a "sub" in neighborforcecalc2_x2 =)

** The demo's frame rate is limited by a delay ** 
** Hit 'S' to toggle the frame rate slowdown on and off, or the LEFT and RIGHT arrow keys to speed or slow the frame rate

More examples of render-to-texture techniques will soon be posted to our developer website.

Enjoy, and let us know if you run into any problems downloading or compiling the code.

-Greg J.
NVIDIA Developer Relations

