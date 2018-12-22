;demo07-13.bb - Demonstrates a moving background
Graphics 800,600

;Draw everything to backbuffer
SetBuffer BackBuffer()

;IMAGES
;Load background image
backgroundimage = LoadImage("stars.bmp")

;Set up scrolling tracker variable
scrolly = 0

;MAIN LOOP
While Not KeyDown(1)

;Tile the background at the y position of scrolly
TileBlock backgroundimage,0,scrolly

;Scroll the background a bit by incrementing scrolly
scrolly=scrolly+1

;If scrolly gets to big, reset it to zero
If scrolly >= ImageHeight(backgroundimage)
	scrolly = 0
EndIf


Flip
Wend
;END OF MAIN LOOP