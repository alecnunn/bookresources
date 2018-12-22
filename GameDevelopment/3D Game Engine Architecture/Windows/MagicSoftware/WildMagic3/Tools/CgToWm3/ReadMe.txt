CG Converter
============

Usage
-----
CgToWm3 <inputfile> [-forceVS2] [-nostrip] [-name <name>]
                    [-ventry <entry>] [-pentry <entry>]

inputfile: Cg filename to be processed (not optional)
-name: Change the name of the effect (default is from inputfile)
-forceVS2: Force DX vertex shaders to be VS2.0
-nostrip: Don't strip Hungarian prefixes [a,f,k] from variables
-ventry <entry>: Vertex shader main is <entry> instead of 'vmain'
-pentry <entry>: Pixel shader main is <entry> instead of 'pmain');

Options
-------
<inputfile>
	The filename of the Cg program to be converted.  Required.
-name
	Change the name of the effect.  By default, the name of the Effect class
    will be taken from the filename (minus the extension and after being
    capitalized).  For example, by default something.cg will by default
    create the SomethingEffect.*, SomethingVShader.*, and
    SomethingPShader.* classes.  The -name option will override this default
    behavior and allow an arbitrary name.

-forceVS2
	Unfortunately, there is no way for the Cg runtime to be able to tell if
    your program is too long or not.  Thus, the CgConverter cannott tell that
    even though your program compiles syntactically into vs_1_1, it will not
    run because of the instruction limit.  Pixel shaders do not have this
    problem.  So, if your program seems long (>128 instructions) and you get
    errors compiling in DirectX or cgc issues a warning that your program is
    too long, issue this flag to fix that problem.

-nostrip
	As of right now, if your constants are prefixed with Hungarian prefixes
    (case sensitive 'a', 'f', and 'k'), then those prefixes will be stripped
    off when creating the Shader/Effect API.  If you do not want this
    behavior, then pass this option.

-ventry
	This is the "main" function of the vertex shader.  By default, it will
    be "vmain".

-pentry
	This is the "main" function of the pixel shader.  By default, it will
    be "pmain".


Requirements
------------
Cg development environment (cg libraries and include files).

General Usage
-------------
The CgConverter takes a Cg file and creates a series of classes for you to
more easily manipulate the Effect and shaders.  At the very least, it will
create an Effect.h and an Effect.cpp file.  If there exist vertex and pixel
shaders, then it will also create VShader.{h,cpp} and PShader.{h,cpp} files.
It may also create .inl files for the get/set functions for constants.  The
Cg programs themselves are compiled and stored as static member data in each
of the shaders.

One thing the Cg Converter does not do is give you detailed compile time
error information.  If your program has errors, the converter will not
succesfully run and will give you errors that it couldn't get the program to
compile under any profiles.  However, it won't give you any more information
than that.  It is recommended that in your project files you run the cgc
compiler first (perhaps with a -nocode flag) before the cg converter.  That
way when there are errors in the program, you can get better information
about them.

Accessing Rendering State
-------------------------

You can access nearly all WildMagic rendering state from within both your
vertex and pixel shader programs in both DirectX and OpenGL.  To access
renderer state, you use certain variable names (which are all prefixed by
"Wm3").  Because of this, if you prefix a variable with "Wm3" and it is not
a valid state constant, the Cg Converter will throw an error.  The names
should be fairly self explanatory about what state they represent.  Here is a
listing of types and names from Source/Wm3StateConstant.cpp:
(See Source/ShaderConstant.* for more details)

	float4 Wm3CameraPosition
	float4 Wm3CameraUp
	float4 Wm3CameraRight
	float4 Wm3CameraDirection

	float4x4 Wm3TransformMVP        (*)
	float4x4 Wm3TransformMV         (*) 
	float4x4 Wm3TransformM          (*)
	float4x4 Wm3TransformP          (*)

	float4 Wm3FogColor
	float4 Wm3FogParams

	float4 Wm3MaterialEmissive
	float4 Wm3MaterialAmbient
	float4 Wm3MaterialDiffuse
	float4 Wm3MaterialSpecular
	float4 Wm3MaterialShininess

	float4 Wm3LightPosition#        (**)
	float4 Wm3LightDirection#       (**)
	float4 Wm3LightAmbient#         (**)
	float4 Wm3LightDiffuse#         (**)
	float4 Wm3LightSpecular#        (**)
	float4 Wm3LightSpotcutoff#      (**)
	float4 Wm3LightAttenparams#     (**)

(*): All matrix state constants can optionally have certain suffixes:
"Trans", "Inv", and "InvTrans".  These correspond to transformations
(transpose, inverse, and inverse transpose respectively) on that matrix.  For
example, Wm3TransformMInv or Wm3TransformPInvTrans are both valid state
constants.

(**): All light constants are REQUIRED to have a number after them,
corresponding to which light it refers to.  Thus, Wm3LightPosition0 and
Wm3LightAmbient5 are valid state constants.  It is the responsibility of the
user to ensure that the lights that are referenced have been defined and
attached as state to the part of the scene graph that is using the shader by
the application itself.

Compatibility Issues
--------------------

If you would like to make your Cg programs compatible with both OpenGL and
DirectX you need to follow a few restrictions:

1) No access of GL state from within the Cg program.
2) DirectX programs (at least in 8.1, and with ps1.1-1.3) truncate inputs to
   pixel shaders into the [0..1] range.  Typically you can munge and unmunge
   the values to allow for a larger range of inputs.  Within the pixel shader
   itself a much larger range is acceptable.
3) OpenGL sets the w component of the outgoing position vector in the vertex
   shader to be one.  DirectX does care and will happily project your points
   all over the place if it is something other than one.


Shader Versions
---------------

Cg has no support for ps.1.4.  This version of DirectX pixel shaders
essentially allows two ps.1.3 shaders to run back to back.  This allows for
things such as dependent texture reads and more instructions (that many pixel
shaders need).  Many cards (GeForce4TI/Radeon8x00) can do ps.1.4 but only
newer cards (GeForceFX/Radeon9x00) have full support for ps.2.0.  If you have
a shader that you would really like to be compatible with more cards, then
your best bet is to somehow compile it into ps.1.4 and then build your own
shader file.  See the building your own shader file section for details.
