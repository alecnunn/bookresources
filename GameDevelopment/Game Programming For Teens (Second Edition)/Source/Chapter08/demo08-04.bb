;demo08-04.bb - Demonstrates sprite movement

Graphics 800,600
;Set up backbuffer and automidhandle
SetBuffer BackBuffer()
AutoMidHandle True

;CONSTANTS
;These constants define the direction that is begin faced
Const DIRECTIONLEFT = 1     ;When direction is left
Const DIRECTIONUP = 2     ;When direction is up
Const DIRECTIONRIGHT = 3     ;When direction is right
Const DIRECTIONDOWN = 4     ;When direction is down

;These constants define how many pixels are moved per frame
Const MOVEX = 5     ;How many pixels moved left/right per frame?
Const MOVEY = 5     ;How many pixels moved up.down per frame?


;These are key code constants
Const LEFTKEY = 203, UPKEY = 200, RIGHTKEY = 205, DOWNKEY = 208


;TYPES
;The player type is used for the character on the screen
Type player
	Field x,y    ;The coordinate position
	Field direction    ;The direction that is being faced (one of the DIRECTIONXXX constants)
	Field frame    ;The frame that should be drawn
	Field image    ;The image that should be drawn
End Type

;Create the player
player.player = New player 

;Give the player starting variables
player\x = 400 
player\y = 300 
player\direction = DIRECTIONLEFT 
player\frame = 0 
;Load the player's image
player\image = LoadAnimImage("monkeyanim.bmp",48,40,0,8) 



;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls

;Print player info
Text 0,0, "Player X: " + player\x
Text 0,12,"Player Y: " + player\y
Text 0,24,"Player Direction: " + player\direction
Text 0,36,"Frame: " + player\frame

;If player hits left, move him left, and find the correct direction and frame
If KeyDown(LEFTKEY) 
	player\x = player\x - MOVEX     ;Move him left 
	player\direction = DIRECTIONLEFT     ;face him left 
	player\frame = (player\frame + 1 )Mod (2) + (2 * (player\direction)-2)     ;find frame

;If player hits up, move him up, and find the correct direction and frame
ElseIf KeyDown(UPKEY) 
	player\y = player\y - MOVEY     ;Move him up
	player\direction = DIRECTIONUP     ;face him up
	player\frame = (player\frame + 1 )Mod (2) + (2 * (player\direction)-2)     ;find frame
	
;If player hits right, move him right, and find the correct direction and frame
ElseIf KeyDown(RIGHTKEY) 
	player\x = player\x + MOVEX     ;move him right
	player\direction = DIRECTIONRIGHT     ;face him right
	player\frame = (player\frame + 1 )Mod (2) + (2 * (player\direction)-2)     ;find frame

;If player hits down, move him down, and find the correct direction and frame
ElseIf KeyDown(DOWNKEY)
	player\y = player\y + MOVEY     ;Move him down
	player\direction = DIRECTIONDOWN     ;face him down
	player\frame = (player\frame + 1 )Mod (2) + (2 * (player\direction)-2)     ;find frame
EndIf

;Draw the player at correct position and frame
DrawImage player\image,player\x,player\y, player\frame 

;wait a (fraction of a) sec
Delay 50 

Flip
Wend
;END OF MAIN LOOP