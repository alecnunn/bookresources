    GRAPHICS PROGRAMMING METHODS
    Edited by Jeff Lander
    

    SIMULATING AND RENDERING PARTICLE SYSTEMS

    Waldemar Celes (celes@inf.puc-rio.br)
    Antonio Calomeni (calomeni@tecgraf.puc-rio.br)

    PUC-Rio - Pontifical Catholic University of Rio de Janeiro, Brazil
 

============
- CONTENTS -
============
I. Introduction
II. DEMO
III. Hints on using the shaders in your applications
 III.A PSIZE.VP
 III.B RK3.VP
IV. Contact information


===================
- I. Introduction -
===================

This README file provides information about the files included in this distribution. The shaders allow you to texturize particles as Point Sprites, computing their size on screen accordingly and optionally project them to a desired frame time, in case your simulation is decoupled from visualization, which is recommended.

=============
- III. DEMO -
=============

The DEMO features a camp scene with dynamic fog, fire and steam.
An area emitter was set, always in front of the viewer, to produce the dynamic fog. 
The particle rate is 600 particles per second.
A spot emitter was set right beneath the teakettle, facing up, to produce the camp fire. 
The particle rate is 200 particles per second.
Finally, another spot emitter was set for the teakettle steam, also facing up. 
The particle rate was 30 particles per second.

This demo has run successfully on NVIDIA GeForce family of cards with the Detonator driver.

The program prompt gives you information about the program controls.

Clicking the right mouse button brings up a menu that allows you to switch techniques:
   -> Simulation decoupled from visualization
   -> Simulation coupled with visualization
   -> Wireframe toggle

In order to notice the gain in performance when using the decoupled strategy, make sure
the rasterizer stage does not represent bottleneck of the rendering pipeline (in such a 
case, a gain in CPU does not interfere with the overall performance).
Switching to wireframe mode may help you on this comparison.
   

==========================================================
- III. Hints on using the shaders in your applications -
==========================================================

Two shaders are available:
     PSIZE.VP - this shader transforms the incoming vertex by the current modelview/projection matrix. It computes the point size of the incoming vertex according to the viewer distance.
     RK3.VP - this shader first projects the incoming vertex (particle) to a desired frame time using the Runge Kutta 3 method, modifying it's state. Then, it does what PSIZE.VP does: transforms the resulting vertex by the current modelview/projection matrix and computes the vertex point size.
     
In order to use both of these shaders, your graphics hardware must support:
     - the GL_NV_vertex_program extension
     - the Point Sprite feature (GL_POINT_SPRITE_NV)
     
Note: In order to compute the point size inside the shader, you must instruct the OpenGL API to ignore the glPointSize() setting and use the shader output instead. This can be done by adding the following function call to your program:
     glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
     
For more specific information regarding the use of these shaders please refer to the following sections. In each section a piece of C++ code is presented illustrating how to pass the parameters to the shader using NVIDIA's extensions. These pieces of code are merely suggestions.

=================
- III.A PSIZE.VP -
=================

This shader should be used with particles that doesn't need projection. If your physical simulation is coupled with visualization, you should use this shader. It just transforms the incoming vertex using current transformation matrices and computes it's point size accordingly.

This shader was used in the DEMO included just for performance comparison purposes. Physical simulation coupled with visualization is not recommended, so this shader shouldn't be needed.

You should pass the following parameters to this shader:

   vertex attribute 4 (secondary color) -> [psize, znear, scale, -]
         psize -> the particle point size in world space
         znear -> the current znear of the viewing frustrum
         scale -> the scale factor, see below for details
         the 4th component is not used.
                           
   vertex position -> particle position

   Computing the scale factor:
     The following piece of C++ code computes the scale factor and passes the necessary parameters to the shader:
     
       float delta_y = 2*ZNear*tan(fovy/2);
       float scale = VPHeight / delta_y; // VPHeight is your viewport height
       glVertexAttrib4fNV(4, psize, ZNear, scale, 0.0f); // 4th component is not used

       glVertex3f(px,py,pz); // particle position
       

===============
- III.B RK3.VP -
===============

This shader projects the incoming vertex (particle) to a desired frame time using the Runge Kutta 3 method. After that, it transforms the resulting state using current transformation matrices and then computes the point size accordingly.

This shader requires the following additional features:
     - 6 texture coordinate units

In order to use this shader, physical simulation should be decoupled from visualization. The desired frame time t should reside in the last simulation step of the current particle.

Given the last simulation step:
   t0 is the start of the step
   h is it's step size (different from h', see below)

You should pass the following parameters to this shader:

   position, normal -> [position, velocity] at time t0
   vertex weight -> [h', t] (only x,y components are used)
                      h' -> step size from t0 to frame time
                      t -> frame time scaled to [0..1]
                      t0 + h*t resides inside last simulation step
   primary color -> particle color, if texturing is disabled

   secondary color -> [psize, znear, scale, -] (the scale factor can be computed the same way as stated in the previous section)
   
   vertex attribute 6 -> [2.0, 3.0, 6.0, 0.166]
                        coefficients used when computing the final state

   the 4th component is not used to store projection parameters
   texture coordinate 0 -> [a, a, a, -] - position
   texture coordinate 1 -> [a, a, a, -] - velocity
   texture coordinate 2 -> [b, b, b, -] - position
   texture coordinate 3 -> [b, b, b, -] - velocity
   texture coordinate 4 -> [c, c, c, -] - position
   texture coordinate 5 -> [c, c, c, -] - velocity

   vertex position -> particle position at time t0


The following piece of C++ code passes the parameters to the shader: ('p' is a particle)

   double pstate[6];
   p->GetPreviousState(pstate); // the state at time t0

   double h_ = time - p->GetPrevTime(); // h' = frame time - t0
   double t = h_ / (p->GetTime()-p->GetPrevTime()); // the frame time must be scaled to [0..1]


   glVertexAttrib3fNV(1, h_, t, 0.0f);

   glNormal3f(pstate[3], pstate[4], pstate[5]); // particle velocity at time t0

   glVertexAttrib4fNV(4, psize, ZNear, scale, 0.0f);

   glVertexAttrib4fNV(6, 2.0, 3.0, 6.0, 0.166);

   double params[18];
   p->GetProjectionParams(params); // parameters are computed and stored in the vector
   int dim = 6;

   glVertexAttrib3fNV(8, params[0], params[1], params[2]);
   glVertexAttrib3fNV(9, params[3], params[4], params[5]);
   glVertexAttrib3fNV(10, params[6], params[7], params[8]);
   glVertexAttrib3fNV(11, params[9], params[10], params[11]);
   glVertexAttrib3fNV(12, params[12], params[13], params[14]);
   glVertexAttrib3fNV(13, params[15], params[16], params[17]);

   glVertex3f(pstate[0], pstate[1], pstate[2]);


The method 'GetProjectionParams' computes the terms a, b and c needed by the shader to perform the projection. Please refer to the article for more information about how to compute these terms.

The velocity data is passed, but not used in this shader, since we're interested in projecting the particle position only. But, if you need so, it's easy to add code that projects the particle velocity as well.


===========================
- IV. Contact information -
===========================

Questions regarding the use of the shaders should be addressed to Antonio. 
We ask that you please read this file carefully before sending your question.
