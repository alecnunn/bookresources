;demo09-05.bb - Demonstrates ImagesOverlap()
Graphics 800,600

;Set up backbuffer, random number generator, and automidhanel
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
	Field x,y
	Field xv,yv
	Field image
End Type

;The playership defines the player
Type playership
	Field x,y
	Field collisions
	Field image
End Type

;Create the enemy and assign it default and random variables
Global enemy.enemyship = New enemyship
enemy\x = 400
enemy\y = 200
enemy\xv = Rand(-5,5)
enemy\yv = Rand(-5,5)
enemy\image = LoadImage("enemyship.bmp") 

;Create the player and assign default values
Global player.playership = New playership
player\x = 400
player\y = 400
player\collisions = 0
player\image = LoadImage("ship.bmp")


;MAIN LOOP
While Not KeyDown(1)
Cls

;Make sure all text appears in top left corner

Text 0,00, "Collisions: " + player\collisions

;Find out if enemy hit a wall
TestEnemyCollisions()

;Test keyboard
TestKeys()

;If player and enemy overlap, increment collisions and reset player and enemy
If (ImagesOverlap(player\image,player\x,player\y,enemy\image,enemy\x,enemy\y))
		player\collisions = player\collisions + 1
		player\x = 400
		player\y = 400
		enemy\x = 400
		enemy\y = 200
		enemy\xv = Rand(-5,5)
		enemy\yv = Rand(-5,5)
EndIf

;Move the enemy
enemy\x = enemy\x + enemy\xv
enemy\y = enemy\y + enemy\yv

;Draw the player and the enemy
DrawImage enemy\image,enemy\x,enemy\y
DrawImage player\image,player\x,player\y	

Flip

;Slow it down
Delay 20

Wend


;FUNCTION TestEnemyCollisions() - Tests to see if enemy hit a wall
;No input parameters
;No return values
Function TestEnemyCollisions()

;If enemy hits a wall, reverse his velocity
If enemy\x < 0
	enemy\xv = -enemy\xv
EndIf
If enemy\x > 800
	enemy\xv = - enemy\xv
EndIf
If enemy\y < 0 
	enemy\yv = -enemy\yv
EndIf
If enemy\y > 600
	enemy\yv = -enemy\yv
EndIf
End Function 


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