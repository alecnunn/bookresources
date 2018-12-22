/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
/////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////

#ifndef BUMPOBJECT_H
#define BUMPOBJECT_H

#include "pof.h"
#include "pofObject.h"

class BumpObject : public pofObject{
  private:
  protected:
  public:
    BumpObject();
    virtual ~BumpObject();

    virtual void loadObject(char *fn);
    void computeLighting(GLfloat lightpos[3], GLuint forCubemap);
    virtual void renderObject(GLuint forCubemap, GLuint pass);
    virtual void renderVectors(GLfloat sf, GLuint coordFlag, GLuint lightFlag, 
                               GLuint forCubemap);
};

#endif
