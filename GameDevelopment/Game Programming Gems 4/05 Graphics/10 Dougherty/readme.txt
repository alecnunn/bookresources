
Copyright © 2003 Michael Dougherty (mdougher@hotmail.com).
All rights reserved worldwide.

Permission to copy, modify, reproduce or redistribute this source code is
granted provided the above copyright notice is retained in the resulting 
source code.


Notes:
	This demo was tested on an ATI 9600 and an ATI 9700.
	* It has not been tested on any other cards.

	The demo requires pixel shader 1.1 and the DirectX 9.0 Summer Update runtime.
	* No DirectX CAP bits are queried.

	Up to 64 textures can be put in the local textures directory and viewed
	in the application.

	If you run the application full screen, the hardware gamma ramp is used by
	default and the HUD is drawn with an inverse gamma transform.

	You can toggle the usage of either hardware gamma ramp or pixel shader gamma ramp
	in the full screen version.

	The windowed version used a pixel shader to apply the gamma transform.

Controls:
	Esc:	Exit
	N:	Next texture
	H:	Toggle HUD
	D:	Toggle the gamma ramp and histogram displays
   	G:	Toggle the dynamic gamma effect
   	P:	Toggle hardware or pixel shader gamma ramp (Full Screen Mode Only)
   
Display:
      Upper Left:	Scene histogram
      Center Left:	Current gamma ramp transform
      Bottom Left:	HUD for demonstating the inverse gamma ramp pixel shader
   
WARNING:
	This application has not been thoroughly tested and is for demonstration
	purposes only.
	YOU RUN THIS APPLICATION AT YOUR OWN RISK.
