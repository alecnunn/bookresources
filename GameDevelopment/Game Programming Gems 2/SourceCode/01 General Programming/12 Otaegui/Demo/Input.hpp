/* Copyright (C) Javier F. Otaegui, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Javier F. Otaegui, 2001"
 */
#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "blitbuf.hpp"

/* Datos Globales */
extern int g_MouseButton, g_MouseX, g_MouseY;
extern int g_oldMouseButton;		// Datos de la posicion anterior
extern int g_oldMouseX, oldMouseY;
extern int g_ShiftStatus;
extern blitbuf *g_MouseBackground;

/* Funciones exportables */
void MouseEvent(void);
void DisplayMouse (blitbuf *buf, unsigned short x, unsigned short y);
BOOL DIInit( HWND hwnd, HINSTANCE g_hinst );
void DITerm(void);

#endif