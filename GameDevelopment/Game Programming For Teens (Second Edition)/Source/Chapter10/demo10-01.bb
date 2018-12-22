;demo10-01.bb - Demonstrates usage of KeyDown()

;Initalize Graphics
Graphics 800,600

;Load the background image
backgroundimage = LoadImage("stars.bmp")

;CONSTANTS
;The following constants are used for testing key presses
Const ESCKEY = 1, UPKEY = 200, LEFTKEY = 203, RIGHTKEY = 205, DOWNKEY = 208

;scrollx and scrolly define how much the image should be moved
scrollx = 0
scrolly = 0


;MAIN LOOP
While Not KeyDown(ESCKEY)

;If the player hits up, we will scroll the background up
If KeyDown(UPKEY)
	scrolly = scrolly - 5 ;scroll background 5 pixels up
EndIf ;End of UPKEY test

;If the player hits left, we will scroll the background left
If KeyDown(LEFTKEY)
	scrollx = scrollx - 5 ;scroll background 5 pixels left
EndIf ;End LEFTKEY test

;If player hits right, we will scroll the background right
If KeyDown(RIGHTKEY) 
	scrollx = scrollx + 5 ;scroll background 5 pixels right
EndIf ;End RIGHTKEY test

;If player hits down, we will scroll the background down
If KeyDown(DOWNKEY)
	scrolly = scrolly + 5 ;Scroll background 5 pixels down
EndIf ;End of DOWNKEY test

;Tile the background image on the screen so it looks like actual outer space
TileBlock backgroundimage,scrollx,scrolly

;Wait a fraction of a second.
Delay 35
Flip

Wend ;END OF MAIN LOOP