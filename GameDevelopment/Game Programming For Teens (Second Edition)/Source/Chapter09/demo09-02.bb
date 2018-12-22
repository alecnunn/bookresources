;demo09-02.bb - Demonstrates Bounding Circles
Graphics 800,600 

;Set up back buffer and automidhandle
SetBuffer BackBuffer() 
AutoMidHandle True 

;Seed the random generator
SeedRnd (MilliSecs()) 


;CONSTANTS
;The number of collidable objects on screen
Const NUMBEROFOBJECTS = 50

;The key code constants
Const UPKEY = 200, DOWNKEY = 208, LEFTKEY = 203, RIGHTKEY = 205 ;The key code constants

;How many pixels the ship moves per frame
Const MOVEX = 5
Const MOVEY = 5

 
;TYPES

;the point type defines each object that can be hit by the ship
Type point 
	Field x,y     ;the x and y coordinate of the ship
End Type

;The player type is the space ship on the screen
Type player 

	Field x,y     ;the x and y coordinate of the player
	Field collisions     ;the number of collisions that have occurred
	Field radius      ;the radius of the player image
	Field image     ;the actual image of the player

End Type


;Create the player and define his starting variables
Global player.player = New player 
player\collisions = 0 
player\image = LoadImage("ship.bmp") 

;Find the radius of the player image
player\radius = FindRadius(player\image) 

;Reset the level and give it new points, coords, etc.
ResetLevel() 


;GAME LOOP
While Not KeyDown(1)

;Clear the screen
Cls

;Make text appear in top left hand corner

Text 0,0, "X: " + player\x     ;Write out the x coordiante of the player
Text 0,12, "Y: " + player\y     ;Write out the y coordinate of the player
Text 0,24, "Collisions: " + player\collisions     ;Write out how many collisions have occured

;Test to see if player has pressed any keys
TestKeys() 

;If there were any collisions, increment the  collision counter and reset the level
If(TestCollisions() = 1) 
	player\collisions = player\collisions + 1 
	ResetLevel() 
EndIf

;Plot every point on the screen
For point.point = Each point 
	Plot point\x, point\y 
Next

;Draw the player
DrawImage player\image,player\x,player\y

;Delay the program a few millisecs
Delay 50

Flip
Wend
;END OF MAIN LOOP


;FUNCTIONS

;FUNCTION ResetLevel() - Resets the level, creates new random points, resets the player's position
;No input parameters
;No return value
Function ResetLevel()

;Delete every point on screen
For point.point = Each point 
	Delete point 
Next

;Create NUMBEROFOBJECTS new points with random x and y coords
For counter = 0 To NUMBEROFOBJECTS 
	point.point = New point 
	point\x = Rand (0,800) 
	point\y = Rand (0,600) 
Next

;Reset the player's coordinates
player\x = 400 
player\y = 300 
End Function



;FUNCTION TestCollisions() - Tests the objects and the ship for collisions
;No input parameters
;Returns 1 if there was a collision, 0 if there was none
Function TestCollisions()

;Check every object to see if it is within player's radius. If it is, return that there was a collision.
For point.point = Each point 
	If Distance(player\x,player\y,point\x,point\y) < player\radius 
		Return 1 
	EndIf
Next

;If there was no collision, return 0
Return 0 ;There was no collision

End Function



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


;FUNCTION Distance(x1,y1,x2,y2) - Finds the distance between two points
;x1 - first point's x, y1 - first point's y, x2 - second point's x, y2 - second point's y
;Returns the distance between the two points
Function Distance(x1,y1,x2,y2)

;Find difference in x's and y's
dx = x2 - x1 
dy = y2 - y1 

;return the actual distance
Return Sqr((dx*dx) + (dy*dy)) 
End Function


;FUNCTION FindRadius(imagehandle)
;imagehandle - the image whose radius you wish to find
;Returns the radius of the image
Function FindRadius(imagehandle)

;Return the radius
Return ((ImageWidth(imagehandle)/2) + (ImageHeight(imagehandle)/2) / 2)
End Function
;END FindRadius()