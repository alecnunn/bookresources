
/****************************************************************************

   PROGRAM: common.cpp

	DESCRIPTION: common (cross-platform) code for xldemo

****************************************************************************/
#include "common.h"

#ifndef WINDOWS
using std::ends;
#endif

// constructor
AVEnvironment::AVEnvironment ()
{
   // set up initial player position and orientation
   int i;
   for (i=0; i < 3; i++)
   {
	   playerPos[i] = 0.0f;
   }
   playerAngle = 0.0f;
   
   // set up display control variables
   displayFPS = false;
   displayEAX = false;
   valEAX = 0;
}

// destructor
AVEnvironment::~AVEnvironment ()
{
	fps = false;
	numSamples = 0;
}

// Init
void AVEnvironment::Init ()
{
   // position player
   playerPos[0] = 0.0f; // x
   playerPos[1] = 2.0f; // y
   playerPos[2] = 30.0f; // z

   // orient player
   float ori[6];
   ori[0] = (float) sin(playerAngle);
   ori[1] = 0;
   ori[2] = (float) -cos(playerAngle);
   ori[3] = 0;
   ori[4] = 1;
   ori[5] = 0;

   Audio.Init();

   Audio.ListenerPosition(playerPos, ori);

   // load up radar sound effect and loop until program stops
   float tempPos[3];
   tempPos[0] = 4.5f;
   tempPos[1] = 11.25f;
   tempPos[2] = -4.5f;
   Audio.SetSourcePosition(Audio.PlayFile((char *) "radar.wav", true), tempPos);

   // load up phaser sound effect and position in first house
   phaserEffect = Audio.LoadFile("phaser.wav", false);
   tempPos[0] = 3.0f;
   tempPos[1] = 6.0f;
   tempPos[2] = -3.0f;
   Audio.SetSourcePosition(phaserEffect, tempPos);

   // load up car sound effect and begin looping 
   carEffect = Audio.PlayFile("motor_a8.wav", true);
   tempPos[0] = 0.0f;
   tempPos[1] = 0.0f;
   tempPos[2] = -300.0f;
   Audio.SetSourcePosition(carEffect, tempPos);

   // load up van sound effect and begin looping
   vanEffect = Audio.PlayFile("motor_b8.wav", true);

   tempPos[0] = 0.0f;
   tempPos[1] = 0.0f;
   tempPos[2] = -300.0f;
   Audio.SetSourcePosition(vanEffect, tempPos);

   // load up funk sound effect and loop until program stops
   tempPos[0] = 24.0f;
   tempPos[1] = 0.0f;
   tempPos[2] = 18.0f;
   Audio.SetSourcePosition(Audio.PlayFile("funk.wav", true), tempPos);

   // init objects
   Sky.Init("Sky.mdl");
   Ground.Init("Ground.mdl");
   Ground.SetAudioObject(&Audio);
   NewHouse(0.0f, 0.0f);
   NewHouse(24.0f, 18.0f);
   NewRadarBeam(4.5f, 0.0f, -4.5f);
   NewCar(-12.0f, -180.0f, carEffect);
   NewVan(-18.0f, -180.0f, vanEffect);

   // GL init
   PlaceCamera();

   glFrontFace(GL_CCW);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
   glShadeModel(GL_SMOOTH);
   
   GLfloat backcol[4] = { 0.6f, 0.6f, 1.0f, 1.0f };
   glClearColor(backcol[0], backcol[1], backcol[2], backcol[3]); // background color for non-foggy world

   textobject.Init(); // text object needs to be initialized once
   
#ifdef LIGHTING   
   // lighting init
   GLfloat fogcol[4]   = { 0.8f, 0.8f, 0.8f, 1.0f };
   GLfloat lightpos[4] = { -30.0f, 36.0f, 30.0f, 0.0f };
   GLfloat lightamb[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
   GLfloat lightdif[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
   
   glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
#endif // LIGHTING

   // optional blending
#ifdef BLENDING
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif // BLENDING

   // optional fog
#ifdef FOG
	glClearColor(fogcol[0], fogcol[1], fogcol[2], fogcol[3]);
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogcol);
    glFogf(GL_FOG_DENSITY, 0.09);
    glHint(GL_FOG_HINT, GL_NICEST);
#endif
}

void AVEnvironment::PlaceCamera ()
{
	static bool inRange = false;
	
   // setup perspective view
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-0.1333, 0.1333, -0.1, 0.1, 0.2, 300.0);

   // place camera
   gluLookAt(playerPos[0], playerPos[1], playerPos[2], 
      (playerPos[0] + sin(playerAngle)), playerPos[1], (playerPos[2] - cos(playerAngle)),
      0.0, 1.0, 0.0);

   // place listener at camera
   float directionvect[6];
   directionvect[0] = (float) sin(playerAngle);
   directionvect[1] = 0;
   directionvect[2] = (float) -cos(playerAngle);
   directionvect[3] = 0;
   directionvect[4] = 1;
   directionvect[5] = 0;
   Audio.ListenerPosition(playerPos, directionvect);

   // update ostruction level between vehicles, houses, and the listener
   Audio.UpdateObstruction(carEffect);
   Audio.UpdateObstruction(vanEffect);

   double distance = (playerPos[0] - 4.5) * (playerPos[0] - 4.5) + (playerPos[2] + 4.5) * (playerPos[2] + 4.5);
   if ((inRange == false) && (distance < 150.0f))
   {
	   Audio.Play(phaserEffect);
	   inRange = true;
   } else
   {
	   if (distance > 150.0f) 
	   {
	     inRange = false;
	     Audio.Stop(phaserEffect);
	   }
   }
}

