;demo01-01.bb - A Complete game of KONG

;Set up graphics mode
Graphics 800,600

;Seed the random generator (make random numbers actually random)
SeedRnd(MilliSecs()) 

;Create a back buffer
SetBuffer BackBuffer()
;Set the handle to the center of images
AutoMidHandle True  


;CONSTS
;The following are key code constants
Const UPKEY = 200   ;Up
Const DOWNKEY = 208  ;Down
Const PAUSEKEY = 25  ;P


Const HUMANSPEED = 7 ;The human's max speed
Const COMPUTERSPEED = 6 ;The computer's max speed


;TYPES
;The player type: both the human and the opponent
Type player 
	Field y,score     ;y position and score
End Type

;The ball type: for the ball
Type ball 
	Field x,y,xv,yv     ;x, y coordinates, and x, y velocity
End Type

;IMAGES
;The picture of the human player
Global player1image = LoadImage("player1.bmp")   

;The picture of the computer player
Global player2image = LoadImage("player2.bmp") 

;The picture of the ball
Global ballimage = LoadImage("ball.bmp") ;Load the ball image

;TYPE INITIALIZATION

;Create a ball
Global ball.ball = New ball 
;Create the human
Global player1.player = New player 
;Create the computer
Global player2.player = New player 


;INITIALIZATION

Text 400,300,"Ready...Set"
;Wait for one second
Delay(1000) 
Text 420,330,"GO!!!"
Flip 
;Delay for 1/5 of a second
Delay(200) 

;Initialize the level
InitializeLevel() 


;Set inital scores
player1\score = 0 
player2\score = 0

;MAIN LOOP
While Not KeyDown(1)

;Clear the screen
Cls 

;Draw the ball
DrawImage (ballimage,ball\x,ball\y) 
;Draw the human
DrawImage (player1image, 60, player1\y) 
;Draw the computer
DrawImage (player2image, 740, player2\y) 

;Test what user pressed
TestKeyboard() 
;What should AI do?
TestAI() 
;Draw the HUD
DrawScore() 

Flip

Delay 20

Wend ;END OF MAIN LOOP

;FUNCTIONS
;INITIALIZELEVEL()
;Sets up starting values
Function InitializeLevel()

;Put ball in center of the screen
ball\x = 400 
ball\y = 300 

;Make the ball move in a random direction
ball\xv = Rand(2,6) 
ball\yv = Rand(-8,8)

;Place the players in their correct position
player2\y = 300 
player1\y = 300
End Function


;DRAWSCORE()
;Draws the HUD in the top right
Function DrawScore()
;Write the human score
Text 700,0,"Player 1: " + player1\score 
;Write the computer's score
Text 700,30,"Player 2: " + player2\score 
End Function

;TESTKEYBOARD()
;Moves player up and down based on keyboard
Function TestKeyboard()

;If player hits up, move him up
If KeyDown(UPKEY)
	player1\y = player1\y - HUMANSPEED 
EndIf

;If player presses down, move him down
If KeyDown(DOWNKEY) 
	player1\y = player1\y + HUMANSPEED 
End If

;if player presses Pause, pause the game
If KeyHit(PAUSEKEY) 
	;make screen blank
	Cls 
	
	Text 400,300,"Press 'P' to Unpause Game"
	
	Flip 
	
	;wait for player to unpause
	While Not KeyHit(PAUSEKEY) 
	Wend

EndIf

End Function





;TESTAI()
;Updates ball and score and enemy
Function TestAI()

;If ball is above computer, move computer up
If ball\y > player2\y 
	player2\y = player2\y + COMPUTERSPEED 

;if ball is lower than computer, move computer down
ElseIf ball\y < player2\y
	player2\y = player2\y - COMPUTERSPEED 
	EndIf
	
;If ball hits human player, reflect it away from him and variate its velocity and direction
If ImagesOverlap(ballimage,ball\x,ball\y,player1image,60,player1\y) 
	ball\xv = -ball\xv + Rand(-4,4) 
	ball\yv = ball\yv + Rand(-4,4)
	
;If ball hits computer, reflect it away from computer and variate its velocity and direction
ElseIf ImagesOverlap(ballimage,ball\x,ball\y,player2image,740,player2\y) 
	ball\xv = -ball\xv + Rand(-4,4) 
	ball\yv = ball\yv + Rand(-4,4)
	
;If ball hits top wall, reflect it downwards
ElseIf ball\y <= 0  
	ball\yv = -ball\yv + Rand (-1,1) 
	ball\xv = ball\xv + Rand (-1,1)  
	
;If ball hits bottom wall, reflect it upwards
ElseIf ball\y >= 600 
	ball\yv = -ball\yv + Rand (-1,1) 
	ball\xv = ball\xv + Rand (-1,1)  
	
;if ball hits left wall, computer has scored so computer gets one more point
ElseIf ball\x <= 0  
	player2\score = player2\score + 1   ;computer scores
	Text 400,300,"Player 2 Scores!!!"
	Flip
	;wait two seconds
	Delay(2000) 
	
	;reset level
	InitializeLevel() 
	
;If ball hits right wall, human scored so give him a point
ElseIf ball\x >= 800 
	player1\score = player1\score + 1   ;human scores
	Text 400,300,"Player 1 Scores!!!"
	Flip 
	;wait 2 secs
	Delay(2000) 
	;reset level
	InitializeLevel() 
EndIf
	
	;update ball's position on screen
	ball\x = ball\x + ball\xv 
	ball\y = ball\y + ball\yv 
	
End Function
	