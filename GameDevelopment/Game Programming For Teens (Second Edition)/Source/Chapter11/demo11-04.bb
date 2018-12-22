;demo11-04.bb - Demonstrates SoundPan

Graphics 800,600

;Set up backbuffer and AutoMidHandle
SetBuffer BackBuffer()
AutoMidHandle True

;TYPES
;This type is used for the enemy. He only moves left and right, so there is no need for a y velocity variable
Type enemy
	Field x,y   ;what is enemy's x coord?
	Field xv   ;How fast is he moving?
	Field image    ;The enemy's image
End Type

;This type is used for the player
Type player
	Field x, y   ;The player's coordinate position
	Field image   ;The player's image
End Type

;Create player and enemy
player.player = New player
enemy.enemy = New enemy

;Set up player's beginning values
player\x = 400
player\y = 400
player\image = LoadImage("spaceship.bmp")

;Set up enemy's beginning values
enemy\x = 400
enemy\y = 200
enemy\xv = 5    ;Move the player 5 pixels left/right each frame
enemy\image = LoadImage("enemyship.bmp")

;SOUNDS
;This is the bullet sound
bulletsound = LoadSound("zing.wav")

;CONSTANTS
;These constants refer to the key codes
Const ESCKEY = 1, SPACEBAR = 57

;IMAGES
;The scrolling background image
backgroundimage = LoadImage("stars.bmp")

;Set up scrolling variable
scrolly = 0

;Set up panning indicator
pan# = 0


;MAIN LOOP
While Not KeyDown(ESCKEY)


;Tile the background

TileBlock backgroundimage, 0, scrolly
;increment the scrolling variable
scrolly = scrolly + 1

If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf

;Print all text at top-left corner
Text 0,0, "Panning variable: " + pan#

;set up player coordinates	
player\x = MouseX()
player\y = MouseY()




;if enemy is to the left of player, make sound come out of left speaker
If enemy\x < player\x
	pan# = -1.000
	;If enemy is to right of player, make sound come out of right speaker
ElseIf enemy\x > player\x
	pan# = 1.000
	;If enemy is in front of player
Else
	pan# = 0
EndIf

;Pan the sound
SoundPan bulletsound, pan#


;If player presses spacebar, play the sound
If KeyHit (SPACEBAR)	


	PlaySound bulletsound
EndIf

;Move the enemy according to his velocity
enemy\x = enemy\x + enemy\xv

;If the enemy goes offscreen, reflect his velocity
If enemy\x < 0 Or enemy\x > 800
	enemy\xv = - enemy\xv
EndIf 

;Draw the player and the enemy
DrawImage player\image,player\x,player\y
DrawImage enemy\image,enemy\x,enemy\y

;Slow it down a little
Delay 20

Flip


Wend