void AVEnvironment::DrawBuffer (float timediff)
{
   static float RadarRotation = 0.0f;
   static float CarTranslation = 2.0f;
   static float VanTranslation = 45.0f;
   
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // draw objects
   Sky.Draw();
   Ground.Draw();
   HouseList.Move(timediff);
   HouseList.Draw();
   RadarBeamList.Move(timediff);
   RadarBeamList.Draw();
   CarList.Move(timediff);
   CarList.Draw();
   VanList.Move(timediff);
   VanList.Draw();

   // draw FPS text
   if (displayFPS == true)
   {
	   glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	   gluOrtho2D(0.0f, (GLfloat) width, 0.0f, (GLfloat) height);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   GLfloat textcolor[3] = { 1.0f, 0.0f, 0.0f };
	   glColor3fv(textcolor);
	   glRasterPos2i(5, 5);
	   ostrstream s;
	   s << "FPS " << fps << ends;
	   textobject.PrintString(s.str());
   }
   
    // draw EAX text
   if (displayEAX == true)
   {
	   glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	   gluOrtho2D(0.0f, (GLfloat) width, 0.0f, (GLfloat) height);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   GLfloat textcolor[3] = { 1.0f, 0.0f, 0.0f };
	   glColor3fv(textcolor);
	   glRasterPos2i(100, 5);
	   ostrstream s;
	   switch (valEAX)
	   {
	   	 case 0:
	   	   s << "EAX: GENERIC" << ends;
	   	   break;
	   	 case 1:
	   	   s << "EAX: PADDED CELL" << ends;
	   	   break;
	   	 case 2:
	   	   s << "EAX: ROOM" << ends;
	   	   break;
	   	 case 3:
	   	   s << "EAX: BATHROOM" << ends;
	   	   break;
	   	 case 4:
	   	   s << "EAX: LIVING ROOM" << ends;
	   	   break;
	   	 case 5:
	   	   s << "EAX: STONE ROOM" << ends;
	   	   break;
	   	 case 6:
	   	   s << "EAX: AUDITORIUM" << ends;
	   	   break;
	   	 case 7:
	   	   s << "EAX: CONCERT HALL" << ends;
	   	   break;
	   	 case 8:
	   	   s << "EAX: CAVE" << ends;
	   	   break;
	   	 case 9:
	   	   s << "EAX: ARENA" << ends;
	   	   break;
	   	 case 10:
	   	   s << "EAX: HANGAR" << ends;
	   	   break;
	   	 case 11:
	   	   s << "EAX: CARPETED HALLWAY" << ends;
	   	   break;
	   	 case 12:
	   	   s << "EAX: HALLWAY" << ends;
	   	   break;
	   	 case 13:
	   	   s << "EAX: STONE CORRIDOR" << ends;
	   	   break;
	   	 case 14:
	   	   s << "EAX: ALLEY" << ends;
	   	   break;
	   	 case 15:
	   	   s << "EAX: FOREST" << ends;
	   	   break;
	   	 case 16:
	   	   s << "EAX: CITY" << ends;
	   	   break;
	   	 case 17:
	   	   s << "EAX: MOUNTAINS" << ends;
	   	   break;
	   	 case 18:
	   	   s << "EAX: QUARRY" << ends;
	   	   break;
	   	 case 19:
	   	   s << "EAX: PLAIN" << ends;
	   	   break;
	   	 case 20:
	   	   s << "EAX: PARKING LOT" << ends;
	   	   break;
	   	 case 21:
	   	   s << "EAX: SEWER PIPE" << ends;
	   	   break;
	   	 case 22:
	   	   s << "EAX: UNDERWATER" << ends;
	   	   break;
	   	 case 23:
	   	   s << "EAX: DRUGGED" << ends;
	   	   break;
	   	 case 24:
	   	   s << "EAX: DIZZY" << ends;
	   	   break;
	   	 case 25:
	   	   s << "EAX: PSYCHOTIC" << ends;
	   	   break;    
	   }
	   textobject.PrintString(s.str());
   }
}

void AVEnvironment::Step (float angle, float distance)
{
   playerPos[0] += distance * (float) sin(angle + playerAngle);
   playerPos[2] -= distance * (float) cos(angle + playerAngle);
}

void AVEnvironment::Turn (float angle)
{
   playerAngle += angle;
}

void AVEnvironment::ChangeView(int x, int y)
{
   width = x;
   height = y;
   glViewport (0, 0, (GLsizei) x, (GLsizei) y);
}

void AVEnvironment::NewHouse(float x, float z)
{
	HouseList.New("House.mdl", 0.0f, x, 0.0f, z, 0.0f, 0.0f, 0);
}

void AVEnvironment::NewRadarBeam(float x, float y, float z)
{
	RadarBeamList.New("RadarBeam.mdl", 0.0f, x, y, z, 0.0f, 90.0f, 0);
}

void AVEnvironment::NewVan(float x, float z, int handle)
{
	VanList.New("Van.mdl", 0.0f, x, 0.0f, z, 12.0f, 0.0f, handle);
}

void AVEnvironment::NewCar(float x, float z, int handle)
{
	CarList.New("Car.mdl", 0.0f, x, 0.0f, z, 16.0f, 0.0f, handle);
}

void AVEnvironment::SetFPS(float floatval)
{
	// average FPS figure over 10 frames
	fpsSum += (int) floatval;
	numSamples++;
	if (numSamples == 10)
	{
		fps = fpsSum / 10;
		fpsSum = 0;
		numSamples = 0;
	}
}

void AVEnvironment::ToggleFPS()
{
	if (displayFPS == true) 
	{
		displayFPS = false;
	} else
	{
		displayFPS = true;
	}
}

void AVEnvironment::IncrementEnv()
{
    valEAX = Audio.IncrementEnv();
    if (valEAX > 0) { displayEAX = true; }    
}
