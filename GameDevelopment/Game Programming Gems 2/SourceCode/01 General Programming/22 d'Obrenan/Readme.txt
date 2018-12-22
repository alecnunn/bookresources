GLWEBCAM
--------
  This demo has just been tested with Windows.  You obviously need a webcamera
installed with the latest Video for Windows drivers for it to work.  Webcameras
tested are the Logitech and Creative Labs webcams.  Also you need a 3d accelerated
video card that has an OpenGL ICD (I doubt this will work with early voodoo's).

IMPORTANT KEYS
--------------

  1     // Switch rendering to color mode
  2     // Switch rendering to cartoon mode
  3     // Switch rendering to greyscale mode
  4     // Toggle predator mode (while in cartoon mode)
  Enter // Press it for a special surprise :)

INI Documentation (webcam.ini)
------------------------------
  wco_use_webcam      1       // Do we even want to test for the webcam? Will load the captain otherwise...
  wco_cam_width     160       // The webcam's capture width
  wco_cam_height    120       // The webcam's capture height
  wco_cam_rendering   0       // What rendering mode do we want to display?  (0=color, 1=cartoon, 2=greyscale)
  wco_cam_updates   100       // The number of updates we get from the camera per milliseconds
  wco_cam_threshold  50       // The amount of movement before we capture a new texture

Default Movement Controls
-------------------------
  Directonal Keys // Fly around regularly
  A & Z           // Move the viewer up and down the 'y' axis (towards the sky and ground)
  S & X           // Look up and look down
  Q & W           // Strafe left, and right
  PgUP & PgDOWN   // Adjusts viewpoint pitch and roll
  END             // Stops the viewpoint
  HOME            // Flips the viewpoint 180 degrees


Have fun!

-Nathan d'Obrenan
www.firetoads.com
nathand@firetoads.com