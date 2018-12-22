;demo07-14.bb - A Parallaxing Program
Graphics 800,600

;Set AutoMidhandle to true and draw everything to back buffer
AutoMidHandle True
SetBuffer BackBuffer()

;IMAGES
;The close and quickly scrolled background
backgroundimageclose = LoadImage("stars.bmp")

;The further and slowly scrolled background
backgroundimagefar = LoadImage("starsfarther.bmp")

;Create scrolling tracker variable
scrolly = 0

;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls

;Tile both backgrounds at proper speed
TileImage backgroundimagefar,0,scrolly
TileImage backgroundimageclose,0,scrolly*2

;Increment scrolly 
scrolly=scrolly+1

;Reset tracker variable if it gets too large
If scrolly >= ImageHeight(backgroundimageclose)
	scrolly = 0
EndIf



Flip
Wend
;END OF MAIN LOOP