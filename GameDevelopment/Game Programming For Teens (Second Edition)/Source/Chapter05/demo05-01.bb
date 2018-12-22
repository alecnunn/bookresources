;demo05-01.bb - Demonstrates a rotated image without AutoMidHandle
Graphics 640,480
SetBuffer BackBuffer()

;INITIALIZATION

;Defines how many rotation frames there are
Const PLAYERFRAMES = 40 
;The array that hold the rotations
Dim player(PLAYERFRAMES) 


;Load the player image
playerimage = LoadImage("player.bmp") 

;Create every frame of the rotation
For loop = 0 To PLAYERFRAMES - 1 
	
	;copy the image (with no rotation) to the array
	player(loop) = CopyImage(playerimage) 
	
	;Rotate the image For later use
	RotateImage player(loop), loop*360/PLAYERFRAMES

Next ;Next frame
;END INITIALIZATION

;GAME LOOP
While(Not KeyDown(1))

	;Clear the screen
	Cls 
	;go to next frame 
	currentframe = currentframe + 1 


	;if you reach the last frame start from the beginning
	If currentframe > PLAYERFRAMES - 1 
		currentframe = 0 
	EndIf

	;draw the rotated player
	DrawImage(player(currentframe),320,240) 


	;slow it down a bit
	Delay 50 
	Flip 
	
Wend ;End of While Loop