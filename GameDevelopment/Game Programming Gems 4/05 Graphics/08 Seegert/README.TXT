Techniques to Apply Team Colors to 3-D Models
Greg Seegert – Stainless Steel Studios
gseegert@alum.wpi.edu

The sample code was created using the DX9 app wizard.  Most of it has not been modified, so the real files of interest are GG4Model.h/cpp.  This contains all the setup and render code for the various team color techniques.  The code is commented quite a bit, and a lot of performance and memory saving techniques have been discarded for the sake of clarity.

Pressing 'F3' will cycle through the techniques.  To see the performance impact of the various techniques, you should disable vsync (or run in windowed mode).  The FPS counter will then more accurately reflect how long things are taking since it won't be tied to the refresh rate.

A fourth technique has been included - this technique makes use of vertex and pixel shaders to make team color slightly cooler looking and a bit more exciting (team color is admittedly a bit boring).  The effect would probably look best on a futuristic model however =)


Fun Legal Stuff:

The included models and textures are for demonstration purposes only.  Included art assets are Copyright © 2003, Stainless Steel Studios, Inc.  All rights reserved.