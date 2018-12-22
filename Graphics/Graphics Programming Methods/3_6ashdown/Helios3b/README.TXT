          *****************************************************
          *** Welcome to the HELIOS Radiosity Renderer (TM) ***
          *****************************************************

Version:        1.03B
Release Date:   97/04/01

+----------------------------------------------------------------------+
| UPDATE: March 25th, 2003                                             |
|                                                                      |
| It has been ten years since the HELIOS Radiosity Renderer was        |
| developed for the book "Radiosity: A Programmer's Perspective."      |
| While much has changed since then, the C++ source code for HELIOS    |
| was designed to be mostly platform-independent.                      |
|                                                                      |
| Not only does HELIOS still compile under Visual C++ 6.0, there is    |
| essentially nothing in the source code that needs to be changed to   |
| implement a fully functional progressive radiosity renderer.         |
| (Please remember however that it was written when Windows NT had     |
| just been introduced, and when OpenGL display cards cost more than   |
| the rest of the computer.)                                           |
|                                                                      |
| While HELIOS lacks many of the features needed for a production-     |
| quality radiosity renderer (such as automatic meshing and physical   |
| light sources), it is more than adequate for use as a testbed for    |
| experimenting with eigenvector radiosity and illumination-based      |
| occlusion culling.                                                   |
|                                                                      |
| The book "Radiosity: A Programmer's Perspective" has been out of     |
| for several years now. However, it is available on CD-ROM from       |
| www.helios32.com. Like the HELIOS source code, most of the material  |
| in the book was designed to be independent of any operating system.  |
| More important, it provides in-depth documentation for the HELIOS    |
| Version 1.03b source code on a per-file basis.                       |
|                                                                      |
| The HELIOS Radiosity Renderer was written to introduce radiosity-    |
| based rendering to a wide audience of computer graphics enthusiasts. |
| After ten years, it is a pleasure to see that it is still just as    |
| useful a tool now as it was then.                                    |
|                                                                      |
|                                        Ian Ashdown, President        |
|                                        byHeart Consultants Limited   |
|                                                                      |
+----------------------------------------------------------------------+

1.  Introduction

Radiosity is an advanced computer graphics technique that you can use to 
synthesize photorealistic images without ray tracing. HELIOS brings this 
new and exciting technology to your personal desktop computer.

Included with this release is the full C++ source code for four separate 
versions of HELIOS. The code is derived from that presented in the book:

  Ashdown, I. 1994. "Radiosity: A Programmer's Perspective," New York,
  NY: John Wiley & Sons, Inc.

The source code is included for those programmers interested in 
experimenting with radiosity methods as an alternative to ray tracing 
techniques. It may be freely copied, redistributed, and/or modified for 
personal, non-commercial use ONLY, provided the copyright notice is 
included with all source code files.

2.  What is Radiosity?

There are two approaches to generating photorealistic images -- digital 
pictures that are difficult to distinguish from real photographs -- in 
computer graphics. The first approach involves ray-tracing techniques; 
the second approach is radiosity.

Radiosity is in a sense the complement of ray tracing. Ray-tracing 
techniques excel in the rendition of point light sources, specular 
reflections, and refraction effects. Radiosity methods accurately model 
area light sources, diffuse reflections, color bleeding effects, and 
realistic shadows. Whether you choose ray tracing or radiosity will 
depend in part on what effects you consider to be more important in your 
images.

Radiosity has the advantage of view independence. Using ray-tracing 
techniques, the number of ray-surface intersection calculations can 
increase geometrically with the complexity of the scene. Change your 
point of view and you typically have to start from scratch to generate a 
new image. With radiosity, however, you only need to perform the lighting 
calculations once for a given environment. Once they have been completed, 
you can quickly render a view of the environment as seen from any 
position and orientation.

Folklore has it that you need a high-powered graphics workstation or even 
a supercomputer to experiment with radiosity rendering techniques. Not 
so! All you need is an IBM PC-AT clone with an 80386 CPU and a math 
coprocessor, 4 megabytes of RAM, and a 256-color video display adapter.

Of course, a faster machine is always better. Using an off-the-shelf 
desktop computer with a 66-MHz 80486 CPU, you can create photorealistic 
images in ... are you ready for this? ... less than a minute. (Uaing a
200 MHz Pentium Pro, the same images can be created in less than 15
seconds!) Compared to the hours typically needed to ray trace
photorealistic images, there is something to be said for radiosity
techniques.

