;demo12-02.bb - Demonstrates random variables
Graphics 800,600

;Set up automidhandle and backbuffer
AutoMidHandle True
SetBuffer BackBuffer()

;Make sure we seed the random generator
SeedRnd MilliSecs()


;CONSTANT
;this constant regulates how long it takes before the fly changes directions
Const CHANGEDIRECTIONS = 1500    ;the fly changes every 1.5 seconds

;The fly type
Type fly
	Field x,y  ;the coordinate position
	Field xv,yv   ;the fly's velocity
	Field image  ;The fly's image
End Type


;let's create the fly
fly.fly = New fly

;Start the fly in the center of the screen
fly\x = 400
fly\y = 300

;Give the fly a random velocity
fly\xv = Rand(-15,15)
fly\yv = Rand(-15,15)

;Now we load the fly image 
fly\image = LoadAnimImage ("fly.bmp",64,64,0,4)

;create a starting frame value
frame = 0

;Create starting timer
timerbegin = MilliSecs()

;Create a variable that says the timer does not need to be reset
timeractive = 1

;MAIN LOOP
While Not KeyDown (1)

;Clear the screen
Cls

Text 0,0,"Fly X: " + flyx
Text 0,20,"Fly Y: " + flyy
Text 0,40, "Current time remaining on timer: " + ( CHANGEDIRECTIONS - MilliSecs() + timerbegin )



;If the counter has run through, update the fly's velocities
If MilliSecs() >= timerbegin + CHANGEDIRECTIONS

	;move the fly a random amount
	fly\xv = fly\xv + Rand(-10,10)
	fly\yv = fly\yv + Rand(-10,10)

	;make sure timer is reset
	timeractive = 0
EndIf

;If the timer is inactive, reset the timer
If timeractive = 0
	timerbegin = MilliSecs()
	timeractive = 1
EndIf




;Move the fly
fly\x = fly\x + fly\xv
fly\y = fly\y + fly\yv

;Test if fly hit any walls
If fly\x <= 0 Or fly\x > 800
	fly\xv = -fly\xv
EndIf

If fly\y <= 0 Or fly\y >= 600
	fly\yv = - fly\yv
EndIf 

;Draw the fly on screen
DrawImage fly\image,fly\x,fly\y,frame

;increment the frame
frame = frame + 1

;If frame gets too large or small, reset it
If frame > 3
	frame = 0
ElseIf frame < 0
	frame = 3 
EndIf

;Flip the buffers
Flip

;Wait a little bit
Delay 75

Wend   ;END OF MAIN LOOP