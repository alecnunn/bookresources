;demo09-06.bb - Demonstrates ImagesOverlap()
Graphics 800,600

;Set up backbuffer, random number generator, and AutoMidHandle
SetBuffer BackBuffer()
SeedRnd MilliSecs()
AutoMidHandle True

;CONSTANTS
;The key code constants
Const UPKEY = 200, DOWNKEY = 208, LEFTKEY = 203, RIGHTKEY = 205

;How many pixels the ship should move per frame
Const MOVEX = 5
Const MOVEY = 5


;TYPES
;The enemy ship is a randomly moving object on the screen
Type enemyship
	Field x,y    ;the x and y coordinates
	Field xv,yv    ;The velocity
	Field image   ;the image
End Type

;The playership type defines the player
Type playership
	Field x,y   ;The x and y coordinate position
	Field collisions    ;How many collisions have occured
	Field image    ;The player's image
End Type


;Create the enemy and assign it default variables
Global enemy.enemyship = New enemyship
enemy\x = 400
enemy\y = 200
enemy\xv = Rand(-5,5)
enemy\yv = Rand(-5,5)
enemy\image = LoadImage("enemyship.bmp") 

;Create player and assign it default variables
Global player.playership = New playership
player\x = 400
player\y = 400
player\collisions = 0
player\image = LoadImage("ship.bmp")


;MAIN LOOP
While Not KeyDown(1)
;Clear the screen
Cls

;Make sure all text appears in top left corner

Text 0,0, "Collisions: " + player\collisions

;Find out if hit a wall
TestEnemyCollisions()

;Test keyboard
TestKeys()

;If player and enemy collide, increment collisions and reset player and enemy
If (ImagesCollide(player\image,player\x,player\y,0,enemy\image,enemy\x,enemy\y,0))
	player\collisions = player\collisions + 1
	player\x = 400
	player\y = 400
	enemy\x = 400
	enemy\y = 200
EndIf

;Move the enemy
enemy\x = enemy\x + enemy\xv
enemy\y = enemy\y + enemy\yv

;Draw the player and the enemy
DrawImage enemy\image,enemy\x,enemy\y
DrawImage player\image,player\x,player\y	

;Slow it down
Delay 20

Flip

Wend
;END OF MAIN LOOP

;FUNCTION TestEnemyCollisions() - Finds out if enemy hit a wall and does something about it
Function TestEnemyCollisions()

;If enemy goes too far to the left or right, flip its x velocity
If enemy\x < 0
	enemy\xv = -enemy\xv
EndIf
If enemy\x > 800
	enemy\xv = - enemy\xv
EndIf

;If enemy goes too high or too low, flip its y velocity 
If enemy\y < 0 
	enemy\yv = -enemy\yv
EndIf
If enemy\y > 600
	enemy\yv = -enemy\yv
EndIf

End Function 
;END OF TestEnemyCollisions()

;FUNCTION TestKeys() - Tests all of the keys to see if they were hit
;No input parameters
;No return value
Function TestKeys()

;If up is hit, move player up
If KeyDown(UPKEY) 
	player\y = player\y - MOVEY 
EndIf

;If down is hit, move player down
If KeyDown(DOWNKEY) ;If down was hit
	player\y = player\y + MOVEY 
EndIf

;If left is hit, move player left
If KeyDown(LEFTKEY) 
	player\x = player\x - MOVEX 
EndIf

;If right is hit, move player right
If KeyDown(RIGHTKEY) 
	player\x = player\x + MOVEX
EndIf

End Function
;END TestKeys()