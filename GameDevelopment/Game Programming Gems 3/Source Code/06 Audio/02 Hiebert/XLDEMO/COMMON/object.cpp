/****************************************************************************

   PROGRAM: object.cpp

   DESCRIPTION: object class code

****************************************************************************/
#include "object.h"

// OBJECT

// constructor
Object::Object ()
{
   PolyList = NULL;
   LastPoly = NULL;
   TransValue.x = 0;
   TransValue.y = 0;
   TransValue.z = 0;
   RotationValue.theta = 0;
   RotationValue.x = 0;
   RotationValue.y = 0;
   RotationValue.z = 1;
   Velocity.x = 0;
   Velocity.y = 0;
   Velocity.z = 0;
   AngularVelocity = 0;
   audioHandle = 0;
}

// destructor
Object::~Object ()
{
   KillPolyList();
}

// SetAudioObject
void Object::SetAudioObject (AudioEnvPtr pointer)
{
	audioObject = pointer;
}

// adds element to polygon list
void Object::AddPolyList(int function, int shape, float fv1, float fv2, float fv3, float fv4)
{
   Poly* tempPoly = new(Poly);
   tempPoly->function = function;
   tempPoly->shape = shape;
   tempPoly->val1 = fv1;
   tempPoly->val2 = fv2;
   tempPoly->val3 = fv3;
   tempPoly->val4 = fv4;
   tempPoly->nextPoly = NULL;

   // attach to current list (must be in-order)
   if (PolyList == NULL) {
      PolyList = tempPoly;
   } else
   {
      LastPoly->nextPoly = tempPoly;
   }
   LastPoly = tempPoly;
}

// clears polygon list
void Object::KillPolyList()
{
   Poly* tempPoly;
   
   while (PolyList != NULL) { // clear out list elements
      tempPoly = PolyList;
      PolyList = PolyList->nextPoly;
      delete(tempPoly);
   }
   PolyList = NULL;
   LastPoly = NULL;
}

// Init
void Object::Init (char* FileName)
{
   //init PolyList
   KillPolyList();
   
   ifstream InputFile(FileName);
   assert(InputFile);

   const int BUFSIZE = 256;
   char buf[BUFSIZE];
   
   while (InputFile.getline(buf, BUFSIZE)) {
      istrstream line(buf, BUFSIZE);
      char function[BUFSIZE];
      char throwaway[BUFSIZE];
      char strval[BUFSIZE];
      float fval1, fval2, fval3, fval4;
      line >> function;
      line >> throwaway;
      if (strcmp(function, "shape") == 0) {
	      line >> strval;
	      fval1 = fval2 = fval3 = fval4 = 0.0f;
         if (strcmp(strval, "QUADS") == 0) {
            AddPolyList(SHAPE, QUADS, fval1, fval2, fval3, fval4);
         }
         if (strcmp(strval, "TRIANGLES") == 0) {
            AddPolyList(SHAPE, TRIANGLES, fval1, fval2, fval3, fval4);
         }
      } else {
	      strval[0] = 0;
	      line >> fval1 >> throwaway >> fval2 >> throwaway >> fval3;
         fval4 = 0.0f;
         if (strcmp(function, "color3") == 0) {
            AddPolyList(COLOR3, 0, fval1, fval2, fval3, fval4);
         }
         if (strcmp(function, "color4") == 0) {
            line >> throwaway >> fval4;
            AddPolyList(COLOR4, 0, fval1, fval2, fval3, fval4);
         }
         if (strcmp(function, "normal") == 0) {
            AddPolyList(NORMAL, 0, fval1, fval2, fval3, fval4);
         }
         if (strcmp(function, "vertex") == 0) {
            AddPolyList(VERTEX, 0, fval1, fval2, fval3, fval4);
         }

      }   
   }
}

// Translate
void Object::Translate(float x, float y, float z)
{
	TransValue.x = x;
	TransValue.y = y;
	TransValue.z = z;
}

// Rotate
void Object::Rotate (float theta, float x, float y, float z)
{
	RotationValue.theta = theta;
	RotationValue.x = x;
	RotationValue.y = y;
	RotationValue.z = z;
}

// SetVelocity
void Object::SetVelocity(float x, float y, float z)
{
	Velocity.x = x;
	Velocity.y = y;
	Velocity.z = z;
}

// SetAngularVelocity
void Object::SetAngularVelocity(float theta)
{
	AngularVelocity = theta;
}

