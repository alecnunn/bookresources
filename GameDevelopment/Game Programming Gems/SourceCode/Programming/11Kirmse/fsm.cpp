/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#include "fsm.h"

/***************************************************************/
FSM::FSM(int32 new_adder,int32 new_multiplier)
{
   state = new_adder ^ new_multiplier; 
   adder = new_adder; 
   multiplier = new_multiplier; 
}

/***************************************************************/
void FSM::UpdateState()
{
   state = ((~state) + adder) * multiplier;
   state = state ^ (state >> 16);
}
