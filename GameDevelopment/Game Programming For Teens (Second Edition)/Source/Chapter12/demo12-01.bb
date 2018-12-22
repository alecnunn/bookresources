;demo12-01.bb - Demonstrates random variables
Graphics 800,600

;Set up automidhandle and backbuffer
AutoMidHandle True
SetBuffer BackBuffer()

;Make sure we seed the random genereator
SeedRnd MilliSecs()

;Now we load the image that we will use.
flyimage = LoadAnimImage ("fly.bmp",64,64,0,4)
;create a starting frame value
frame = 0

;create the x and y values for the fly
flyx = 400
flyy = 300

;MAIN LOOP
While Not KeyDown (1)

;Clear the screen
Cls

Text 0,0,"Fly X: " + flyx
Text 0,20,"Fly Y: " + flyy

;move the fly a random amount
flyx = flyx + Rand(-15,15)
flyy = flyy + Rand(-15,15)

;Draw the fly on screen
DrawImage flyimage,flyx,flyy,frame

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
Delay 25

Wend   ;END OF MAIN LOOP