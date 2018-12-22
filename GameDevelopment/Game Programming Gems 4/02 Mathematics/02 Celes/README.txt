Extracting Frustum and Camera Information
-----------------------------------------

The distributed code exemplifies how the Frustum classes
can be used to extract frustum and camera information.

This code is freely available; you can redistribute it and/or modify it.
The software provided hereunder is on an "as is" basis, and
the author has no obligation to provide maintenance, support, 
updates, enhancements, or modifications.

Send your comments and suggestions to celes@inf.puc-rio.br.

Directory content:
   * frustum
      - root directory
   * frustum/alg
      - alg directory: provides basic classes to extract information
   * frustum/alg/frustum.h
      - defines AlgFrustum class
   * frustum/alg/plane.h
      - defines AlgPlane class
   * frustum/alg/vector.h
      - defines AlgVector class
   * frustum/vgl
      - vgl directory: provides class to extract OpenGL viewing info
   * frustum/vgl/frustum.h
      - defines the interface of the VglFrustum class
   * frustum/vgl/frustum.cpp
      - implements the VglFrustum class
   * frustum/frustum_test.cpp
      - demonstrates the code in action: it extracts viewing information,
        based on the OpenGL modelview and projection matrices, and light 
        source information, based on a perpective shadow matrix, exemplifying
        arbitrary projective transformation.
        This demo shows a simple scene and allows the user to modify the
        viewer and the light parameters, showing the corresponding values
        extracted from the matrices.
   * frustum/linux
      - linux/unix stuffs
   * frustum/linux/Makefile
      - Makefile for linux
   * frustum/windows
      - windows stuff
   * frustum/windows/windows.dsw
      - Visual C++ 6 workspace
   * frustum/windows/frustum_test.dsp
      - Visual C++ 6 project
   * frustum/windows/frustum_test.exe
      - Application that demostrates the program (just run it)

