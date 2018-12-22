/****************************************************************************

   FILE: common.h

	DESCRIPTION: header for common (cross-platform) code for xldemo

****************************************************************************/
#include "switches.h"
#include "xlheader.h"

#if !defined (COMMON_CLASSES)
#define COMMON_CLASSES

#include "object.h"
#include "textout.h"

class AVEnvironment
{
private:
   int width, height;                                 // current width, height of window
   bool displayFPS;		                              // display FPS?
   bool displayEAX;									  // display EAX settings?
   int valEAX;                                        // EAX preset value
   int fps;					                          // frames per second figure
   int numSamples;                                    // number of samples for current fps average
   int fpsSum;                                        // sum of fps figures
   float playerPos[3];                                // camera/listener position
   float playerAngle;                                 // camera/listener angle
   AudioEnv Audio;									  // audio system
   Object Sky,                                        // objects in the environment
          Ground;
   ObjectList HouseList,
	      RadarBeamList,
          CarList,
          VanList;
   int phaserEffect,								  // handle to sound effects
	   vanEffect,
	   carEffect;
   GLTextOut textobject;
public:
	AVEnvironment ();
	~AVEnvironment ();
   void Init ();
   void PlaceCamera ();
   void DrawBuffer (float);
   void Step(float, float);
   void Turn(float);
   void ChangeView(int, int);
   void NewHouse(float, float);
   void NewRadarBeam(float, float, float);
   void NewVan(float, float, int);
   void NewCar(float, float, int);
   void ToggleFPS ();
   void SetFPS(float);
   void IncrementEnv();
};

#endif // !defined (COMMON_CLASSES)	
