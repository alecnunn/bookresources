VIPM method comparison tool - Tom Forsyth, Muckyfoot Productions, 2001

Published as part of "Games Programming Gems 2"



To run the executable requires a PC, DX8 and a 3D card that can render in a window. To compile the code from scratch requires the DX8 SDK, and using Visual C 6 would be a good idea.



Caveat coder:
------------

This tool is pretty basic, but shows what I wanted to show. I cannot emphasise strongly enough that bits of this are NOT meant to be copied - they are nowhere near robust enough for real-world cases, and don't deal with real-world stuff. I'll point out any bits that ARE decent. If in doubt about a bit of code, assume I just hacked it in so it was good enough for this demo (I have a real job to do as well).


Use:
---


You start in the collapse creation mode with the red & yellow wireframe of the object. When loaded, the object has no collapses, so you need to create some.

Viewing controls:

-LMB = pitch & yaw camera

-RMB = roll camera & move forwards/backwards.

-both MBs = move sideways & up and down - camera has a sort of "orbit" mode that tries to keep looking at the origin. Doesn't work all that well.

-If you hold Control, you control the object rather than the camera.



Two edges are highlighted with pointers on the model. The blue one is the edge that the QEM rout thinks is the best one to do next. The "thick end" is at the vertex that will be binned, and the pointer points at the vertex that will be kept. The green pointer is the edge that the cursor is closest to. Actually, it finds the closest vertex to the pointer, then finds the nearest edge that goes from it.


Collapse creation:

-Alt+C or Shift+RMB to collapse the QEM's choice of edge (blue). The QEM will never chose to collapse a boundary or seam edge.

-Shift+LMB to collapse the chosen (green) edge. You can manually collapse boundary or seam edges.

-Del/Backspace to delete the last collapse.

-9/0 to scan through the existing collapses. Hold shift to move by ten. Hold ctrl to go to first or last.

-Alt+W toggles whether the sliding window info is displayed and also whether the QEM tries to avoid making collapses that create a new sliding window level.

-[/] change the allowed error in sliding-window edge selection before a new level is forced (ignored if sliding-window mode is off).

-Alt+Shift+C does all the recommended collapses, one collapse per frame. This is just like holding down Alt+C, only less tedious. Any of the other collapse-control keys will stop the process (Alt+C, Del, LoD control keys, etc).


When sliding-window mode is on, you can see the difference between the selected edge collapse error and the best edge collapse error. Also, triangles already affected by collapses are shown in darker red. You cannot perform collapses that would affect these triangles without starting a new level (at which point everything becomes available and bright red again). If the green and blue edges are on edges that would force a new level, they are drawn in a slightly darker shade.

Note that you are perfectly free to auto-collapse to a certain point, make a manual collapse or two (including boundary or seam collapses), then keep auto-collapsing. There is nothing special about auto-collapses apart from the selection of the edge. This is actually a good way to go about authoring - use Alt+Shift+C, but with an artist watching. If the QEM does a silly collapse, or the artist decides that it would be an appropriate time to do a special collapse (seam or boundary), they stop the process, make the edit, and continue automatically. What is currently missing is a way to allow the artist to tag an edge so it is not auto-collapsed - if the artist decides the edge is more important than the QEM thinks it is. In this way, perceptually important things like faces can be preserved til later in the collapse order. It can also be used to deal with *ahem* "features" in the QEM routine, in the incredibly unlikely event that it delivers less than perfect results (in the eyes of artists, all auto-collapse routines suck, they just suck to different extents - frequently in direct proportion to the amount of time you have spent coding them).




Once the collapses are constructed, the object can be viewed in a slightly (though only slightly) more realistic setting, and using the various VIPM methods. This allows us to compare the various methods. Toggle between collapse creation mode and VIPM viewing mode using Alt+M. In viewing mode, the same viewing controls and LoD-selection controls work. One difference is that you can ask the code to select the LoD automatically based on distance from the camera - this is toggled on and off by Alt+A. When it is on, using 9 and 0 (+shift = faster) changes a global factor rather than directly controlling the number of collapses.

Alt+V cycles through the various VIPM methods. Alt+I toggles information about the selected VIPM method on and off - things like memory use, vertex cache misses and so on. Note that turning information on means that the index buffers need to be created so that they are CPU-readable, which may hurt performance on some systems (though at the moment nothing actually puts IBs in AGP memory - maybe in the future).

The slowest part about "compiling" the different VIPM types is the vertex-cache optimisation. The routine I use is fairly rough, but it mostly works, and gets fairly good results. Feel free to turn it up or down according to your speed wishes - it can take a seriously long time if you turn it up too much. By default the "cheap and cheerful" mode is on, which is basically just a stripifier, and doesn't give wonderful results, but it does work quite quickly. You can turn on the slow mode from the menus, but it is quite slow, so make yourself a cup of coffee or something.





The Code
--------

Project file included for VC6, though you will need to link it to the DX8 framework yourself of course.

With a bit of playing with project files, you can probably get it to work under another compiler. And most of the actual VIPM code (object.h and object.cpp) should be fairly straightforward to port to another API or platform.



Three main sections:

First there are my libraries in MyInc\. I wasn't going to write code without them, and I guess I can't stop you using them, and I don't really mind all that much if you do, but a credit and/or thankyou would certainly be appreciated if you do use any of it.

