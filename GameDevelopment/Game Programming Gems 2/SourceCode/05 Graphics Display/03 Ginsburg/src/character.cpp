/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//
//
//
//
//
////////////////////////////////////

#include <string.h>
#include "character.h"

float s_width=1.0f/40.0f;
float s_height=1.0f/20.0f;

#define OFFSET (0.2f/8.0f)

void flDrawString(float x, float y, char* str){
  unsigned char* array;
  int row,c;
  float pos=x;

  array=(unsigned char*)str;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0f,1.0f,0.0f,1.0f);

  while(*array){
    if(*array>127){
      array++;
      continue;
    }
    if(*array=='\n'){
      y-=s_height;
      x=pos;
      array++;
      continue;
    }

    row=16-(*array/8)-1;
    c=*array%8;

    glBegin(GL_QUADS);

    //bottom left corner
    glTexCoord2f(c/8.0f+OFFSET,row/16.0f);
    glVertex2f(x,y);

    //bottom right corner
    glTexCoord2f((c+1)/8.0f-OFFSET,row/16.0f);
    glVertex2f(x+s_width,y);

    //top right corner
    glTexCoord2f((c+1)/8.0f-OFFSET,(row+1)/16.0f);
    glVertex2f(x+s_width,y+s_height);

    //top left corner
    glTexCoord2f(c/8.0f+OFFSET,(row+1)/16.0f);
    glVertex2f(x,y+s_height);

    glEnd();

    array++;
    x+=s_width;
  }
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}



void flSetWidth(float width){
  s_width=width;
}

void flSetHeight(float height){
  s_height=height;
}
