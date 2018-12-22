;KONG
;Developed by Maneesh Sethi
;11/2002


Graphics 800,600 ;Set up graphics mode

AppTitle "KONG" ;Make the program name "KONG"


SeedRnd(MilliSecs()) ;Seed the random generator (make random numbers actually random)


SetBuffer BackBuffer() ;Create a back buffer)



;CONSTS
Const UPKEY = 200  ;Up
Const DOWNKEY = 208 ;Down
Const PAUSEKEY = 25 ;P
Const HUMANSPEED = 7 ;The human's max speed
Const COMPUTERSPEED = 6 ;The computer's max speed


;TYPES
Type player ;Each player's type
	Field y,score ;y position and score
End Type


Type ball ;ball's type
	Field x,y,xv,yv ;x, y coordinates, and x, y velocity
End Type

;IMAGES

Global player1image = LoadImage("player1.bmp") ;Load the human image
Global player2image = LoadImage("player2.bmp") ;Load the computer image
Global ballimage = LoadImage("ball.bmp") ;Load the ball image
Global backgroundimageclose = LoadImage("stars.bmp") ;Load the close
Global backgroundimagefar = LoadImage("starsfarther.bmp") ;Load the far stars

;TYPE INITIALIZATION
Global ball.ball = New ball ;Create a ball
Global player1.player = New player ;Create the human
Global player2.player = New player ;Create the computer


;INITIALIZATION

Text 400,300,"Ready...Set"
Delay(1000) ;Wait for one second
Text 420,330,"GO!!!"
Flip ;Show GO!!! on the screen
	Delay(200) ;Delay for 1/5 of a second
scrolly = 0


InitializeLevel() ;Call InitializeLevel()

AutoMidHandle True ;Set the handle to the center of images
player1\score = 0 ;Set init scores
player2\score = 0

;MAIN LOOP
While Not KeyDown(1)

Cls ;Clear the screen

TileImage backgroundimagefar,0,scrolly
TileImage backgroundimageclose,0,scrolly*2

scrolly=scrolly+1
If scrolly >= ImageHeight(backgroundimageclose)
	scrolly = 0
EndIf


TestKeyboard() ;Test what user pressed
TestAI() ;What should AI do?
DrawScore() ;Draw the HUD

DrawImage (ballimage,ball\x,ball\y) ;Draw the ball
DrawImage (player1image, 60, player1\y) ;Draw the human
DrawImage (player2image, 740, player2\y) ;Draw the computer

Flip

Wend ;END OF MAIN LOOP




;FUNCTIONS
;INITIALIZELEVEL()
;Sets up starting values
Function InitializeLevel()
ball\x = 400 ;Ball is in center of screen
ball\y = 300 ;Ball is in center of screen
ball\xv = Rand(2,6) ;Ball is moving to the right between 2 and 6
ball\yv = Rand(-8,8) ;Ball is moving up and down between -8 and 8

player2\y = 300 ;Put players in center of screen
player1\y = 300
End Function


;DRAWSCORE()
;Draws the HUD in the top right
Function DrawScore()
Text 700,0,"Player 1: " + player1\score ;Write the human score
Text 700,30,"Player 2: " + player2\score ;Write the computer's score
End Function

;TESTKEYBOARD()
;Moves player up and down based on keyboard
Function TestKeyboard()

If KeyDown(UPKEY) ;player hits up
	player1\y = player1\y - HUMANSPEED ;Move him up
EndIf

If KeyDown(DOWNKEY) ;player hits down
	player1\y = player1\y + HUMANSPEED ;move player down
End If

If KeyHit(PAUSEKEY) ;player hits 'p'
	Cls ;make screen blank
	Text 400,300,"Press 'P' to Unpause Game"
	Flip ;show text
	While Not KeyHit(PAUSEKEY) ;wait for player to unpause
	Wend
EndIf

End Function





;TESTAI()
;Updates ball and score and enemy
Function TestAI()
If ball\y > player2\y ;If ball is higher than computer
	player2\y = player2\y + COMPUTERSPEED ;move computer up
ElseIf ball\y < player2\y;if ball is lower than computer
	player2\y = player2\y - COMPUTERSPEED ;move computer down
	EndIf
If ImagesOverlap(ballimage,ball\x,ball\y,player1image,60,player1\y) ;if ball hits player
	ball\xv = -ball\xv + Rand(-4,4) ;reflect it towards player
	ball\yv = ball\yv + Rand(-4,4) ;keep speed with a little variation
ElseIf ImagesOverlap(ballimage,ball\x,ball\y,player2image,740,player2\y) ;if ball hits computer
	ball\xv = -ball\xv + Rand(-4,4) ;reflect towards human
	ball\yv = ball\yv + Rand(-4,4) ;keep speed with a little variation
ElseIf ball\y <= 0  ;if ball hits top wall
	ball\yv = -ball\yv + Rand (-1,1) ;reflect downwards
	ball\xv = ball\xv + Rand (-1,1)  ;change slope a little
ElseIf ball\y >= 600 ;if ball hits bottom wall
	ball\yv = -ball\yv + Rand (-1,1) ;reflect upwards
	ball\xv = ball\xv + Rand (-1,1)  ;change slope a little
ElseIf ball\x <= 0  ;if ball hit's human wall
	player2\score = player2\score + 1 ;computer scores
	Text 400,300,"Player 2 Scores!!!"
	Flip ;show text
	Delay(2000) ;wait two seconds
	InitializeLevel() ;reset level
ElseIf ball\x >= 800 ;if human scores
	player1\score = player1\score + 1 ;human scores
	Text 400,300,"Player 1 Scores!!!"
	Flip ;show text
	Delay(2000) ;wait 2 secs
	InitializeLevel() ;reset level
EndIf
	
	ball\x = ball\x + ball\xv ;move ball a little right and left
	ball\y = ball\y + ball\yv ;move ball a little up and down

End Function
	