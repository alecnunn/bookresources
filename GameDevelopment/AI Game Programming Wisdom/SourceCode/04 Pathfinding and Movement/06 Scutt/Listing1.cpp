/* Copyright (C) Tom Scutt and Core Design Ltd, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Scutt and Core Design Ltd, 2001"
 */

// Swarm update. (Code reproduced by kind permission of Core Design Ltd.)

void UpdateAgents()
{
	AGENT_STRUCT	*agent;
	long	h, i, angle, dx, dy, dz, oldx, oldy, oldz;

	agent = &agents[0];

	for (i=0;i<MAX_AGENTS;i++,agent++)
	{
		//for each agent, check to see if it's active 
		if (agent->active)
		{
			//remember its current position
			oldx = agent->xpos;
			oldy = agent->ypos;
			oldz = agent->zpos;

			//update x and z pos depending on y-rotation and speed
			agent->xpos += agent->speed * SIN(agent->yrot);
			agent->zpos += agent->speed * COS(agent->yrot);

			//update y depending on fallspeed
			agent->ypos += agent->fallspeed;

			//update fallspeed by adding gravity
			agent->fallspeed += GRAVITY;

			//calculate the difference between the agent's 
			//position and the target position 
			dz = target->zpos - agent->zpos;
			dy = target->ypos - agent->ypos;
			dx = target->xpos - agent->xpos;

			//work out the *relative* angle of the target to the agent
			//if it's time for the agent to flee, reverse the angle
			//It's a good idea to initialize agents with the timer 
			//set to a small random value. That way, the swarm will 
			//gradually disperse over time.
			if (agent->timer < (RAT_LIFE_TIME - RAT_FLEE_TIME))
				angle = ATAN(dz, dx) - agent->yrot;
			else 
				angle = agent->yrot - ATAN(dz, dx);

			//It wasn't done here, but one could influence 
			//the swarm depending on the way the player is facing.
			//One could also extrapolate player movement to set up the 
			//player's likely future position as the target

			//Hit the player if within range 
			//(using your favored collision method)
			//Here: a simple collision box.
			if (abs(dz) < AGENT_HIT_RANGE && abs(dy) < AGENT_HIT_RANGE 
				&& abs(dx) < AGENT_HIT_RANGE)
				{target->hp-=AGENT_DAMAGE;}

			if (!agent->falling) 
			// steer because not the agent is not falling 
			{
				//if the target is outside your set range, 
				//go (roughly) towards it
				if (abs(dz) + abs(dx) > SWARM_RANGE) 
				{
					//increase speed to the maximum for this creature
					//give each creature a slightly different maximum
					//(done here by & the index with 0x1f)

					if (agent->speed < 24 + (i & 0x1f))
						agent->speed++;

					if (abs(angle) < AGENT_LOCK_ANGLE)
					{
						//vary the steering each cycle - more organic
						agent->yrot += (global_counter - i) << 3;
					}
					else if (angle < 0)
						agent->yrot -= AGENT_TURN_SPEED<<1;
					else
						agent->yrot += AGENT_TURN_SPEED<<1;
				}
				else
				//if the target is roughly within a short range, 
				//swirl around it
				{
					if (agent->speed & 0x1)
						agent->yrot += AGENT_TURN_SPEED;
					else
						agent->yrot -= AGENT_TURN_SPEED;

					//play around with the numbers in this next line 
					//to get a variety of different behaviors
					agent->speed = 48 - (abs(angle) >> 10);

				}
			}
			//end of steering

			h = FloorHeight(agent->x_pos,agent->y_pos,agent->z_pos);
			if (h < agent->ypos - AGENT_MAX_STEP)
			//If the agent's new position would be much higher 
			//than its current position, bounce it off the wall 
			//by resetting its position and changing its angle
			{
				//if the agent hits a wall while it's fleeing, 
				//deactivate it. This looks fine with small creatures
				if (agent->timer > (AGENT_LIFE_TIME - AGENT_FLEE_TIME))
					agent->active = 0;

				if (angle > 0)
					agent->yrot += 180_DEGREES;
				else
					agent->yrot -= 180_DEGREES;
				agent->xpos = oldx;
				agent->ypos = oldy;
				agent->zpos = oldz;
				agent->fallspeed = 0;
			}
			//If the agent's new position would take it onto a 
			//block of climbable height, angle its body upward,
			//reset its x and z position and move it upwards
			else if (h < agent->ypos - AGENT_IGNORE_STEP)
			{
				fx->pos.x_rot = AGENT_CLIMB_ANGLE;
				fx->pos.x_pos = oldx;
				fx->pos.y_pos = oldy - AGENT_CLIMB_SPEED;
				fx->pos.z_pos = oldz;
				fx->fallspeed = 0;
			}
     		else if (agent->ypos > h)
			//the agent is below the floor surface, so move it up to //the floor, reset its fallspeed and set falling to 0. //This happens every cycle the agent isn't in the air //(as we always try to change the y-pos due to gravity)
			{
				agent->ypos = h;
				agent->fallspeed = 0;
				agent->falling = 0;
			}

			if (agent->fallspeed < AGENT_MAX_FALLSPEED && 
				agent->timer < AGENT_LIFE_TIME)
			//tilt things as they fall
				agent->xrot = -(agent->fallspeed << 6);
			else
			//if they fall too fast or time's run out, kill them
				next_agent = agent->active = 0;

			agent->timer++;

		}
	}
}