// Draw
void Object::Draw ()
{
   Poly* tempPoly;
   GLfloat rgb[4];
   GLfloat spec[4] = { 0.4f, 0.4f, 0.4f, 1.0f };

   tempPoly = PolyList;
   
   glPushMatrix();
   glTranslatef (TransValue.x, TransValue.y, TransValue.z);
   glRotatef (RotationValue.theta, RotationValue.x, RotationValue.y, RotationValue.z);
   glBegin(GL_QUADS);
   while (tempPoly != NULL) {
      switch (tempPoly->function) {
         case SHAPE:
               glEnd();
               switch (tempPoly->shape) {
                  case QUADS:
                     glBegin(GL_QUADS);
                     break;
                  case TRIANGLES:
                     glBegin(GL_TRIANGLES);
                     break;
               }
            break;
         case COLOR3:
            glColor3f(tempPoly->val1, tempPoly->val2, tempPoly->val3);
            rgb[0] = tempPoly->val1;
            rgb[1] = tempPoly->val2;
            rgb[2] = tempPoly->val3;
            rgb[3] = 1.0f;
    		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgb);
    		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgb);
    		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
            break;
         case COLOR4:
            glColor4f(tempPoly->val1, tempPoly->val2, tempPoly->val3, tempPoly->val4);
            rgb[0] = tempPoly->val1;
            rgb[1] = tempPoly->val2;
            rgb[2] = tempPoly->val3;
            rgb[3] = tempPoly->val4;
    		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgb);
    		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgb);
    		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
            break;
         case NORMAL:
            glNormal3f(tempPoly->val1, tempPoly->val2, tempPoly->val3);
            break;
         case VERTEX:
            glVertex3f(tempPoly->val1, tempPoly->val2, tempPoly->val3);
            break;
      }
      tempPoly = tempPoly->nextPoly;
   }
   glEnd();
   glPopMatrix();
}

// Move
void Object::Move (float timediff)
{
	TransValue.x += timediff * Velocity.x;
	if (TransValue.x > 300.0) { TransValue.x = -300.0f; }
	TransValue.y += timediff * Velocity.y;
	if (TransValue.y > 300.0) { TransValue.y = -300.0f; }
	TransValue.z += timediff * Velocity.z;
	if (TransValue.z > 300.0) { TransValue.z = -300.0f; }
	RotationValue.theta += timediff * AngularVelocity;
	if (RotationValue.theta > 360.0) { RotationValue.theta = 0.0f; }

	if ((audioHandle > 0) && (audioObject != NULL))
	{
		ALfloat sourcePos[3];
		sourcePos[0] = TransValue.x;
		sourcePos[1] = TransValue.y;
		sourcePos[2] = TransValue.z;
		audioObject->SetSourcePosition(audioHandle, sourcePos);
	}
}

// AttachSource
void Object::AttachSource(ALuint handle)
{
	audioHandle = handle;
}


// OBJECTLIST

// constructor
ObjectList::ObjectList ()
{
   pList = NULL;
}

// destructor
ObjectList::~ObjectList ()
{
	ObjectListElement * pTemp;

	while (pList != NULL)
	{
		pTemp = pList->nextObj;
		delete(pList);
		pList = pTemp;
	}
}

// New -- creates new object in list
void ObjectList::New (char* FileName, float theta, float x, float y, float z, float v, float av, int handle)
{
	ObjectListElement * pNewObject;
	pNewObject = new(ObjectListElement);

	pNewObject->nextObj = pList;
	pList = pNewObject;
	pNewObject->Obj.Init(FileName);
	pNewObject->Obj.Translate(x, y, z);
	pNewObject->Obj.Rotate(theta, 0.0f, 1.0f, 0.0f);
	pNewObject->Obj.SetVelocity(0.0f, 0.0f, v);
	pNewObject->Obj.SetAngularVelocity(av);
	pNewObject->Obj.AttachSource(handle);
}

// Draw -- draws all objects in list
void ObjectList::Draw ()
{
	ObjectListElement * pTemp = pList;

	while (pTemp != NULL)
	{
		pTemp->Obj.Draw();
		pTemp = pTemp->nextObj;
	}
}


// Move -- moves all objects in list
void ObjectList::Move (float timediff)
{
	ObjectListElement * pTemp = pList;

	while (pTemp != NULL)
	{
		pTemp->Obj.Move(timediff);
		pTemp = pTemp->nextObj;
	}
}

