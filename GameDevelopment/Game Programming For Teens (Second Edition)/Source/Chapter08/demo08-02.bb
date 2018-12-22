;demo08-02.bb - A moving animated image

Graphics 800,600

;Set up BackBuffer() and AutoMidHandle
SetBuffer BackBuffer()
AutoMidHandle True

;IMAGES
;Load the animated image of the boy
playerimage = LoadAnimImage("animatedboy.bmp",95,71,0,8)

;TYPES
;Load the player type
Type player
	Field x,y    ;The x and y coordinate position
	Field frame  ;The frame that should be drawn
End Type

;Create the player
player.player = New player 

;Give the player its starting values
player\x = 400
player\y = 300
player\frame = 0

;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls


;Position text at the top left corner of the screen
Text 0,0, "X Coordinate: " + player\x
Text 0,12, "Y Coordinate: " + player\y


;If player presses left, move him left and decrement the frame number
If KeyDown (203)
	player\x = player\x - 5
	player\frame = player\frame - 1

EndIf

;If player presses right, move him right and increment the frame number
If KeyDown(205)
	player\x = player\x + 5
	player\frame = player\frame + 1
EndIf

;If player presses up, move him up and increment the frame number
If KeyDown (200) 
	player\y = player\y -5
	player\frame = player\frame + 1
EndIf

;If player presses down, move him down and decrement the frame number
If KeyDown (208)
	player\y = player\y + 5
	player\frame = player\frame - 1
EndIf

;If the frame gets too high, reset it back to zero.
If player\frame > 7
	player\frame = 0
	
;If the frame gets too low, reset it to 3
ElseIf player\frame < 0
	player\frame = 7
EndIf

;Draw the player at the correct position and the correct frame
DrawImage playerimage, player\x,player\y, player\frame

;Wait a while
Delay 100
Flip
Wend
;END OF MAIN LOOP