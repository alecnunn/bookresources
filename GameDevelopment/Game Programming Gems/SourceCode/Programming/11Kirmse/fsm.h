/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#ifndef _FSM_H
#define _FSM_H

#include "defs.h"

class FSM
{
public:
   
   FSM(int32 new_adder,int32 new_multiplier);
   void SetState(int32 new_state) { state = new_state; }
   int32 GetState() { return state; }
   void UpdateState();

private:

   int32 state;
   int32 adder;
   int32 multiplier;
};

#endif
