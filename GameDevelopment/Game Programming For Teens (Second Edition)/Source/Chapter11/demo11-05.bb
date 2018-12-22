;demo11-05.bb - Demonstrates channels

Graphics 800,600

;Set Up backbuffer and AutoMidHandle
SetBuffer BackBuffer()
AutoMidHandle True

;CONSTANTS
;These are the key code constants
Const UPKEY = 200, DOWNKEY = 208, AKEY = 30, ZKEY = 44, LEFTKEY = 203, RIGHTKEY = 205, PKEY = 25, RKEY = 19, SKEY = 31, ENTERKEY = 28

;IMAGES
;The background stars image
backgroundimage = LoadImage("stars.bmp")

;Play the music file and save its channel
backgroundmusic = PlayMusic("Interim Nation - Human Update.mp3")

;Create starting variables for pitch, volume, and pan.
pitch = 22000
volume# = .500
pan# = 0.00

;Initialize the pitch, volume, and panning effect of the music
ChannelPitch backgroundmusic, pitch
ChannelVolume backgroundmusic, volume#
ChannelPan backgroundmusic, pan#


;MAIN LOOP
While Not KeyDown(1)

;Tile the background image on the screen
TileBlock backgroundimage, 0, scrolly

;Scroll the background a tiny bit
scrolly = scrolly + 1

;If the scrolly variable is too large, reset the scrolly variable
If scrolly > ImageHeight (backgroundimage)
	scrolly = 0
EndIf

;Make sure text appears in top left corner of screen
Text 0,0,"This program allows you to adjust the background music that is playing right now."
Text 0,12,"Press Up or Down to increase or decrease the pitch. Pitch = " + pitch
Text 0,24,"Press A or Z to increase or decrease the volume. Volume = " + volume#
Text 0,36,"Press the Left or Right Arrow to change the pan amount. Pan = " + pan#
Text 0,48,"Press 'P' to Pause the song, 'R' to resume the song, or 'S' to stop the song."
Text 0,60,"Press Enter to begin the song from the beginning if it stops playing."

;If the up key is pressed, increase the pitch by 1000 hz
If KeyDown(UPKEY)
	pitch = pitch + 1000
	
	;Do not let pitch grow too large
	If pitch >= 44000  ;44000 is the max pitch
		pitch = 44000
	EndIf
	
EndIf

;If down key is pressed, decrease pitch by 1000 hz
If KeyDown(DOWNKEY)
	pitch = pitch - 1000

	;Do not let pitch grow too small
	If pitch <= 0
		pitch = 0
	EndIf
	
EndIf

;If 'A' is pressed, increase volume by .1
If KeyDown(AKEY)
	volume# = volume# + .1

	;Do not let volume get too large
	If volume# >= 1    ;1 is the max volume
		volume# = 1
	EndIf
EndIf

;If 'Z' is pressed, decrease volume by .1
If KeyDown(ZKEY)
	volume# = volume# - .1

	;Do not let volume get too small
	If volume# <= 0    ;1 is the max volume
		volume# = 0
	EndIf
EndIf

;If Left Arrow is pressed, change pan by -.1 (move it left by .1)
If KeyDown(LEFTKEY)
	pan# = pan# - .1
	
	;Don't let pan go too far left
	If pan# <= -1
		pan# = -1    ;-1 is the lowest pan can be
	EndIf
EndIf

;If Right Arrow is pressed, change pan by .1 (move pan right by .1)
If KeyDown(RIGHTKEY)
	pan# = pan# + .1
	
	;Don't let pan go too far left
	If pan# >= 1
		pan# = 1    ;1 is the highest pan can be
	EndIf
EndIf

;If 'P' is pressed, pause the channel
If KeyDown(PKEY)
	PauseChannel backgroundmusic
EndIf

;If 'R' is pressed, resume the channel
If KeyDown(RKEY)
	ResumeChannel backgroundmusic
EndIf

;If stop is pressed, stop the channel
If KeyDown(SKEY)
	StopChannel backgroundmusic
EndIf

;If Enter is pressed, test if the channel is playing. If it is, disregard enter, otherwise begin the song from the beginning
If KeyDown(ENTERKEY)
	
	;If channel is not playing, reset the song from the beginning
	If Not ChannelPlaying(backgroundmusic)
		backgroundmusic = PlayMusic("Interim Nation - Human Update.mp3")
	EndIf
EndIf

;Synchronize the pitch, volume and pan with the music
ChannelPitch backgroundmusic, pitch
ChannelVolume backgroundmusic, volume#
ChannelPan backgroundmusic, pan#

Delay 25
Flip
Wend