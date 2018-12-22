
User Interface controls:
T	- Increase vertical tesselation
G	- Decrease "
H	- Increase horizontal tesselation
F	- Decrease "

. >	- Increase twist
, <	- Decrease twist
]	- Increase width
[	- Decrease width

R	- Reset object width, height, twist

W	- Wireframe

Arrows  - Move camera
Home	- Reset camera
PgUP	- Move camera up
PgDown	- Move camera down



This effect demonstrates the proper lighting of a ribbon object - a 
twisted 2D sheet of triangles with no volume.  It also blends between
two textures for the diffuse color of the object.  The first is a normal
texture and represents light reflected back to the camera.  The second
texture represents light transmitted through the thin object by lights
behind it.  In this way, we end up with an approximation to the optics
of thin scattering transmissive objects.  This technique can be used to
enhance the realism of thin objects like leaves, lampshades, or just 
about anything partialy transmissive.  

Under DX7 this single-surface twisted object would be cumbersome to
render.  You would first render the object with one set of normals.
Then you would have to modify all the vertices by negating the normals
and render a second pass to account for all normals that were
"backfacing" in the first pass.

With DX8 we can write a vertex shader program to perform this flip
of the normal as needed and render the object correctly in a single
pass.  The instructions required for this are short and simple, so
rendering a collection of ribbons, leaves, etc. in this way is very
attractive.

It would be interesting to render a more complex scene with this
technique!


-Greg J.
NVIDIA Developer Relations
11/29/00






