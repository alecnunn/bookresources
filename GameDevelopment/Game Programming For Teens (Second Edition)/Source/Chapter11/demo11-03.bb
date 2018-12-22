;demo11-03.bb - Demonstrates SoundVolume

Graphics 800,600

;Create Backbuffer() and set up AutoMidHandle
SetBuffer BackBuffer()
AutoMidHandle True

;TYPES
;This type is used for the ship that controls the player
Type player
	Field x,y    ;The player's x and y coordinates
	Field image    ;The image of the player's ship
End Type

;This type is used for the enemy
Type enemy
	Field x,y     ;The enemy's coordinate position
	Field xv,yv   ;The enemy's velocity
	Field image   ;The image of the enemy's ship
End Type

;Create the player and the enemy
Global player.player = New player
Global enemy.enemy = New enemy

;Choose the player's x and y coords
player\x = 400
player\y = 400

;Assign the player's image
player\image = LoadImage("spaceship.bmp")

;Choose the enemy's beginning x and y coords
enemy\x = 400
enemy\y = 200

;Give the enemy a velocity that is between -7 and 7
enemy\xv = Rand (-7,7)
enemy\yv = Rand (-7,7)

;Assign the enemy's image
enemy\image = LoadImage("enemyship.bmp")
	

;Make sure that neither the x nor the y velocity is 0
While enemy\xv = 0 Or enemy\yv = 0
	enemy\xv = Rand(-7,7)
	enemy\yv = Rand(-7,7)
Wend


;CONSTANTS
;The following constants are used for key codes
Const ESCKEY = 1,SPACEBAR = 57, UPKEY = 200, DOWNKEY = 208

;SOUNDS
;The following sound is produced each time the player hits space bar
Global bulletsound = LoadSound("zing.wav")

;Create a volume tracker variable and set it to half volume (.5)
Global volume# = .5

;Synchronize the sound with the volume
SoundVolume bulletsound, volume#

;Create a distance variable
Global dist

;MAIN LOOP
While Not KeyDown(ESCKEY)
;Clear the screen
Cls

;Make sure print commands occur in top left hand corner
Text 0,0,"Current Volume (between 0 and 1.000): " + volume#
Text 0,12,"Distance between enemy and player (in pixels) at time of last bullet: " + dist


;Test if any keys have been pressed
TestKeys()

;Check to see if enemy hit any walls
TestBoundaries()

;Move the enemy according to his velocity
enemy\x = enemy\x + enemy\xv
enemy\y = enemy\y + enemy\yv

;Set player\x to the current mouve position
player\x = MouseX()
player\y = MouseY()

;Draw the player and enemy images
DrawImage player\image,player\x,player\y
DrawImage enemy\image,enemy\x,enemy\y

;Slow it down
Delay 20
Flip

Wend
;END OF MAIN LOOP


;FUNCTIONS
;Function TestKeys() - Tests the keyboard to see what has been pressed
Function TestKeys()


;Create a new bullet if spacebar is hit
If KeyHit(SPACEBAR)
	
	;Find what volume# should be
	FindCorrectVolume()
	
	;Assign bulletsound to volume#
	SoundVolume bulletsound, volume#
	
	;Play the bullet
	PlaySound bulletsound
EndIf


;Move player left if left arrow is hit
If KeyDown (LEFTKEY)
	player\x = player\x - 5
EndIf

;Move player right if right arrow is hit
If KeyDown (RIGHTKEY)
	player\x = player\x + 5
EndIf

;Move player up if up arrow is hit
If KeyDown (UPKEY)
	player\y = player\y - 5
EndIf

;Move player down if down arrow is hit
If KeyDown (DOWNKEY)
	player\y = player\y + 5
EndIf

End Function


;Function FindCorrectVolume - Sets volume# to the correct value depending on distance from player to enemy
Function FindCorrectVolume()

;Find distance between player and enemy
dist = Distance(player\x,player\y,enemy\x,enemy\y)

;Assign the volume number to volume# depending on how far the distance is. The farther the distance, the quieter the sound
If dist < 100
	volume# = 1.000
ElseIf dist < 200
	volume# = .700
ElseIf dist < 300
	volume# = .400
ElseIf dist < 400
	volume# = .1000
Else
	volume# = 0.000
EndIf



End Function
;Function Distance()
;Returns the distance between two points
;x1: x coord of first point
;y1: y coord of first point
;x2: x coord of second point
;y2: y coord of second point
Function Distance(x1,y1,x2,y2)

;Find difference between x's and y's
dx = (x2 - x1)
dy = (y2 - y1)

;Find the actual distance using the distance formula
dist = Sqr( (dx * dx) + (dy * dy) )


;Send the result back to the calling function
Return dist

End Function


;Function TestBoundaries() - Tests enemy to see if it has hit any walls

Function TestBoundaries()

;If enemy has gone either too left or two right, make him move the opposite direction
;For example, if the enemy has gone off screen to the right (x > 800), make him begin to move left
	If enemy\x < 0 Or enemy\x > 800
		enemy\xv = -enemy\xv
	EndIf
	
;Flip enemy in the opposite direction if he goes too far up or down
	If enemy\y <0 Or enemy\y > 600
		enemy\yv = -enemy\yv
	EndIf
End Function