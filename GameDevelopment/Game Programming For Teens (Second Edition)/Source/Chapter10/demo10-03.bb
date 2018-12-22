;demo10-02.bb - Demonstrates problem with not using FlushKeys

Graphics 800,600

;Create the background image that will be scrolled
backgroundimage = LoadImage ("stars.bmp")
;Load ship image
shipimage = LoadImage ("ship.bmp")
AutoMidHandle True

;Create variable that determines how much background has scrolled
scrolly = 0

;MAIN LOOP
While Not KeyDown(1)

;Scroll background a bit by incrementing scrolly
scrolly = scrolly + 1

;Tile the background
TileBlock backgroundimage,0,scrolly

;Reset scrolly if the number grows too large
If scrolly > ImageHeight(backgroundimage)
	scrolly = 0
EndIf

;Print necessary text
Text 0,0,"When you want to quit, press Esc."
Text 0,12,"Hopefully, a message stating 'Quitting' will appear after you hit Esc."

;Draw ship
DrawImage shipimage,400,300

;Delay the program for a fraction of a second 
Delay 25
Flip


Wend ;END OF MAIN LOOP


Text 0,24,"Quitting..."

Flip
Delay 1000
Flip
