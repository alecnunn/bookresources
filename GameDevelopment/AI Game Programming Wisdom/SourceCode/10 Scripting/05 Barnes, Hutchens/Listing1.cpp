/* Copyright (C) Jonty Barnes and Jason Hutchens, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jonty Barnes and Jason Hutchens, 2001"
 */

// A simple script written in the Challenge programming language.

begin script SickCow(Target)
	Cow = create ANIMAL COW at position of hand
Start
	make evil spirit point to Cow
	say "This unhealthy beast is on its last legs!"
	move Cow to {Target}
	begin loop
		wait 3 seconds
		HEALTH of Cow -= 0.1
		if {Cow} near {Target} radius 15
			say "It's almost there! Hurry up and die, bovine!"
		elsif HEALTH of Cow < 0.5 and HEALTH of Cow > 0.3
			say "It's almost dead! Yippee!"
		elsif Cow is FLYING
			say "Is it a bird? Is it a plane? No. It's a cow."
		end if
	until {Cow} at {Target}
		say "It made it to safety. How unfortunate!"
	until HEALTH of Cow <= 0
		say "Ha! It's steak for dinner!"
	end loop
	send evil spirit home
end script SickCow
