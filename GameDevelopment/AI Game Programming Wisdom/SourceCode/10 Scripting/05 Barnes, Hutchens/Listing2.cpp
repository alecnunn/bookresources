/* Copyright (C) Jonty Barnes and Jason Hutchens, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jonty Barnes and Jason Hutchens, 2001"
 */

// A complete cinematic sequence introducing the
// "Lost Brother" Challenge.

begin cinema
	// Create a moody storm
	start music MOODY_PIANO
	Storm = create WEATHER DRIZZLE at {Brother}

	// Move the sister out with a new walking animation
	move Sister position to {SoapBox}
	set Sister anim DESPAIR_WALK

	// Set the camera to dynamically follow the sister
	set camera focus follow Sister
	move camera position to {SoapBox}+{-3,4,-3} time 10

	// When the sister nears her soapbox, make her pray
	wait until {Sister} near {SoapBox} radius 1
	Sister play PRAY loop 1
	wait 4 seconds

	// Sister: "Oh holy one, I kneel before you."
	say LOST_BROTHER_LINE1
	wait until camera ready

	// Zoom the camera in on the sister
	move camera position to {Sister}+{-2,1,-2} time 8
	move camera focus to {Sister}+{0,0.7,0} time 8

	// Wait until she's said her line and stopped praying
	wait until read and Sister played

	// Turn the sister to face her brother and point at him
	set Sister focus to {SickBed}
	Sister play POINTING loop 1 

	// Sister: "My brother ... [has] ... become lost."
	say LOST_BROTHER_LINE2

	// Move camera behind the sister, wait until done pointing
	move camera position to {1703,18,2592.5} time 5
	move camera focus to {Sister} time 8
	wait until read and Sister played
	wait until camera ready
	Sister play DESPAIR_STAND loop 1

	// Fade to black
	set fade red 0 green 0 blue 0 time 1.5
	wait until fade ready

	// Move the camera along a pre-defined path and fade in
	move camera to TRACK00_START time 3
	set fade in time 2			
	camera path TRACK00			
	set Brother focus to {BrotherCamPos}
	set Sister position to {SoapBox}
	wait 1 second

	// Make the brother suffer (Brother: "Ugh.")
	Brother play POISONED loop 1
	start sound GROAN1 at {Brother}

	// Sister: "He is so weak, I'm frightened he'll die!"
	say single line LOST_BROTHER_LINE3
	wait until read
	wait until camera ready

	// Emphasize his sickness (Brother: "Eueergh!")
	move camera position to {Brother}+{2,0.3,-2} time 4
	move camera focus to {Brother} time 3
	wait 2 seconds
	start sound GROAN2 at {Brother}

	// Make the brother look like he is dying
	Brother play DYING loop -1

	// Cut back to crying sister
	set camera position to {1703,18,2592.5}
	set camera focus to {Brother}

	// Sister: "If you find him I'll give you the gate stone."
	say single line LOST_BROTHER_LINE4
	Sister play MOURNING loop 1
	wait 1 seconds

	// Focus the camera on the stormy mountain
	move camera position to {1725.8,28,2576} time 8
	move camera focus to {1667,17.2,2589} time 8
	wait until read
	wait until camera ready
	stop music

	// Get the advisor spirits on screen
	eject good spirit
	eject evil spirit

	// Evil: "I gotta plan, lets trash the house ..."
	make evil spirit point at {House}
	say LOST_BROTHER_LINE5
	wait until read	
	stop evil spirit pointing

	// Good: "I heartily object. That's just malicious."
	say LOST_BROTHER_LINE6
	wait until read

	// Evil: "That's kinda the whole point!"
	say LOST_BROTHER_LINE7
	wait until read

	// Send the advisor spirits away
	send good home
	send evil home
end cinema

