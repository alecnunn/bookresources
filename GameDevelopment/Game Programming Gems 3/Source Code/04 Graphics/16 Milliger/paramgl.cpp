/*
    ParamListGL
    - class derived from ParamList to do simple OpenGL rendering of a parameter list
    sgg 8/2001
*/

#include "param.h"
#include "paramgl.h"

#include <GL/glut.h>
#include "glh_linear.h"

using namespace glh;

void beginWinCoords(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0.0, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0.0);
  glScalef(1.0, -1.0, 1.0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);

  glMatrixMode(GL_MODELVIEW);
}

void endWinCoords(void)
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void glPrint(int x, int y, const char *s, void *font)
{
  int i, len;

  glRasterPos2f(x, y);
  len = (int) strlen(s);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, s[i]);
  }
}

ParamListGL::ParamListGL(char *name) : ParamList(name)
{
  font = (void *) GLUT_BITMAP_9_BY_15;
//    font = (void *) GLUT_BITMAP_8_BY_13;

  bar_x = 250;
  bar_w = 250;
  bar_h = 10;
  bar_offset = 5;
  text_x = 5;
  separation = 15;
  value_x = 200;
  font_h = 15;
  start_x = 0;
  start_y = 0;

  text_col_selected = vec3f(1.0, 1.0, 1.0);
  text_col_unselected = vec3f(0.0, 0.0, 0.0);
  bar_col_outer = vec3f(0.0, 0.0, 0.0);
  bar_col_inner = vec3f(0.0, 0.0, 0.0);
}

void
ParamListGL::Render(int x, int y)
{
  beginWinCoords();
  glDisable(GL_DEPTH_TEST);

  for(std::vector<ParamBase *>::const_iterator p = m_params.begin(); p != m_params.end(); ++p) {
    if ((*p)->IsList()) {
      ParamListGL *list = (ParamListGL *) (*p);
      list->Render(x+10, y);
      y += separation*list->GetSize();

    } else {
      if (p == m_current)
        glColor3fv((GLfloat *) &text_col_selected);
      else
        glColor3fv((GLfloat *) &text_col_unselected);
      glPrint(x + text_x, y + font_h, (*p)->GetName()->c_str(), font);

      glPrint(x + value_x, y + font_h, (*p)->GetValueString().c_str(), font);

      glColor3fv((GLfloat *) &bar_col_outer);
      glBegin(GL_LINE_LOOP);
      glVertex2f(x + bar_x, y + bar_offset);
      glVertex2f(x + bar_x + bar_w, y + bar_offset);
      glVertex2f(x + bar_x + bar_w, y + bar_offset + bar_h);
      glVertex2f(x + bar_x, y + bar_offset + bar_h);
      glEnd();

      glColor3fv((GLfloat *) &bar_col_inner);
      glRectf(x + bar_x, y + bar_offset + bar_h, x + bar_x + (bar_w*(*p)->GetPercentage()), y + bar_offset);

      y += separation;
    }

  }
  glEnable(GL_DEPTH_TEST);
  endWinCoords();
}


void
ParamListGL::Mouse(int x, int y)
{
  if ((y < start_y) || (y > start_y + (separation * m_params.size()) - 1) )
    return;

  int i = (y - start_y) / separation;
  m_current = &m_params[i];
}

void
ParamListGL::Motion(int x, int y)
{
  if (x < bar_x) {
    (*m_current)->SetPercentage(0.0);
    return;
  }
    
  if (x > bar_x + bar_w) {
    (*m_current)->SetPercentage(1.0);
    return;
  }

  (*m_current)->SetPercentage((x-bar_x) / (float) bar_w);
}
