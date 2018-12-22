;demo03-11.bb - Draws a ship which can be moved and killed

Graphics 400,300

;CONSTANTS
;starting hitpoints, feel free To change
Const STARTHITPOINTS = 3 ;
;What the ship looks like
Const SHIP$ = "<-*->" 
;the keycodes
Const ESCKEY = 1, SPACEBAR = 57, UPKEY = 200, LEFTKEY = 203, DOWNKEY = 208, RIGHTKEY = 205 
; Where the player starts
Const STARTX = 200, STARTY = 150


;TYPES
Type Ship
	Field x,y ;the position of the ship
	Field hitpoints ;how many hits left
	Field shipstring$ ;what the ship looks like
End Type

;INITIALZATION SECTION
Global cont = 1 ;continue?
Global player.ship = New ship
player\x = STARTX
player\y = STARTY
player\hitpoints = STARTHITPOINTS
player\shipstring = SHIP$

;Game loop
While cont = 1
	Cls
	Text player\x, player\y, player\shipstring$
	
	TestInput()
	DrawHUD()
	Flip
Wend
;End of loop
	
	
	
;TestInput() changes the direction or hitpoints of the player
Function TestInput()

;If player presses left, move him left.
If KeyHit(LEFTKEY)

	player\x = player\x - 3
	If player\x <= 0
		player\x = 10
	EndIf
EndIf

;If player presses right, move him right.
If KeyHit(RIGHTKEY)
	player\x = player\x + 3

	If player\x >= 385
		player\x = 380
	EndIf
EndIf

;If player presses up, move him up.
If KeyHit(UPKEY)

	player\y = player\y - 3
	If player\y <= 0
		player\y = 10
	EndIf
EndIf

;If player presses down, move him down.
If KeyHit(DOWNKEY)

	player\y = player\y + 3
	If player\y >= 285
		player\y = 280
	EndIf
EndIf

;If player presses space bar, remove a hitpoint

If KeyHit(SPACEBAR)

	player\hitpoints = player\hitpoints - 1
	If player\hitpoints <= 0
		cont = 0
	EndIf

EndIf

;If player presses Esc, set cont to 0, and exit the game
If KeyHit(ESCKEY)
	cont = 0
EndIf

End Function

;DrawHUD() draws user's info in top Right of screen
Function DrawHUD()
	Text 260, 10, "X position: " + player\x
	Text 260, 20, "Y position: " + player\y
	Text 260, 30, "Hitpoints:  " + player\hitpoints
End Function 