Next is the main Window and UI application gubbins that you need in gemvipm.cpp. Most of it is tedious UI stuff, and even the rendering routs aren't interesting (they're actually in object.cpp).

And the interesting stuff is in object.h and object.cpp. Major classes:

-MeshPt, MeshTri, MeshEdge. Actually defined in mesh.h in the library. Very general structures for dealing with mesh topologies. Does (mostly) handle non-manifold meshes, but not actually very elegantly. Note the kludge of #defining MESH*_APP_DEFINED before inclding mesh.h, which specifies a class to include inside each Mesh* object. It's not pretty in the definition, but actually it's the easiest and quickest way to extend the mesh stuff. I could muck around with inheriting the classes, but why bother? This works fine.

-GeneralCollapseInfo. Holds the definition of an edge collapse in a very general sense. Since tris and edges get deleted and created when performing collapses, this only references vertices (pts) to define its triangles.

-Quad. A very simple implementation of the basic Garland-Heckbert Quadric Error Metric. Use it show you how simple QEM actually is, but then go and read the original paper and Hoppe's modified version for something you'd actually use. This is only as complex as is required to generate sensible data. It doesn't necessarily produce _good_ data.

-Object. Describes an object. One instance per type (i.e. teapot, dog, etc) of object. None of it is fast, none of it would appear in a game - it's all concerned with constructing the collapse order. Owns all the Mesh* stuff and the GeneralCollapseInfo data.

-ObjectInstance. Describes an instance of the object on-screen. Each Object may have multiple ObjectInstances that it reference it. Basically just holds a position and orientation, and also the list of different VIPM type instances.

-OptimisedMesh. A virtual class that describes the general interface to some sort of optimised mesh description. One per VIPM type per object. Each VIPM type has its own class that is derived from this. The Object class owns one of these (or rather the derived version) for each VIPM type. This holds the global data for the VIPM type.

-OptimisedMeshInstance. An instance of OptimisedMesh. Again, each VIPM type has its own type derived from this. ObjectInstance owns one of these (or rather the derived version) for each VIPM type. Each derived version of OptimisedMeshInstance refers to an associated derived version of OptimisedMesh, one per instance. This holds the instance-specific data for the VIPM type.

-Lots of objects derived from OptimisedMesh and OptimisedMeshInstance. These are not declared in object.h, because the outside world doesn't need to know about them - it just uses the OptimisedMesh and OptimisedMeshInstance interfaces. They are all named OM* and OMI* respectively, and they have various collapse records and suchlike. The sets are:

-OMVanilla, OMIVanilla, VanillaCollapseRecord. Vanilla VIPM.
-OMSlidingWindow, OMISlidingWindow. Sliding window VIPM.
-OMSkiplist, OMISkiplist. Skiplist VIPM.

The factory for the OM* class is the static method OptimisedMesh::Create(), which creates the correct OM* version. Then from that you call CreateInstance(), which is the factory for the associated OMI* class.


The heart of these OM* and OMI* classes are:

-the OptimisedMesh::Update() method which converts from the GeneralCollapseInfo to the VIPM-specifc form. This would not be in a shipped game, just a preprocessor, and this data would be saved to disk. Not fast.

-the OptimisedMeshInstance::Update() method which copies the required per-instance data. This would happen in the game whenever a new object instance was created. Moderately fast.

-the OptimisedMeshInstance::RenderCurrentObject() method which performs any collapses or splits needed and renders the object instance. Fast.

-and most methods have their internal DoCollapse() and UndoCollapse() routines that are called by RenderCurrentObject. Not all - sliding window doesn't, because it doesn't do any work when collapsing or splitting - it's all done via a lookup table in RenderCurrentObject. Fast.




A real game doesn't have to worry about generating global data - it just loads it off disk. It also doesn't need to worry about each instance having multiple VIPM types hanging off it - generally each object type will only be rendered using one VIPM method. So the above can be considerably simplified. I have tried to keep the core routines (marked "fast") fast, since they would be in a real game, but the rest are not because they will just be in tools and preprocessors.



Note that I wrote about four main methods in the text of the Gem:

-Vanilla.
-Multi-level Skipstrip.
-Mixed Mode.
-Sliding Window.

Of these, Skipstrip has been implemented as a Skiplist. The only reason for this was that the rest of the code is all in lists, and I have a list optimiser. I didn't really want to write a good stripifier and degenerate-tri linker just for one method, and the actual code difference between skiplists and skipstrips is trivial (the collapse do/undo, most of the collapse data creation, and the data structures are all identical). And I just didn't have time to do mixed-mode - game deadlines approaching very fast. Fortunately it's simply a hybrid of some of the others, and the main difficulties are just book-keeping (which will probably be different in a real app anyway), not actual algorithms.




Many thanks to my fellow Muckyfoot coders for letting me slack off Startopia a bit during a hectic time to get these Gems finished. I appreciate it chaps.

Many thanks to my section editors Eric Lengyel and Sim Dietrich, and of course the book's editor, Mark DeLoura, without whom it just wouldn't happen. Thanks to one and all.



I think most of the swearing has been removed from the code. But apologies if any slipped through.

Any comments, questions or just random greets to me at TomF@muckyfoot.com.




Tom Forsyth, 1st May 2001
TomF@muckyfoot.com



