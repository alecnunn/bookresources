;demo07-11.bb - Demonstrates preloading and real-time rotation

Graphics 800,600
;Set up AutoMidHandle and BackBuffer()
AutoMidHandle True
SetBuffer BackBuffer()

;IMAGES
;Load the spaceship image that will be rotated
shipimage = LoadImage ("spaceship.bmp")

;CONSTANTS
;How many rotations do you want total?
Const rotations = 16

;Create the rotation array
Dim imagearray(rotations)

;For all of the rotations you want, copy the spaceship image and rotate it the correct amount of degrees
For frame = 0 To rotations - 1
	imagearray(frame) = CopyImage (shipimage)
	RotateImage imagearray(frame), frame*360/rotations
Next 




;Begin at frame 0 (facing upwards)
frame = 0

;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls

;Add Text
Text 0,0, "Press Left to rotate counter-clockwise and right to rotate clockwise,"
Text 0,20, "Press Esc to exit."

;Rotate the ship left if user presses left
If KeyDown (203)
	
;Decrement frame by 1 (thus rotating it left)
	frame = frame - 1

;If the frame count is less than 0, put it back at the max it the max value of the array
	If frame <= 0
		frame = rotations - 1
	EndIf
;Rotate the ship right if user presses right
ElseIf KeyDown (205)

;Increment frame by 1 (thus rotating it right)
	frame = frame + 1

;If frame gets too big, set it to the first frame (0)
	If frame >= rotations
		frame = 0
	EndIf 

EndIf



;Draw the current frame
DrawImage imagearray(frame), 400,300


Flip

;Wait for a while
Delay 50
Wend