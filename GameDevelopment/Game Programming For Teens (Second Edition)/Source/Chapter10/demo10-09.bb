;demo10-09.bb - A Space Simulation with MouseDown() and KeyDown()

Graphics 800,600

;Set automidhandle to true
AutoMidHandle True
;Set up Backbuffer
SetBuffer BackBuffer()

;TYPES

;Bullet type = hold the information for each bullet
Type bullet
	Field x,y   ;the coordinates of the bullet
	Field bullettype ;LASER or NORMALBULLET (see constants)
End Type


;Player type - holds the actual player
Type player
	Field x,y   ;the coordinates of the player
End Type

;Create player and initialize field
Global player.player = New player
player\x = 400
player\y = 500


;CONSTANTS
;The following constants are used for testing key presses (mouse and keyboard)
Const ESCKEY = 1, LEFTMOUSEBUTTON = 1, RIGHTMOUSEBUTTON = 2
;The following constants are used for the bullets, BULLET is a regular bullet, LASER is a laser
Const NORMALBULLET = 1, LASER = 2



;IMAGES
playerimage = LoadImage("ship.bmp")
Global bulletimage = LoadImage("bullet.bmp")
Global laserimage = LoadImage("laser.bmp")
backgroundimage = LoadImage("stars.bmp")

HandleImage laserimage, ImageWidth(laserimage)/2, ImageHeight(laserimage)


;VARIABLES
;Create a scrolling indicator variable
scrolly = 0

;Number of times left and mouse buttons were hit
Global leftmouseclicks = 0
Global rightmouseclicks = 0

;MAIN LOOP
While Not KeyDown(ESCKEY)
	
;Increment scrolling variable
scrolly = scrolly + 1

;Tile the background
TileBlock backgroundimage,0,scrolly

;set up text
Text 0,0,"Player X: " + MouseX()
Text 0,12,"Player Y: " + MouseY()
Text 0,24,"Number of times left mouse button was hit: " + leftmouseclicks
Text 0,36,"Number of times right mouse button was hit: " + rightmouseclicks

;Reset the scrolling variable when it grows too large
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf

;Test mouse buttons
TestMouse()

;Update (move) each bullet
UpdateBullets()

;Draw the player
DrawImage playerimage,  player\x, player\y


;Slow it down
Delay 20

;Flip the front and back buffers
Flip

Wend   ;END OF MAIN LOOP


;FUNCTIONS
;Function TestMouse() - Tests what mouse buttons have been pressed and where player is located
Function TestMouse()



;set the player at the position of the mouse
player\x = MouseX()
player\y = MouseY()

;If the player hits left mouse button, create a bullet
If MouseHit(LEFTMOUSEBUTTON)
	bullet.bullet = New bullet    ;create bullet
	bullet\x = player\x    ;place bullet at player's x coordinate
	bullet\y = player\y    ;place bullet at player's y coordinate
	bullet\bullettype = NORMALBULLET    ;make it a normal bullet
	
	;increment left mouse clicks
	leftmouseclicks = leftmouseclicks + 1
EndIf 

;If the player hits left, we will scroll the background left
If MouseDown(RIGHTMOUSEBUTTON)
	bullet.bullet = New bullet    ;create bullet
	bullet\x = player\x    ;place bullet at player's x coordinate
	bullet\y = player\y    ;place bullet at player's y coordinate
	bullet\bullettype = LASER    ;make it a laser
	
	;add amount of right mouse clicks since last frame
	rightmouseclicks = rightmouseclicks + MouseHit(RIGHTMOUSEBUTTON)
EndIf 
End Function




;Function UpdateBullets() - Moves each bullet on screen
Function UpdateBullets()

;For every bullet, move it up 5 pixels. If it goes offscreen, delete it, otherwise, draw it
For bullet.bullet = Each bullet

	
	;If bullet moves offscreen, delete it, otherwise, draw it onscreen. Draw laserimage if it is a laser, bulletimage if it is a bullet
	If bullet\y < 0
		Delete bullet
	ElseIf bullet\bullettype = NORMALBULLET	
		bullet\y = bullet\y - 5   ;Move bullet up
		DrawImage bulletimage, bullet\x, bullet\y    ;Draw the bullet
	ElseIf bullet\bullettype = LASER
		If player\x <> bullet\x
			Delete bullet
		Else
			DrawImage laserimage, bullet\x, bullet\y     ;Draw the laser
		EndIf
	EndIf

Next  ;move to next bullet

End Function