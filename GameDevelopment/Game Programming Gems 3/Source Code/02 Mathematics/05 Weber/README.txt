/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/


Additional Copyright & License
------------------------------
The subdirectory CoralTree contains third party software leveraged
for this demo's viewer.  That package uses the official Artistic License
published by opensource.org.  See CoralTree/meta/ART_LICE for a copy.


Introduction
------------
This source and binaries are examples designed to accompany the articles
"Constrained Inverse Kinematics" and "Improved Bones Deformation"
presented in "Game Programming Gems III" published by Charles River Media
in the year 2002.


General Usage Instructions
--------------------------

Some short scripts are included to compile and run the demo.

Win32:
	To compile using the command line:
		.\COMPILE.bat

	To run using the command line:
		.\RUN.bat

Linux (x86):
	To compile from a shell:
		sh COMPILE.sh

	To run from a shell:
		sh RUN.sh

If you don't want to compile anything yourself, the scripts
RUN_IK and RUN_MOTION run prebuilt demos for the two articles.

You should be able to double-click any of these scripts and run them
from your file manager.

The build system is based on .bld files.  They have some similarities
to makefiles, but they have more script-like capabilites.

My apologies to the Mac folks.  The Bones code is Mac compatible
and even has some PPC optimizations, but the testbed viewer is not.


Piloting the Demo
-----------------

First, hit 'h' for help like it says.  Almost everything is listed.
Hitting keys repeatedly cycles through their modes.
Some of the mouse operations can be "thrown" by releasing the
mouse button while still moving the mouse.
Aux and Special Event have no effect here.  Some options require
other options to be on (bone names requires bones to be shown).
Vertex numbers are broken and they will mess up the demo (have to restart).
The texture coordinates are all wrong, so you needn't try 't'.

Use the middle button or shift-left button to pan the view.
Use ctrl-left button to zoom and the right button to adjust time.
"Throwing" the time can be very useful with the IK mode to keep
the arm effector moving at a constant rate.

For IK, changing the time (rightmouse left and right) moves an effector
around the equator of the model space with a little added sine motion up
and down.  Try turning skin off (s), bones on (b), and constraints on (k).

The see the weights, use (w), usually best with the skin off and bones on.
There are two weight visualization modes.  The first only shows the weights
for the selected bone.  The second color-codes the bones red, green, and blue.
The cursor keys can select bones up and down the hierarchy,
and right and left between siblings (sorry, no picking).

'-' is good to profile the usage of CPU by the various steps, but keep
in mind that we didn't get to restore all the optimizations for this demo.


The Other Demo
--------------
The is a more polished executable-only demo available on the web.
You should be able to find it at
ftp://download.intel.com/ial/3dsoftware/3dviewer_demo.zip
or just look arounf under http://www.intel.com/ial/3dsoftware.


Tweaking the Parameters
-----------------------

Edit the file BonesTestbed/BonesTestbed.cpp.
Near the start is a whole bunch of #define statements.
A lot of them might not even work anymore.

You might like to change "TEST_IK" to switch between loading a motion
and using IK.  The block starting with "QUALITY" controls the relinking,
smoothing, weight regeneration, etc.  Beyond that, you're on your own.


Investigating the Source
------------------------

From the articles, you'll probably be most interested in
Bones/GPGBoneNode.cpp, Bones/GPGSkin.cpp, and Bones/GPGCharacter.cpp.

Only a subset of the files are included.  Many are given in binary form.
If you are interested in getting more information, please contact us.


Caveats, Notes, Etc.
--------------------

There are no collision effects hooked in for this demo.
The constraints assert some avoidance, but the model can still
touch inside his stomach.

The demo was pieced together with a mix of old and new code.
The code includes some prototype subsystems that are not at release quality.

The testbed utilizes some platform-independent Event/Window/Widget
libraries that I put together in a previous job (CoralTree).
I don't necessarily recommend that you use them or even look at them,
but you'll probably notice them.  The event abstraction it provided was a
real help and I didn't want to mess with X visuals again.  The package
uses the rather lenient Artistic License used by Perl.

The mousewheel isn't working under Win32.  Probably simple things,
but I'm out of time.
