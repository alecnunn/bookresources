/*
    ParamListGL
    - class derived from ParamList to do simple OpenGL rendering of a parameter list
    sgg 8/2001
*/

#ifndef PARAMGL_H
#define PARAMGL_H

#pragma warning(disable:4786)   // No stupid debug warnings

#include "param.h"
#include "glh_linear.h"

using namespace glh;

class ParamListGL : public ParamList {
public:
  ParamListGL(char *name = "");

  void Render(int x, int y);
  void Mouse(int x, int y);
  void Motion(int x, int y);

private:
  int bar_x;
  int bar_w;
  int bar_h;
  int text_x;
  int separation;
  int value_x;
  int font_h;
  int start_x, start_y;
  int bar_offset;

  vec3f text_col_selected;
  vec3f text_col_unselected;
  vec3f bar_col_outer;
  vec3f bar_col_inner;

  void *font;
};

#endif
