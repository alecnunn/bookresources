Unique Texturing example - Tom Forsyth, Muckyfoot Productions, 2002

Published as part of "Games Programming Gems 3"



To run the executable requires a PC, DX8 and a 3D card that can render in a window. To compile the code from scratch requires the DX8 SDK, and using Visual C 6 or better would be a good idea.



Use:
---

The landscape is rendered as if the camera were at the centre of it, so that is where the maximum mipmap detail is. The actual camera is free to move around to allow inspection of parts of the landscape.

Cursor keys - move viewpoint.
Shift - show texture sizes as colours.
Ctrl - drop a splat texture at the centre of the landscape.
Left mouse button - look around.
Right mouse button - zoom & rotate.
Both mouse buttons - orbit.
H - toggle help text on/off.





The Code
--------

Project file included for VC6, though you will need to link it to the DX8 framework yourself of course.

With a bit of playing with project files, you can probably get it to work under another compiler.



The main parts of interest:


* GemUnique.cpp(240) - CompositeImage(). This constructs a texture. It takes a list of vertices that influence the texture. Usually these vertices lie in the texture, but nearby vertices may have filters attached to them that draw onto the texture, so they need to be included as well. In the case of this landscape draw, no filter is ever bigger than the space between two vertices, and the filter is always attached to the nearest vertex, so this is not a problem. But it is something to be aware of for arbitrary meshes.

Essentially this merge-sorts the filters of the vertices together (they should already be sorted within each vertex), using their dwLayerNumber values as a sort key. Then it draws the merged list in order to the texture.


* GemUnique.cpp(878) - Render(). Composed of a few phases.

- Scan the existing textured chunks to see if any have changed mipmap level, gone out of view, or need updating because they have had a filter added (e.g. a splat). This frees up textures, ready for the updater to create them again. Limits the number of updates per frame to keep the framerate speed high even when moving quickly.

- Go through the chunks around the viewpoint updating and/or creating textures as necessary using Composite Image.

- Draw the actual polygons and text.

- Handle mouse and viewpoint moves.


* GemUnique.cpp(1456) - InitDeviceObjects() - does most of the initialisation. Has a simple midpoint-subdivision fractal landscape generator that may be useful to some people. Then goes through each vertex adding the correct filter type to each.

Note that chunks are really just a thing to hold the status of a texture. A general-mesh renderer has the same thing, with additional information to say how the tetxure is mapped to the mesh, and a list of vertices that can affect the texture. Here, most of that is implicit, because it is a regular 2D grid.


* Filter.cpp - comtains all the Render() code for each filter. Actually not very exciting reading, they just do various types of 2D alpha blending.


* Filter.h (199) - Filter constructor - shows how a filter is added to a vertex in a linked list, higher layers (=drawn last over other layers) listed first. The lists for each vertex that affects a texture are merged, and then all drawn in the merged order. The dwLayerNumber value should ideally be unique to each filter, otherwise you get sorting artifacts. Consider two splat filters over each other, and straddling a join between two textures. If they have the same dwLayerNumber number, their sorting w.r.t. each other may be different on each side, depending on which vertex is submitted first on each side.

This can be resolved by testing for equality and then sorting on, for example, vertex address pointers (since they are going to be the same for each side), but it is usually easier simply to ensure that each layer has a unique ID. A 32-bit number should provide more than enough namespace for this.

The different types of filter (in fact, all are simply alpha-blended textures rather than being conventional "filters") are then derived from the Filter base class, and each has its own Render() method.



Reasons Why This Demo Sucks:
---------------------------

It is pretty awful, but I have excuses for everything.

Q: Surely the compositing should be done use render-to-texture?

A: Absoloutely. But render-to-texture is a bit of a compatibility headache with older cards, i.e. it doesn't work well, it can BSOD, and there is no warning from the driver that things are going to fail, you just have to check DeviceID and magically know. Also, to get efficient render-to-texture, you need to batch renders together into large texture pages to avoid as many SetRenderTarget calls as possible. This is all far more work than I feel like doing for a demo. I do have code to do all of this, but only (a) a DX7 version (in StarTopia) or (b) an XBox-only version (in Blade 2). And both are embedded in the middle of commercial games and hard to extract. So I went for a 100% software compositor.

Q: What happened to all the cool scales and rotates and so on?

