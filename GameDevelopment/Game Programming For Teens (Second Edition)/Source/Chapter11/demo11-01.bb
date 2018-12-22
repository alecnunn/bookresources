;demo11-01.bb - Demonstrates playing a sound
Graphics 800,600


;Initialize Back Buffer and set AutoMidHandle to True
SetBuffer BackBuffer()
AutoMidHandle True


;CONSTANTS
;The following constants are scan codes for their corresponding keys
Const ESCKEY = 1, LEFTKEY = 203, RIGHTKEY = 205, UPKEY = 200, DOWNKEY = 208, SPACEBAR = 57


;Load the background image and set up a scrolling variable
backgroundimage = LoadImage("stars.bmp")
scrolly = 0

;SOUNDS
;Load the sound that is played when the player fires a bullet
Global bulletsound = LoadSound("zing.wav")

;Load the sound that is played when the player destroys the enemy
Global explosionsound = LoadSound ("explode.wav")



;TYPES
;The player type is made for the player's spaceship
Type player
	Field x,y    ;The X and Y coordinates
	Field image    ;The drawn player image
End Type

;The enemy type is the player's uhh, ship?
Type enemy
	Field x,y   ;The x and y coordinates
	Field xv,yv   ;The x and y velocites
	Field image   ;The enemy spacecraft
End Type

;The bullet type is used for each instance of the bullet
Type bullet
	Field x,y   ;The x and y coordinates
	Field image   ;The bullet image
End Type


;We now create the player and the enemy
Global player.player = New player
Global enemy.enemy = New enemy


;Reset the level (gives starting values to the player and enemy)
ResetLevel()



;MAIN LOOP
While Not KeyDown (ESCKEY)

;Draw the background of stars
TileBlock backgroundimage, 0, scrolly

;Scroll the background up
scrolly = scrolly + 1

;If the scrolling variable gets too high, reset the scrolling variable
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf

;Test to see what keys the player pressed
TestKeys()

;Test to see if the enemy hit a wall
TestBoundaries()

;Move and draw each bullet, and test for collision with enemy
UpdateBullets()

;Move enemy depending on their xv and yv velocities
enemy\x = enemy\x + enemy\xv
enemy\y = enemy\y + enemy\yv

;Draw player and enemy
DrawImage player\image, player\x, player\y
DrawImage enemy\image, enemy\x, enemy\y

Delay 20

;Flip the buffers
Flip

Wend


;END OF MAIN LOOP

;FUNCTIONS

;Function TestKeys() - Tests the keyboard to see if the player pressed any keys
Function TestKeys()

;Create a new bullet if space bar is hit
If KeyHit(SPACEBAR)
	bullets.bullet = New bullet    ;Create the bullet
	bullets\x = player\x   ;Assign bullet to player's x
	bullets\y = player\y   ;Assign bullet to player's y
	bullets\image = LoadImage("bullet.bmp")   ;Load the bullet image
	
	;Play the bullet sound
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


;Function UpdateBullets() - Moves and tests each bullet for collision
Function UpdateBullets()

;Loop through every bullet
	For bullets.bullet = Each bullet
		
		;Update the bullet's position by moving 5 pixels up
		bullets\y = bullets\y - 5
		
;Draw the bullet at its proper coordinates
		DrawImage bullets\image, bullets\x, bullets\y
		
	
;If the bullet hit the enemy, play the explosion and reset the level
		If ImagesOverlap(enemy\image,enemy\x,enemy\y,bullets\image,bullets\x,bullets\y)
			PlaySound explosionsound ;Play the explosion
			Cls
			Text 260,300, "You destroyed the enemy! How could you?"
			Flip
			Delay 4000
			ResetLevel() ;Reset all variables of the level
			Return ;Go back to main loop
		EndIf

;If the bullet goes off screen, delete it
		If bullets\y < 0
			Delete bullets
		EndIf
		
		
	Next  ;Move on to next bullet
End Function 


;Function ResetLevel() - Deletes all previous bullets and resets all type variables
Function ResetLevel()

;Delete each of the bullets (if there are any)
For bullets.bullet = Each bullet
	Delete bullets
Next
	

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
enemy\image = LoadImage("maneeshimage.bmp")
MaskImage enemy\image, 255 ,255,255
	

;Make sure that neither the x nor the y velocity is 0
While enemy\xv = 0 Or enemy\yv = 0
	enemy\xv = Rand(-7,7)
	enemy\yv = Rand(-7,7)
Wend
End Function