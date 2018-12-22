/****************************************************************************
 * Prim.hpp - Primitive Functions Header File V2.04
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ****************************************************************************/

#ifndef prim_h
#define prim_h

#include "letras.hpp"

/* ------------------------ Drawing Primitives ---------------------------- */

void FillScreen(pixelT color);
void putPixel(int x, int y, pixelT color);

void Hline(int x1, int x2, int y, pixelT color);
void Vline(int x, int y1, int y2, pixelT color);
void Line(int x1, int y1, int x2, int y2, pixelT color);
void Box(int x1, int y1, int x2, int y2, pixelT color);

#endif