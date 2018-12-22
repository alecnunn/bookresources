// **********************************************************************
// * FILE  : glutFunc.h
// *---------------------------------------------------------------------
// * PURPOSE : glut and GL stuff
// *
// *---------------------------------------------------------------------
// * AUTHOR   : Thomas Di Giacomo . MIRALab (giacomo@miralab.unige.ch)
// **********************************************************************
// * COPYRIGHT :
/*
 * This copyright notice must be included in all copies or derivative works.
 * (c) Copyright 2003, MIRALab, C.U.I. University of Geneva 
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of MIRALab not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL MIRALAB
 * BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT MIRALAB  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef __GLUTFUNC_H__
#define __GLUTFUNC_H__

#include "../Physics/CEnvNod.h"
#include "../Physics/CMyObj.h"
#include "../Gfx/trackball.h"

extern	CNet*		wire;
extern	CMyObj*	object;

void glutMain(int, char**);

// ------

static void	drawScene();

static void setOrthographicProjection();
static void resetPerspectiveProjection();
static void renderBitmapString(float, float, void*, char*);

static void display(void);
static void reshape(int, int);
static void keyboard(unsigned char, int, int);
static void processSpecialKeys(int, int, int);

static void processMouse(int, int, int, int);
static void processMouseActiveMotion(int, int);

#endif // __GLUTFUNC_H__
