Procedural Cloud Demo for DirectX8 - Game Programming Gems 2
April 24, 2001

Author: Kim Pallister - Intel Technology Diffusion Team
email: Kim.Pallister@intel.com

----------------------------------------------------------------------
 
Copyright © 2001 Intel Corporation
All Rights Reserved

Permission is granted to use, copy, distribute and prepare derivative works of this 
software for any purpose and without fee, provided, that the above copyright notice
and this statement appear in all copies.  Intel makes no representations about the
suitability of this software for any purpose.  This software is provided "AS IS." 

Intel specifically disclaims all warranties, express or implied, and all liability,
including consequential and other indirect damages, for the use of this software, 
including liability for infringement of any proprietary rights, and including the 
warranties of merchantability and fitness for a particular purpose.  Intel does not
assume any responsibility for any errors which may appear in this software nor any
responsibility to update it.

----------------------------------------------------------------------

Contents
========

1. Usage
2. Sample overview
3. Known Issues
4. Contact Info

1. Usage
--------

Run the demo and wait for clouds to appear. They should take about 8 seconds.

Pressing 'H' while the demo is running will bring up a list of keys.

2. Sample overview
------------------

The framework for this sample was constructed using a custom appwizard we've 
developed for creating Direct3D frameworks. As such, there are many classes
and files not used by the demo, but that have been left in here in case they
of use to readers.

The files/classes that are relevant are (.cpp/.h implied):

main     - pretty bare-bones. Fires up an instance of IawWindow3D class and
           one instance of the ProceduralCloudDemo class.

IawWindow3D - Creates the window, creates an IawD3DWrapper, uses the wrapper
           to do device/mode enumeration, and then builds menus using that 
           info. The IawWindow3D handles messages as well.

IawD3DWrapper - Handles D3DDevice, front & back buffer, etc.

ProceduralCloudDemo - ** This is where the bulk of the action happens **
           The world geometry is handled here, as is the shadermanager, etc.
	   This uses a "RandTexture" class to provide it with 4 summed 
	   octaves of noise, which it then turns into clouds.

StaticRandTexture - Handles maintaining a texture which is a sum of four 
           octaves of noise. Several textures are used to create the final
           resulting texture. We use this for the detail texture on the terrain.
	   Though it isn't used in the clouds themselves, it may help to become
	   familiar with the class before looking at the more complicated
	   RandTexture class.

RandTexture - Handles maintaining a texture which is a sum of four octaves of
           noise. Each of these is a render target texture, which is animated by
           interpolating between updates of two different rendertarget textures.

ShaderManager - Handles storing and activating groups of stateblocks to compose 
	   shaders

The bulk of the technique described in the gem is done in the RandTexture 
class (animating and summing octaves of noise to create turbulence texture) and 
in ProceduralCloudDemo (turning the turbulence into clouds).

3. Known Issues
---------------

The sample requires DirectX 8.0

The sample also requires a 3D accelerator that supports dual texture and 
render-to-texture. Render-to-texture support is somewhat sketchy, especially
in DX7 drivers and earlier. If you have problems, check your card manufacturers
website for an updated driver first.

The driver has been tested on Nvidia GeForce2 & GeForce3, Matrox G400 & G450, 
and ATI Radeon, under Windows 2000 and Windows98.

GeForce2: seems to work OK. 16bit color mode is very ugly because it only supports
565 rendertargets, so green channel gets out of sync with the others. 32bit 
color looks good.

The demo crashes on some GeForce 2 systems when running on the retail
runtime of DirectX8. Be sure to have the most up to date drivers, and if the
problem persists, run the demo on the debug runtime.

GeForce3: No known issues.

G400/G450: No known issues.

ATI Radeon: runs well, but exhibits some artifacts when their anti-aliasing is
switched on. The driver doesn't appear to be properly clearing the back buffer.

4. Contact Info
---------------

Feel free to contact me regarding any issues or questions regarding the sample
code or the article. I can be reached at kim.pallister@intel.com. More of my
demos and sample code, including the appwizard used to create this project,
can be found on http:\\www.intel.com\ids\


