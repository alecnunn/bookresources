;demo10-10.bb - Demonstrates use of MouseZ()

;Set up graphics and backbuffer
Graphics 800,600
SetBuffer BackBuffer()


;Load images
backgroundimage = LoadImage("stars.bmp")
shipimage = LoadImage("ship.bmp")

;MAIN LOOP
While Not KeyDown(1)

;Scroll the background 20 pixels with each mouse wheel scroll
scrolly = MouseZ() * 20

;Tile the background
TileBlock backgroundimage,0,scrolly


;Draw the player
DrawImage shipimage, MouseX(), MouseY()

Flip
Wend
;END OF MAIN LOOP
	