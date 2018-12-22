;demo10-04.bb - Demonstrates movement with KeyHit()

Graphics 800,600

;Set up page flipping
SetBuffer BackBuffer()

;Create the background image that will be scrolled
backgroundimage = LoadImage ("stars.bmp")
playerimage = LoadImage("ship.bmp")

;Create variable that determines how much background has scrolled
scrolly = 0

;CONSTANTS
;The following constants are used for testing key presses
Const ESCKEY = 1, UPKEY = 200, LEFTKEY = 203, RIGHTKEY = 205, DOWNKEY = 208

;Create player's x any y coordinates - center him
x = 400
y = 300


;MAIN LOOP
While Not KeyDown(ESCKEY)

;Scroll background a bit by incrementing scrolly
scrolly = scrolly + 1

;Tile the background
TileBlock backgroundimage,0,scrolly

;Reset scrolly if the number grows too large
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf


;If the player hits up, move player up
If KeyHit(UPKEY)
	y = y - 5   ;move player 5 pixels up
EndIf

;If the player hits left, move player left
If KeyHit(LEFTKEY)
	x = x - 5   ;move player 5 pixels left
EndIf

;If player hits right, move player right
If KeyHit(RIGHTKEY) 
	x = x + 5   ;move player 5 pixels right
EndIf

;If player hits down, move player down
If KeyHit(DOWNKEY)
	y = y + 5   ;move player 5 pixels down
EndIf 

;draw the player at his proper coordinates on the screen
DrawImage playerimage, x,y

;Wait a bit
Delay 50

;Flip the front and back buffers
Flip

Wend ;END OF MAIN LOOP