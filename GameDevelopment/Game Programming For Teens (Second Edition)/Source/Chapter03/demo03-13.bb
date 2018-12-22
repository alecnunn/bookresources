;demo03-12.bb - A textdrawn arkanoid
Graphics 640,480
SetBuffer BackBuffer()

;TYPES
;the player type
Type paddle 
	Field x,y   ;coordinates
End Type

;The ball type
Type ball
	Field x,y   ;the coordinate
	Field directionx, directiony  ;the velocities of the ball
End Type
	

;Constants
;What the blocks look like
Const BLOCKSTRING$ = "XXXXXXX"
;What the paddle looks like
Const PADDLESTRING$ = "---------"
;What the ball looks like
Const BALLSTRING$ = "O"
;How many rows of blocks there are
Const BLOCKROWS = 3
;How many columns of blocks there are
Const BLOCKCOLUMNS = 6
;The number of pixels between each column
Const BLOCKXGAP = 85
;The number of pixels between each row
Const BLOCKYGAP = 32
;The number of pixels from the top left corner the first column is
Const BLOCKXORIGIN = 16
;The number of pixels from the top left corner the first row is
Const BLOCKYORIGIN = 8
;The height of each block
Global BLOCKHEIGHT = FontHeight()
;The length of each block
Global BLOCKWIDTH = Len(BLOCKSTRING$) *FontWidth()
;The width of each paddle
Global PADDLEWIDTH = Len(PADDLESTRING$) * FontWidth()
;The height of each paddle
Global PADDLEHEIGHT = FontHeight()
;The width of the ball
Global BALLWIDTH = Len(BALLSTRING$) *FontWidth()
;The height of the ball
Global BALLHEIGHT = FontHeight()
;The starting X coordinate for the player
Const STARTX = 300
;The starting Y coordinate for the player
Const STARTY= 340
;The Key code constants
Const ESCKEY = 1, LEFTKEY = 203, RIGHTKEY = 205 



;Initialization
;Seed the random Generator
SeedRnd MilliSecs()
;Initialize the score
Global score = 0
;The number of total block hits
Global blockhits = 0 
;The level the player is on
Global level = 1

;Creat an array of blocks
Dim blocks(BLOCKROWS, BLOCKCOLUMNS)

;Create a new ball
Global ball.ball = New ball
;Create a new paddle
Global player.paddle = New paddle

;Initialize the new level
NewLevel()





;Game Loop
While Not KeyDown(1)
Cls

DrawHUD()
TestInput()
DrawBlocks()
DrawPaddle()
CheckBall()




Flip

Wend

Function DrawBlocks()
	
	x = BLOCKXORIGIN
	y = BLOCKYORIGIN
;This variable creates a new level if there are no blocks
	newlevel = 0 
	
;For all the rows
	For rows = 0 To BLOCKROWS - 1
;reset rows position
		x = BLOCKXORIGIN 
		
		For cols = 0 To BLOCKCOLUMNS - 1

			;If the block exists, draw it on screen
			If (blocks(rows,cols) = 1) Then
				Text x,y, BLOCKSTRING$
				newlevel = newlevel + 1
			EndIf
		;Move over to the next block
		x = x + BLOCKXGAP
		
		Next
		;Move to the next column
		y = y + BLOCKYGAP
	Next
	If newlevel = 0
		level = level + 1
		NewLevel()
	EndIf
		
End Function

Function CheckBall()

;Move and draw ball 
UpdateBall() 

;Check and see if ball hit anything
CheckBallWithPaddle()
CheckBallWithBlocks()
CheckBallWithWalls()
End Function
			
			
Function UpdateBall()

;Move the ball to the left or right
ball\x = ball\x + ball\directionx 

;Move the ball up or down
ball\y = ball\y + ball\directiony 

;Draw the ball
Text ball\x, ball\y, BALLSTRING$  
End Function



Function DrawPaddle()
;Draw the paddle
Text player\x,player\y,PADDLESTRING$
End Function

Function CheckBallWithPaddle()
;If the ball hits a paddle, revers its direction
If ball\x >= player\x And ball\x <= player\x + PADDLEWIDTH And ball\y + BALLHEIGHT>= player\y  And ball\y + BALLHEIGHT <= player\y + PADDLEHEIGHT
	ball\directiony = -ball\directiony + Rand(-3,3)
EndIf
End Function

Function CheckBallWithBlocks()
;y is the first row
y = BLOCKYORIGIN 

For rows=0 To BLOCKROWS - 1

	;Reset x to first block of column
	x = BLOCKXORIGIN 

	;For every column of blocks
	For cols = 0 To BLOCKCOLUMNS - 1;
		
		;If it exists
		If blocks(rows,cols) 

			;If the ball hit the block, delete the block
			If ball\x >= x And ball\x <=  x + BLOCKWIDTH And ball\y >= y And ball\y <= y + BLOCKHEIGHT 

				blocks(rows,cols) = 0  ;Delete block

				ball\directiony = -ball\directiony + Rand(-2,2)  ;Reverse its direction and add randomizer		

				score = score + 75

				blockhits = blockhits + 1

				;It can't hit more than one block, so leave function
				Return 
			EndIf
		EndIf

		;move to next column
		x = x + BLOCKXGAP 
	Next
	
	;move to next row
	y = y + BLOCKYGAP
Next

End Function



Function CheckBallWithWalls()
;If ball hits the left wall, reverse its direction and add randomizer
If ball\x <= 0 
	ball\directionx = -ball\directionx + Rand(-2,2)

;If ball hits top wall, reverse its direction and add randomizer
ElseIf ball\y <= 0 
	ball\directiony = -ball\directiony + Rand(-2,2)

; If it hits right wall, reverse its direction and add randomizer
ElseIf ball\x >= 640 - BALLWIDTH 
	ball\directionx = -ball\directionx + Rand(-2,2) ;

;If ball hits lower wall, dock points for missing ball
ElseIf ball\y >= 480
	score = score - 200

	;Reset the level
	ResetLevel()
EndIf
End Function


Function TestInput()

;hit Esc
If KeyDown(ESCKEY) 

	;quit the game
	End 

 ;hit left arrow
ElseIf KeyDown(LEFTKEY)
;move paddle left
	player\x = player\x - 10 

;hit right arrow
ElseIf KeyDown(RIGHTKEY) 

	;move paddle right
	player\x = player\x + 10 
EndIf
End Function

Function ResetLevel()
ball\x = 320
ball\y = 150
ball\directiony = 4
ball\directionx = Rand(-5,5)
player\x = STARTX
player\y = STARTY
Delay 500

End Function

Function DrawHUD()
Text 0,440, "Level: " + level ;write the level
Text 0,450, "Score: " + score ;write the score
Text 0,460, "Block Hits: " + blockhits ;write the block hits
End Function

Function NewLevel()
For rows=0 To BLOCKROWS
	For cols=0 To BLOCKCOLUMNS
		;Set block to existing (1)
		blocks(rows,cols) = 1
	Next
Next
ResetLevel()
End Function