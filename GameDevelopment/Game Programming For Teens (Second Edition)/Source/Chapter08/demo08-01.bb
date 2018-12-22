;demo08-01.bb - A moving static image

Graphics 800,600

;Make back buffer default and set automidhandle to true
SetBuffer BackBuffer()
AutoMidHandle True

;IMAGES
;Load the image that will be drawn on screen
playerimage = LoadImage("staticboy.bmp")

;TYPES
;This type defines the coordinate position of the player
Type player
	Field x,y 
End Type

;Create the player
player.player = New player 

;Set up beginning values for player
player\x = 400
player\y = 300


;MAIN LOOP
While Not KeyDown(1) ;While user does not press Esc

;Clear the screen
Cls

;Print text
Text 0,0, "X Coordinate: " + player\x
Text 0,12, "Y Coordinate: " + player\y

;If player presses left, move bitmap left
If KeyDown (203) 
	player\x = player\x - 5
EndIf

;If player presses left, move bitmap right
If KeyDown(205)
	player\x = player\x + 5
EndIf

;If player presses up, move bitmap up
If KeyDown (200) 
	player\y = player\y -5
EndIf

;If player presses down, move bitmap down
If KeyDown (208)
	player\y = player\y + 5
EndIf

;Draw the player on screen
DrawImage playerimage, player\x,player\y

Flip

;Slow it down a little
Delay 50

Wend
;END OF MAIN LOOP