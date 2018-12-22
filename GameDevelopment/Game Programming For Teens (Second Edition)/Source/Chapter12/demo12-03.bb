;demo12-03.bb - Demonstrates chasing algorithms
Graphics 800,600

;Set up backbuffer and automidhandle
SetBuffer BackBuffer()
AutoMidHandle True

;IMAGES
;player and enemies ships
playership = LoadImage ("spaceship.bmp")
enemyship  = LoadImage ("enemyship.bmp")

;Load background
backgroundimage = LoadImage ("stars.bmp")


;CONSTANTS
;The following constants are used for testing key presses
Const ESCKEY = 1, UPKEY = 200, LEFTKEY = 203, RIGHTKEY = 205, DOWNKEY = 208

;the following constants define how fast the player and the enemy move
Const PLAYERSPEED = 10
Const ENEMYSPEED = 5

;position player on bottom center of screen
playerx = 400
playery = 400

;positionenemy on upper center of screen
enemyx = 400
enemyy = 200

;set up scrolling variable
scrolly = 0

;MAIN LOOP
While Not KeyDown(ESCKEY)

;tile the background image
TileBlock backgroundimage, 0, scrolly

;move the background up a little
scrolly = scrolly + 1

;If scrolly gets too big, reset it
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf



;Test the keypresses of the player
;If the player hits up, we move him up
If KeyDown(UPKEY)
	playery = playery - PLAYERSPEED 
EndIf 

;If the player hits left, we move him left
If KeyDown(LEFTKEY)
	playerx = playerx - PLAYERSPEED
EndIf 

;If player hits right, we move him right
If KeyDown(RIGHTKEY) 
	playerx = playerx + PLAYERSPEED 
EndIf 

;If player hits down, we move him down
If KeyDown(DOWNKEY)
	playery = playery + PLAYERSPEED
EndIf 



;Now, we move the enemy depending on where the player is
;If the player is above the enemy, move the enemy up
If playery > enemyy
	enemyy = enemyy + ENEMYSPEED
EndIf

;If the player is to the left of the enemy, move the enemy left
If playerx < enemyx
	enemyx = enemyx - ENEMYSPEED
EndIf

;If the player is to the right of the enemy, move the enemy right
If playerx > enemyx
	enemyx = enemyx + ENEMYSPEED
EndIf

;if the player is below the enemy, move the enemy down
If playery < enemyy
	enemyy = enemyy - ENEMYSPEED
EndIf

;draw the player and enemy on the screen
DrawImage playership, playerx, playery
DrawImage enemyship, enemyx, enemyy


;delay for a bit
Delay 25

;Flip the front and back buffer
Flip

Wend
;END OF MAIN LOOP 













