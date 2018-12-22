;demo09-03.bb - Draws a bounding box

Graphics 800,600

;Set default backbuffer and automidhandle to true
SetBuffer BackBuffer()
AutoMidHandle True

;IMAGES
;Load the ship image
Global shipimage = LoadImage("ship.bmp")

;Give the ship default parameters
Global x = 400
Global y = 300



;CONSTANTS
;The key code constants
Const UPKEY = 200, DOWNKEY = 208, LEFTKEY = 203, RIGHTKEY = 205 

;These constants define how many pixels are moved per frame
Const MOVEX = 5
Const MOVEY = 5

;MAIN LOOP
While Not KeyDown(1)
;Clear the screen
Cls

;Find out if any important keys on the keyboard have been pressed
TestKeys() 

;Draw the bounding box around the player
DrawPlayerRect() 

;Draw the image of the ship
DrawImage shipimage,x,y 

Flip

;Slow it down
Delay 20

Wend
;END OF MAIN LOOP


;FUNCTION DrawPlayerRect() - Draws a bounding rectangle
Function DrawPlayerRect()

;find the width of the image
iw = ImageWidth(shipimage)

;Find the upper left hand coordinates
x1# = ((-ImageWidth(shipimage)/2) +x)
y1# = ((-ImageHeight(shipimage)/2) + y)

;Draw the entire bounding box
Rect x1#,y1#,ImageWidth(shipimage),ImageHeight(shipimage), 0

End Function


;FUNCTION TestKeys() - Tests all of the keys to see if they were hit
Function TestKeys()

;If up is hit, move player up
If KeyDown(UPKEY) 
	y = y - MOVEY 
EndIf

;If down is hit, move player down
If KeyDown(DOWNKEY) ;If down was hit
	y = y + MOVEY 
EndIf

;If left is hit, move player left
If KeyDown(LEFTKEY) 
	x = x - MOVEX 
EndIf

;If right is hit, move player right
If KeyDown(RIGHTKEY) 
	x = x + MOVEX 
EndIf

End Function