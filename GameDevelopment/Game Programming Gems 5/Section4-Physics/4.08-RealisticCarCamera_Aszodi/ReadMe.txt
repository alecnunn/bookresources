+=======================================================================+

             Realistic Camera Movement in 3D Car Simulator

 Demonstrative OpenGL application presenting real movement of a camera
 which follows a moving car-like object

 Authors: Aszódi, Barnabás   [ab011@hszk.bme.hu]
          Czuczor, Szabolcs  [cs007@hszk.bme.hu]
 Created: July, 2004., Budapest, Hungary

 Budapest University of Technology and Economics,
 Department of Control Engineering and Information Technology (BME-IIT)
 Computer Graphics Group
+=======================================================================+


 System requirements:
+--------------------+
 Hardware:
  - Average PC with 3D graphics card

 Software:
  - Microsoft Visual Studio .NET 2002 C++ framework v1.0 (or above)
    for editing and compiling the code
  - installed OpenGL and GLUT libraries


 Running the application:
+------------------------+
  The application can be started with the file "shakecam.exe", which is a
32-bit Windows application. You need the texture file "asphalt.tga" in the
same directory.


 Opening the Visual C++ project (solution) for editing
+-----------------------------------------------------+
  You can double-click on the file "shakecam.sln" or "shakecam.vcproj",
which starts the developer environment of Visual Studio .NET 2002 (v1.0)
automatically, if it is installed on your system.


 Files in the package:
+---------------------+
  - asphalt.tga ----- texture image for the application
  - camera.h -------- class definition of our virtual camera model
  - car.h ----------- class definition of a virtual moving car
  - environment.h --- class definition of the environment of our virtual scene
  - object.h -------- definition of the car's and the environment's super class
  - ReadMe.txt ------ THIS README FILE
  - resource.h ------ needed for the VC++ project
  - sc_big.ico ------ icon of the application
  - sc_small.ico ---- icon of the application
  - shakecam.aps ---- needed for the VC++ project
  - shakecam.exe ---- the compiled executable of our application
  - shakecam.rc ----- needed for the VC++ project
  - shakecam.sln ---- solution file of the project (open this for editing the
                      project)
  - shakecam.auo ---- needed for the VC++ project
  - shakecam.vcproj - descriptor of the project
  - shakecam_01.cpp - source code of the main part of the application
  - shakecam_01.h --- header of the main part of the application
  - stdafx.cpp ------ needed for the VC++ project
  - stdafx.h -------- needed for the VC++ project
  - tgaldr.h -------- definition of our TGA image loader class
  - vector.h -------- definition of our 3D vector class



 Keyboard layout of the application:
+-----------------------------------+

 [F1] - toggle the help screen ON/OFF
 [F2] - toggle full screen mode
 [UP] [END] [LEFT] [RIGHT] - accelerate ahead, break, steer left, steer right
                             the car
 [DOWN] - accelerate backwards
 [SPACE] - hand break (sudden stop)
 [1]...[7] - show/hide screen message 1...7 (#7 is the DEBUG-graph by default)
 [R] - Reset scene
 [Q] - toggle direct follow / human camera control
 [A] [D] [W] [S] - rotate left/right, raise/sink camera's eye
 [O] - toggle auto zoom ON/OFF
 [I] [U] - zoom in/out
 [TAB] - switch camera
 Numpad
    [7] [8] [9] - huge amplitude
    [4] [5] [6] - moderate amplitude
    [1] [2] [3] - small amplitude
    Slow, fast, extreme fast shaking of the car (vertical movement)
 [ESC] - exit



 Screen messages:
+----------------+

  The application provides a way to monitor variables in run-time. We have
defined 8 screen messages (numbered from 0 to 7), which can be shown on the
OpenGL screen in specified positions. These messages can be turned on or off
by pressing the appropriate number key [1] to [7].

The first screen message (indexed by 0) contains the HELP screen describing
the keyboard layout. This message can be turned on or off by pressing the
[F1] key (and not [0]).

The positions of the messages are defined by a Vector3f object. The last
screen message is special, because its 3 coordinates (x, y, z) are used for
drawing three different graphs in time colored by red, green and blue. The
actual values of these graphs can be originated from any variable in the
code.

Hereby using these screen messages is a perfect way to debug the
application real-time.

Default associations:
  #0 - HELP screen [F1]
  #1 - actual frame rate [1]
  #2...#6 - not defined [2]...[6]
  #7 - DEBUG-graph [7]: RED (X coord.) ---- X coordinate of the camera's
                                            target
                        GREEN (Y coord.) -- X coordinate of the car
                        BLUE (Z coord.) --- the difference between the
                                            actual and the final value of
                                            the camera's fov (field of view)



