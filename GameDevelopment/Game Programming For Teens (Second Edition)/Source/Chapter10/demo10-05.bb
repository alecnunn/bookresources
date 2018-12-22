;demo10-05.bb - A Space Simulation with KeyHit()

Graphics 800,600

;Set automidhandle to true
AutoMidHandle True
;Set up Backbuffer
SetBuffer BackBuffer()

;TYPES

;Bullet type = hold the information for each bullet
Type bullet
	Field x,y   ;the coordinates of the bullet
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
;The following constants are used for testing key presses
Const ESCKEY = 1, UPKEY = 200, LEFTKEY = 203, RIGHTKEY = 205, DOWNKEY = 208, SPACEBAR = 57



;IMAGES
playerimage = LoadImage("ship.bmp")
Global bulletimage = LoadImage("bullet.bmp")
backgroundimage = LoadImage("stars.bmp")

;Create a scrolling indicator variable
scrolly = 0

;MAIN LOOP
While Not KeyDown(ESCKEY)
	
;Increment scrolling variable
scrolly = scrolly + 1

;Tile the background
TileBlock backgroundimage,0,scrolly

;Reset the scrolling variable when it grows too large
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf

;Test input keys
TestKeys()

;Update (move) each bullet
UpdateBullets()

;Draw the player
DrawImage playerimage,  player\x, player\y

;Wait a bit
Delay 50

;Flip the front and back buffers
Flip

Wend   ;END OF MAIN LOOP


;FUNCTIONS
;Function TestKeys() - Tests what buttons have been pressed by player
Function TestKeys()

;If the player hits up, we move him 5 pixels up
If KeyDown(UPKEY)
	player\y = player\y - 5 ;move player 5 pixels up
EndIf 

;If the player hits left, we move him 5 pixels left
If KeyDown(LEFTKEY)
	player\x = player\x - 5 ;move player 5 pixels left
EndIf 

;If player hits right, we move him 5 pixels right
If KeyDown(RIGHTKEY) 
	player\x = player\x + 5 ;move player 5 pixels right
EndIf 

;If player hits down, we move him 5 pixels down
If KeyDown(DOWNKEY)
	player\y = player\y + 5 ;move player 5 pixels down
EndIf 



;If player hits spacebar, we will create a new bullet at the player's current position
If KeyHit(SPACEBAR)
	bullet.bullet = New bullet
	bullet\x = player\x
	bullet\y = player\y
EndIf

End Function

;Function UpdateBullets() - Moves each bullet on screen
Function UpdateBullets()

;For every bullet, move it up 5 pixels. If it goes offscreen, delete it, otherwise, draw it
For bullet.bullet = Each bullet
	bullet\y = bullet\y - 5   ;Move bullet up
	
	;If bullet moves offscreen, delete it, otherwise, draw it onscreen
	If bullet\y < 0
		Delete bullet
	Else	
		DrawImage bulletimage, bullet\x, bullet\y    ;Draw the bullet
	EndIf

Next  ;move to next bullet

End Function