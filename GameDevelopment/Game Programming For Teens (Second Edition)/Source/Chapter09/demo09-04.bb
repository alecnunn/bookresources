;demo09-04.bb - Demonstrates Bounding Rectangles
Graphics 800,600 

;Set up backbuffer and automidhandle
SetBuffer BackBuffer() 
AutoMidHandle True 

;Seed the random generator
SeedRnd (MilliSecs()) 


;CONSTANTS

;The number of objects on a screen at one time
Const NUMBEROFOBJECTS = 50 

;The key code constants
Const UPKEY = 200, DOWNKEY = 208, LEFTKEY = 203, RIGHTKEY = 205 

;How many pixels should ship move per frame
Const MOVEX = 5
Const MOVEY = 5


;TYPES

;The point type defines each object that can be hit by the ship
Type point 
	Field x,y     ;the x and y coordinate of the ship
End Type

;This type contains the player
Type player 
	Field x,y    ;the x and y coordinate of the player
	Field collisions    ;the number of collisions that have occurred
	Field image     ;the actual image of the player
End Type


;Create the player and give him beginning variables
Global player.player = New player 
player\collisions = 0 
player\image = LoadImage("ship.bmp") 

;Reset the level with new points and new coordinates
ResetLevel() 


;MAIN LOOP
While Not KeyDown(1)
;Clear the screen
Cls 

;Reset text to the top left of screen

Text 0,0, "X: " + player\x 
Text 0,12, "Y: " + player\y 
Text 0,24, "Collisions: " + player\collisions 

;Test what keys have been pressed
TestKeys()

;If there have been any collisions, incremen collision counter and reset level
If(TestCollisions() = 1) 
	player\collisions = player\collisions + 1 
	ResetLevel() 	
EndIf


;Draw each point on the screen
For point.point = Each point 
	Plot point\x, point\y 
Next

;Draw the player
DrawImage player\image,player\x,player\y 

;Wait a (fraction of a) sec
Delay 50 

Flip
Wend
;END OF MAIN LOOP


;FUNCTIONS
;FUNCTION ResetLevel() - Resets the level, creates new random points, resets the player's position
;No input parameters
;No return value
Function ResetLevel()

;Delete all remaining objects
For point.point = Each point 
	Delete point 
Next

;Create NUMBEROFOBJECTS (default 50) points and assign them random x/y coordinates
For counter = 0 To NUMBEROFOBJECTS 
	point.point = New point     ;Create the point

;Find random x and y coordinates
	point\x = Rand (0,800) 
	point\y = Rand (0,600) 
Next

;Reset the player's coordinate position
player\x = 400 
player\y = 300 
End Function


;END ResetLevel()


;FUNCTION TestCollisions() - Tests the objects and the ship for collisions
;No input parameters
;Returns 1 if there was a collision, 0 if there was none
Function TestCollisions()

;Test each point to see if it is within the player's radius
For point.point = Each point 

;Find player's bounding box
x1 = -ImageWidth(player\image)/2 + player\x
x2 = ImageWidth(player\image)/2 + player\x
y1 = -ImageHeight(player\image)/2 + player\y
y2 = ImageHeight(player\image)/2 + player\y

;If the point is within collision radius, return 1
	If (point\x > x1) And (point\x < x2) And (point\y > y1) And (point\y < y2)
		Return 1
	EndIf
Next    ;Move on to next point

;There were no collisions if the function makes it here, so return 0
Return 0 
End Function
;END TestCollisions()



;FUNCTION TestKeys() - Tests all of the keys to see if they were hit
;No input parameters
;No return value
Function TestKeys()

;If up is hit, move player up
If KeyDown(UPKEY) 
	player\y = player\y - MOVEY 
EndIf

;If down is hit, move player down
If KeyDown(DOWNKEY) ;If down was hit
	player\y = player\y + MOVEY 
EndIf

;If left is hit, move player left
If KeyDown(LEFTKEY) 
	player\x = player\x - MOVEX 
EndIf

;If right is hit, move player right
If KeyDown(RIGHTKEY) 
	player\x = player\x + MOVEX
EndIf

End Function
;END TestKeys()