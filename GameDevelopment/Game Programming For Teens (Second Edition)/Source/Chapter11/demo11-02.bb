;demo11-02.bb - Demonstrates SoundPitch

Graphics 800,600

;Make sure backbuffer is drawn on and automidhandle is true
SetBuffer BackBuffer()
AutoMidHandle True

;IMAGES
;load the player's ship image
playerimage = LoadImage ("spaceship.bmp")


;SOUNDS
;Load the bullet sound
explosionsound = LoadSound ("explode.wav")

;CONSTANTS
;The following constants are used for key codes
Const ESCKEY = 1,SPACEBAR = 57, UPKEY = 200, DOWNKEY = 208

;create hertz variable
hertz = 22000

;Make sure bullet has hertz value of hertz variable to begin with
SoundPitch explosionsound, hertz



;MAIN LOOP
While Not KeyDown(1)
;Clear the screen
Cls

;Make sure text is drawn in top left hand corner
Text 0,0, "Current Hertz Value: " + hertz

;Play explosion sound if player hits space bar
If KeyHit(SPACEBAR)
	PlaySound explosionsound
EndIf

;If up is hit, increment hertz variable
If KeyHit (UPKEY)
	hertz = hertz + 1000
EndIf

;If down is hit, decrement hertz variable
If KeyHit(DOWNKEY)
	hertz = hertz - 1000
EndIf

;Make the explosion have the same pitch as the hertz variable
SoundPitch explosionsound, hertz

;Draw the player
DrawImage playerimage, MouseX(), MouseY()

Flip
Wend


;END OF MAIN LOOP