/****************************************************************************

   FILE: object.h

   DESCRIPTION: header for object class

****************************************************************************/
#include "audioenv.h"

#if !defined (OBJECT_CLASS)
#define OBJECT_CLASS

#include <fstream.h>  // includes iostream.h
#ifdef WINDOWS
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <string.h>
#include <assert.h>

enum functions { SHAPE, COLOR3, COLOR4, NORMAL, VERTEX };
enum shapes { QUADS, TRIANGLES };

struct Poly {
   int function;
   int shape;
   float val1;
   float val2;
   float val3;
   float val4;
   Poly* nextPoly;
};

struct Translation {
	float x;
	float y;
	float z;
};

struct Rotation {
	float theta;
	float x;
	float y;
	float z;
};

class Object
{
private:
   Poly* PolyList;
   Poly* LastPoly;
   Translation TransValue;
   Rotation RotationValue;
   Translation Velocity;
   ALuint audioHandle;
   static AudioEnvPtr audioObject;
   float AngularVelocity;
   void AddPolyList(int function, int shape, float fv1, float fv2, float fv3, float fv4);
   void KillPolyList();
public:
   Object ();
   ~Object ();
   void SetAudioObject (AudioEnvPtr);
   void Init (char* FileName);
   void Translate(float x, float y, float z);
   void Rotate (float theta, float x, float y, float z);
   void SetVelocity(float x, float y, float z);
   void SetAngularVelocity(float theta);
   void Draw ();
   void Move (float);
   void AttachSource(ALuint handle);
};

struct ObjectListElement {
   Object Obj;
   ObjectListElement * nextObj;
};

class ObjectList
{
private:
	ObjectListElement * pList;
public:
	ObjectList ();
	~ObjectList ();
	void New (char* FileName, float theta, float x, float y, float z, float v, float av, int src);
	void Draw ();
	void Move (float);
};

#endif // !defined (OBJECT_CLASS)	

