/*********************************************************************
 * UI.h
 * Authored by Kris Hauser 2002-2003
 *
 * Defines a generic interface from GLUT into a framework program.
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/


#include "viewport.h"


struct BaseMenu;

struct DeformMain
{
  DeformMain();
  ~DeformMain();

  void InitViewports();
  void Update();
  void Draw();
  void Reset();
  
  void NextMenu();
  void PrevMenu();

  void NextTool();
  void PrevTool();
  void Click(int button, int state);
  void Nav(bool up, bool down, bool left, bool right);
  void Drag(int button, float x, float y);
  void MoveTo(float x, float y);
  void Rotate(float x, float y);
  void Zoom(float z);
  void Key(unsigned char c);
  
  //HLIGHTSYSTEM light_system;
  editor_viewport viewport;

  BaseMenu* current_menu;
  int i_current_menu;
 
  int mouse_x, mouse_y;
  int mouse_dx, mouse_dy;
  int mouse_ddx, mouse_ddy; 

  float rotation_dx, rotation_dy;
  vec3_t rotation_d;
  vec3_t rotation_dd;
};
