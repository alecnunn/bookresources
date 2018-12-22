;demo07-01.bb - Demonstrates Translation
Graphics 640,480

;Set up backbuffer and AutoMidHandle
SetBuffer BackBuffer() 
AutoMidHandle True 

;IMAGES
;The image that will be drawn on screen
shipimage = LoadImage("enemy.bmp") 

;Make ship's coordinates begin at top left corner
shipx = 0
shipy = 0


;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls

;Draw the ship
DrawImage shipimage,shipx,shipy ;Draw the ship image

;Translate the ship image 7 pixels right and 5 pixels down
shipx = shipx + 7
shipy = shipy + 5

;Wait a (fraction of a ) sec
Delay 50

Flip
Wend
;END OF MAIN LOOP