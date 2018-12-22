/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_INPUT_H_
#define _BB_INPUT_H_

#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE


bool isButtonDown(BB_INPUT_BUTTON btn);
bool isButtonUp(BB_INPUT_BUTTON btn, bool wasDown);
void GetMousePosition(int *mx, int *my);


END_ENGINE_NAMESPACE

#endif
