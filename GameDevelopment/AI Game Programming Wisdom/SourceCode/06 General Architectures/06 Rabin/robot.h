/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "statemch.h"


class Robot : public StateMachine
{
public:

	Robot( GameObject * object )
		: StateMachine( object ) {}
	~Robot( void ) {}


private:

	virtual bool States( StateMachineEvent event, MSG_Object * msg, int state );

	int m_timer;

};


#endif	// __ROBOT_H__