A: Yes, you get all those for free if using render-to-texture, and they should be used very agressively. But for a software compositor, it means writing a full software triangle rasteriser. I have written software rasterisers on far too many machines in my life, and I really don't feel like writing yet another one. So I decided to stick with a 2D-only one. Even so, the results are suprisingly nice.

Q: You call that "nice"?

A: That IS nice for coder art. That's the Mona Lisa of coder art. Think what it would look like with real art done by a real artist.

Q: The blending code is written in C. It even has for() loops to do each R,G,B component. That's terrible.

A: Well, yes it is. It should obviously be written in MMX, which has useful things like saturating adds, parallel multiplies, etc. That would speed it up enormously. However, if you then think that actually for a software compositor in a real game, you need a general triangle rasteriser, that will slow it down again. So it's hard to say whether things would actually go any faster.

In fact, I found that the MMX software rasterisers I have written on the PC in the past have been memory-bandwidth bound, rather than bound by CPU cycles, and the same may well apply here. So the inefficient code may not be slowing it down all that much.

Q: Wouldn't it be better to do this using quadtree texturing, as shown by Thatcher Ulrich at http://tulrich.com/geekstuff/tiling/index.html ?

A: Definately - far more efficient, and it looks prettier too (fewer and smaller mipmap-edge-clamp artifacts). I just didn't have time to do that for this demo.

Q: I thought you said Unique Texturing would work for arbitrary meshes.

A: Yes, it does. As much as possible, I have kept the Filter/FilterVertex system quite general. However, since the blending is all done with 2D operations rather than render-to-texture, it's rather hard to extend the whole thing as-is to arbitrary meshes. Using a proper triangle rasteriser (software or hardware) would very easily allow arbitrary meshes, since it is essentially based on vertices and where they are mapped on textures.

Q: There's some kind of wierd effect where the mipmaps aren't being updated when I move.

A: There is a primitive throttling system in. The program is only allowed to update a certain number of each size of texture each frame. If it tries to update any more than that, it leaves that texture until next frame. This helps keep the framerate steady, even when the viewpoint is moving around quickly, though it can cause mipmap errors. The real throttling system would allow n texels per frame to be updated, but for a quick system (that also illustrates itself at work), I simply allow only one of each texture size to be updated.

So if the framerate drops, and the viewpoint is moving, you may see some textures not being updated as early as they should, and incorrect mipmap levels will be shown. As soon as the viewpoint stops moving, the system catches up. In my opinion, showing an incorrect mipmap is far more preferable to having the framerate drop too far. Pressing Shift shows the mipmap levels, and if you press Shift and Ctrl, that tends to slow the framerate down nicely and you can see the mipmap levels "trailing" behind.

Q: You're using DrawPrimitiveUP and calling it with two triangles per go. Could you get any slower?

A: It would take a bit of effort to go slower, yes. But in fact the number of triangles drawn in this demo is tiny. A real game would use something like quadtree'd VIPM chunks of course. This is a texturing demo, not a triangle-rendering demo :-)

Q: What's with the smiley? (start the demo, scroll right a bit)

A: It just demonstrates that you can have a huge number of filters without much loss of performance. Doing this sort of rendering by conventional means such as decal polygons would murder both fillrate and poly throughput.

Q: Why is placing splats (with the Ctrl key) so slow?

A: Because each splat is being placed at the camera's position. Placing a splat here forces an update of the four highest-detail textures in the scene, which gets slow if you do it every frame. What a real game would probably do with dynamic decals such as bullet-holes and so on is to render them with conventional decal polygons when they first appear. As soon as they go off-screen, remove the decal polygons, make the splat into a filter and mark the texture as needing an update. This allows the texture to updated when it is convenient, or possibly even spread over a couple of frames. Additionally, because the texture is no longer visible, the update will usually be to a much smaller mipmap of the texture. And usually this will happen to multiple splats at once (bullet holes from automatic weapons, etc), with only a single texture update doing all of them.



Any comments, questions or just random greets to me at TomF@muckyfoot.com.

This is not quite the demo I hoped for, but it was just too evil to try to get a proper render-to-texture demo up and running in the time available. Getting render-to-texture up and running on most PCs without random BSODs at a sensible speed is probably a good subject for a Gem all on its own.

I think most of the swearing has been removed from the code. But apologies if any slipped through.



Tom Forsyth, 2002
TomF@muckyfoot.com