3.  Using HELIOS

HELIOS Version 1.03B is a Microsoft Windows 95 / NT program that has
the following hardware and software requirements: 

  *  An IBM PC-AT clone with an 80386 CPU and a math coprocessor (an 
     80486 or Pentium CPU is recommended. 
  *  A color video display capable of displaying at least 256 colors. 
  *  Microsoft Windows 95 or Windows NT.

Unlike many MS-Windows programs, you do *not* need to modify any system 
files (such as WIN.INI or AUTOEXEC.BAT) in order to run HELIOS, nor do 
you have to install anything. In fact, all you have to do is to store the 
following files in the same directory on your hard disk or a floppy 
diskette: 

  APPLE.ENT
  CEILING.ENT
  CHAIR.ENT
  FLOOR.ENT
  HELIOS.EXE
  HELIOS.HLP
  LIGHT_1.ENT
  LIGHT_2.ENT
  PICTURE.ENT
  ROOM.WLD
  SPHERE.ENT
  WALL.ENT

The ROOM.WLD and *.ENT files describe a sample environment that consists
of a room with a chair on the floor, a floating apple and an oblate
sphere, a picture on the wall, and two fluorescent lighting fixtures
that illuminate the room.

While this room may not be the most exciting of environments, it clearly
demonstrates radiosity's ability to model area light sources, diffuse
reflections, color bleeding effects and realistic shadows.

Assuming you have stored the files on a floppy diskette: 

  1. Insert the diskette into your floppy disk drive. 
  2. Start Microsoft Windows 95 / NT.
  3. Select Start.
  4. Select the Run... menu item.
  5. Enter A:HELIOS.EXE in the Command Line edit control. (Substitute the 
     drive letter of your floppy disk drive if necessary.) 
  6. Press the <Enter> key or select the OK button.

To run HELIOS from the directory C:\HELIOS on your hard disk: 

  1. Start Microsoft Windows.
  2. Select Start.
  3. Select the Run... menu item.
  4. Enter C:\HELIOS\HELIOS.EXE in the Command Line edit control. 
     (Substitute another directory name if necessary.) 
  5. Press the <Enter> key or select the OK button.

Once HELIOS is up and running, you can: 

  1. Choose Help from the HELIOS Radiosity Renderer menu bar.
  2. Select the Contents menu item.
  3. Select the Using HELIOS topic to view instructions on how to use 
     HELIOS.

The Using HELIOS topic provides a brief but complete tutorial on how to 
render the demonstration room in full color and view it from any angle.

4.  Four Versions of HELIOS?

The C++ source code included with this release includes four versions
of HELIOS: 

  HELIOS_C      Cubic tetrahedron
  HELIOS_H      Hemicube
  HELIOS_R      Ray casting
  HELIOS_S      Shading only

4.1 HELIOS_H - Hemicube

     This version is based on the progressive radiosity method. It uses 
     the classic hemicube method to determine form factors. 

4.2 HELIOS_C - Cubic Tetrahedron

    This version is also based on the progressive radiosity method. 
    However, it uses the cubic tetrahedron method to determine form 
    factors. Cubic tetrahedrons result in faster execution times (about 
    25 percent or so) and fewer form-factor aliasing artifacts than do 
    hemicubes.

    The program HELIOS.EXE in the root directory is really HELIOS_C.EXE. 

4.3 HELIOS_R - Ray Casting

    This version uses ray casting techniques to determine its form 
    factors. It does not employ BSP or any other polygon culling 
    techniques, however, and so it is quite slow in comparison to 
    HELIOS_C and HELIOS_H. 

4.4 HELIOS_S - Shading Only

    This version is *not* a radiosity renderer. Rather, it implements the 
    three-dimensional viewing system, polygon rendering, user interface 
    and other feature needed to implement a fully-functional radiosity 
    renderer. It is only capable of displaying flat-shaded polygons. 

All of the C++ source code and support files for these programs has been 
included in this release of HELIOS, as itemized in the next section.

5.  Release Contents

The complete set of files included with this release consists of: 

5.1 Root Directory

    HELIOS.EXE      HELIOS V1.03B MS-Windows 95 / NT  executable
    HELIOS.HLP      HELIOS V1.03B on-line help file

    README.TXT      This file

    [DEMO]          Demonstration environment subdirectory
    [HELP]          Help files subdirectory
    [INCLUDE]       HELIOS C++ include files subdirectory
    [SOURCE]        HELIOS C++ source code files subdirectory

5.2 Subdirectory DEMO

    APPLE.ENT       Demonstration room entity files
    CEILING.ENT
    CHAIR.ENT
    FLOOR.ENT
    LIGHT_1.ENT
    LIGHT_2.ENT
    PICTURE.ENT
    SPHERE.ENT
    WALL.ENT

    ROOM.WLD        Demonstration room environment file

5.3 Subdirectory HELP

    GLOSSARY.RTF    HELIOS help file glossary (Rich Text Format)
    HELIOS.HPJ      HELIOS help project file
    HELIOS.HLP      HELIOS help file
    HELIOS.RTF      HELIOS help file master (Rich Text Format)
    WIN_HELP.H      HELIOS help include file

5.4 Subdirectory INCLUDE    HELIOS C++ include files

    COLOR.H         Colors
    CT_CLIP.H       Cubic tetrahedron element clipping
    CT_DELTA.H      Cubic tetrahedron form factor deltas
    CT_POLY.H       Cubic tetrahedron elements
    CT_SCAN.H       Cubic tetrahedron element scanning
    CUBIC_T.H       Cubic tetrahedrons
    C_JITTER.H      Color jittering
    ENVIRON.H       Environment
    ERROR.H         Error reporting
    FF_CLIP.H       Form factor element clipping
    FF_DELTA.H      Form factor deltas
    FF_POLY.H       Form factor elements
    FF_SCAN.H       Form factor element scanning
    GAMMA.H         Gamma correction
    GENERAL.H       General
    HC_CLIP.H       Hemicube element clipping
    HC_DELTA.H      Hemicube form factor deltas
    HC_POLY.H       Hemicube elements
    HC_SCAN.H       Hemicube element scanning
    HELIOS.H        HELIOS MS-Windows interface
    HEMICUBE.H      Hemicubes
    INSTANCE.H      Instances
    OCT_QUAN.H      Octree color quantization
    OUT_POLY.H      Polygon output
    PARSE.H         Input file parser
    PATCH3.H        3D patches
    PROG_RAD.H      Progressive radiosity
    P_CLIP4.H       Homogeneous polygon clipping
    P_RENDER.H      Polygon rendering
    RAD_EQN.H       Radiosity equation solver
    RAY_CAST.H      Ray casting
    RAY_RAD.H       Ray-cast radiosity
    RESOURCE.H      HELIOS resource file
    SURFACE3.H      3D surfaces
    SYN_CAM.H       Synthetic camera
    TRANSFM3.H      3D transformations
    VECTOR3.H       3D vectors
    VECTOR4.H       Homogeneous vectors
    VERTEX4.H       Homgeneous vertices
    VIEW_SYS.H      Viewing system
    WIN_BMAP.H      MS-Windows BMP (bitmap) files
    WIN_HELP.H      Online help include file
    WIN_META.H      MS-Windows metafiles
    WIN_SBAR.H      Scroll bars
    WIN_TEXT.H      MS-Windows text manager
    WIN_TGA.H       Targa (TGA) files

5.5 Subdirectory SOURCE     HELIOS C++ source code files

    CT_CLIP.CPP     Cubic tetrahedron element clipping
    CT_DELTA.CPP    Cubic tetrahedron form factor deltas
    CT_SCAN.CPP     Cubic tetrahedron element scanning
    CUBIC_T.CPP     Cubic tetrahedrons
    C_JITTER.CPP    Color jittering
    ENVIRON.CPP     Environment
    ERROR.CPP       Error reporting
    FF_CLIP.CPP     Form factor element clipping
    FF_SCAN.CPP     Form factor element scanning
    GAMMA.CPP       Gamma correction
    HC_CLIP.CPP     Hemicube element clipping
    HC_DELTA.CPP    Hemicube form factor deltas
    HC_SCAN.CPP     Hemicube element scanning
    HELIOS.DEF      HELIOS definition file
    HELIOS.RC       HELIOS resource file
    HELIOS.CPP      HELIOS MS-Windows interface
    HELIOS_C.ICO    HELIOS V1.03A/CT icon
    HELIOS_H.ICO    HELIOS V1.03A/HC icon
    HELIOS_R.ICO    HELIOS V1.03A/RC icon
    HELIOS_S.ICO    HELIOS V1.03A/SH icon
    HEMICUBE.CPP    Hemicubes
    OCT_QUAN.CPP    Octree color quantization
    PARSE.CPP       Input file parser
    PATCH3.CPP      3D patches
    PROG_RAD.CPP    Progressive radiosity
    P_CLIP4.CPP     Homogeneous polygon clipping
    P_RENDER.CPP    Polygon rendering
    RAD_EQN.CPP     Radiosity equation solver
    RAD_TMP.CPP     Dummy radiosity equation solver
    RAY_CAST.CPP    Ray casting
    RAY_RAD.CPP     Ray-cast radiosity
    SYN_CAM.CPP     Synthetic camera
    TEST_1.CPP      Development test program #1
    TEST_2.CPP      Development test program #2
    TEST_3.CPP      Development test program #3
    TEST_4.CPP      Development test program #4
    VECTOR3.CPP     3D vectors
    VIEW_SYS.CPP    Viewing system
    WIN_BMAP.CPP    MS-Windows BMP (bitmap) files
    WIN_SBAR.CPP    Scroll bars
    WIN_TGA.CPP     Targa (TGA) files

6.  Compiler Notes

The C++ source code included with this release has been successfully
compiled for both Win16 and Win32 environments using Microsoft Visual
C++ 1.52, Visual C++ 5.0 and Borland C++ 5.0 compilers.

As explained in Section 4, there are four separate versions of HELIOS
that can be compiled from these files. The files to be included for
each version are:

    File Name		HELIOS_C	HELIOS_H	HELIOS_R	HELIOS_S
	------------------------------------------------------------
    CT_CLIP.CPP        X		   -		   -		   -
    CT_DELTA.CPP       X		   -		   -		   -
    CT_SCAN.CPP        X		   -		   -		   -
    CUBIC_T.CPP        X		   -		   -		   -
    C_JITTER.CPP       X		   X		   X		   X
    ENVIRON.CPP        X		   X		   X		   X
    ERROR.CPP          X		   X		   X		   X
    FF_CLIP.CPP        X		   X		   -		   -
    FF_SCAN.CPP        X		   X		   -		   -
    GAMMA.CPP          X		   X		   X		   X
    HC_CLIP.CPP        -		   X		   -		   -
    HC_DELTA.CPP       -		   X		   -		   -
    HC_SCAN.CPP        -		   X		   -		   -
    HELIOS.DEF         X		   X		   X		   X
    HELIOS.RC          X		   X		   X		   X
    HELIOS.CPP         X		   X		   X		   X
    HELIOS_C.ICO       X		   -		   -		   -
    HELIOS_H.ICO       -		   X		   -		   -
    HELIOS_R.ICO       -		   -		   X		   -
    HELIOS_S.ICO       -		   -		   -		   X
    HEMICUBE.CPP       -		   X		   -		   -
    OCT_QUAN.CPP       X		   X		   X		   X
    PARSE.CPP          X		   X		   X		   X
    PATCH3.CPP         X		   X		   X		   X
    PROG_RAD.CPP       X		   X		   -		   -
    P_CLIP4.CPP        X		   X		   X		   X
    P_RENDER.CPP       X		   X		   X		   X
    RAD_EQN.CPP        X		   X		   X		   -
    RAD_TMP.CPP        -		   -		   -		   X
    RAY_CAST.CPP       -		   -		   X		   -
    RAY_RAD.CPP        -		   -		   X		   -
    SYN_CAM.CPP        X           X 		   X		   X
    VECTOR3.CPP        X		   X		   X		   X
    VIEW_SYS.CPP       X		   X		   X		   X
    WIN_BMAP.CPP       X		   X		   X		   X
    WIN_SBAR.CPP       X		   X		   X		   X
    WIN_TGA.CPP        X		   X		   X		   X

The following preprocessor definitions are needed for compiling both
the C++ source code files *and* the resource file:

	HELIOS_C	_CUBIC_TETRA
	HELIOS_H	_HEMI_CUBE
	HELIOS_R	_RAY_CAST

If you are compiling HELIOS as a Win16 application for Windows 3.1
using Microsoft Windows V1.52, you will need to disable load optimization
for the resources compiler.

7.  Credits

HELIOS was written by:

  byHeart Consultants Limited
  620 Ballantree Road
  West Vancouver, B.C.
  Canada V7S 1W3

  Attn: Ian Ashdown, President

  Web URL:      http://www.helios32.com
  e-mail:       admin@helios32.com

Comments and questions are welcome.
