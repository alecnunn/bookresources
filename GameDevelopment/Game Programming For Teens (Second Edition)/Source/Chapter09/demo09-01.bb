;demo09-01.bb - Demonstrates Pixel Collisions
Graphics 400,300

;create variables that define coordinate position of pixel
Global x = 200
Global y = 150

Cls

;This variable contains the amount of times a collision has occured
collisions = 0

;CONSTANTS
;These are the key code constants
Const UPKEY = 200, DOWNKEY = 208, LEFTKEY = 203, RIGHTKEY = 205 

;MAIN LOOP
While Not KeyDown (1)

;Print intro
Text 0,0, "Press the arrow keys to move the pixel around."

;Print the number of collisions
Text 0,12, "Collisions: " + collisions

;Move player around depending on the key he pressed
If KeyDown(UPKEY) 
	y = y - 5 
ElseIf KeyDown(DOWNKEY)
	y = y + 5 
ElseIf KeyDown(LEFTKEY)
	x = x - 5 
ElseIf KeyDown(RIGHTKEY)
	x = x + 5 
EndIf

;Call the CheckForCollisions function and determine if a collision occurred
collisions = CheckForCollisions(collisions)

;Draw the pixel on the screen
Plot x,y 

;wait a (fraction of a) sec
Delay 100 

Flip


Wend
;END OF MAIN LOOP


;FUNCTIONS

;Function CheckForCollisions(collisions) - Returns number of total collisions, tests for new ones
;collisions: the number of collisions at the time of calling the function
Function CheckForCollisions(collisions)

;If the pixel is offscreen, report a collision
If x <= 0 Or x >= 400 Or y <= 0 Or y >= 300
	collisions = collisions + 1     ;increment collisions
	Cls     ;clear the screen
	Text 100,150,"A Collision Has Occurred"
	Flip	
	Delay 1000     ;wait a sec
	Cls     ;clear screen again
	Flip
	Cls


	x = 200     ;reset x
	y = 150     ;reset y
EndIf

;return the amount of collisions
Return collisions 
Cls
End Function