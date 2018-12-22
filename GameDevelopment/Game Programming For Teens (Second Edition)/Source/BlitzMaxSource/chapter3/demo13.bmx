'**************************************************
'* demo13.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int					= 800
	Const HEIGHT:Int				= 600
	Const DEPTH:Int					= 0
	Const PLAYERSTRING:String		= "[)()()()()()()()()(]"
	Const PLAYERSPEED:Int			= 8
	Const BALLSTRING:String			= "()"
	Const MAXIMUMBALLSPEED:Int		= 4
	Const BLOCKSTRING:String		= "[XXXXXX]"
	Const MAXIMUMBLOCKROWS:Int		= 9
	Const MAXIMUMBLOCKCOLUMNS:Int	= 9
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Types ******************************************************************************************
	
	Type TPlayer
		
		Field x:Int
		Field y:Int
		Field playerImage:String
	End Type
	
	Type TBall
		
		Field x:Int
		Field y:Int
		Field directionX:Int
		Field directionY:Int
		Field ballImage:String
	End Type
	
	Type TBlock
		
		Field x:Int
		Field y:Int
		Field blockImage:String
	End Type
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global level:Int
	Global score:Int
	Global framesPerSecond:Int
	Global framesPerSecondTime:Int
	Global framesPerSecondCounter:Int
	Global playing:Int		= 1
	Global playerWidth:Int	= TextWidth(PLAYERSTRING)
	Global playerHeight:Int	= TextHeight(PLAYERSTRING)
	Global ballWidth:Int	= TextWidth(BALLSTRING)
	Global ballHeight:Int	= TextHeight(BALLSTRING)
	Global blockWidth:Int	= TextWidth(BLOCKSTRING)
	Global blockHeight:Int	= TextHeight(BLOCKSTRING)
	
	Global player:TPlayer	= New TPlayer
	Global ball:TBall		= New TBall
	Global block:TBlock		= New TBlock
	
	Global blocks:Int[MAXIMUMBLOCKROWS + 1, MAXIMUMBLOCKCOLUMNS + 1]
'** End Global variables ***************************************************************************

SetColor 223, 223, 223
SetClsColor 31, 31, 31
SeedRnd MilliSecs()

newGame()

'** Main loop **************************************************************************************
	
	While playing
		
		Cls
		
		testInput()
		getFramesPerSecond()
		draw()
		
		FlushMem
		
		Flip
	Wend
'** End Main loop **********************************************************************************

End

'** newGame:Int() **********************************************************************************
	
	Function newGame:Int()
		
		level				= 1
		score				= 0
		player.playerImage	= PLAYERSTRING
		ball.ballImage		= BALLSTRING
		block.blockImage	= BLOCKSTRING
		
		Local rows:Int
		Local columns:Int
		
		For rows = 0 To MAXIMUMBLOCKROWS
			
			For columns = 0 To MAXIMUMBLOCKCOLUMNS
				
				blocks[rows, columns] = 0
			Next
		Next
		
		message("Demo 13, Press [any] key to start the game.", 0)
		
		WaitKey
		
		message("Controls: [Left] Move player left, [Right] Move player right, [Escape] Quit game.", 3500)
		
		newLevel()
	End Function
'** End newGame:Int() ******************************************************************************

'** newLevel:Int() *********************************************************************************
	
	Function newLevel:Int()
		
		player.x			= WIDTH / 2 - playerWidth / 2
		player.y			= HEIGHT - playerHeight - 10 - TextHeight("Level") - 10
		ball.x				= WIDTH / 2 - ballWidth / 2
		ball.y				= HEIGHT / 4 * 3 - ballHeight / 2
		ball.directionX		= Rand(-MAXIMUMBALLSPEED, MAXIMUMBALLSPEED)
		ball.directionY		= Rand(1, MAXIMUMBALLSPEED)
		
		message("Level " + level, 1000)
		
		Local rows:Int
		Local columns:Int
		
		For rows = 0 To level - 1
			
			For columns = 0 To MAXIMUMBLOCKCOLUMNS
				
				blocks[rows, columns] = 1
			Next
		Next
	End Function
'** End newLevel:Int() *****************************************************************************

'** message:Int(message:String, seconds:Int) *******************************************************
	
	Function message:Int(message:String, seconds:Int)
		
		Local x = WIDTH / 2 - TextWidth(message) / 2
		Local y = HEIGHT / 2 - TextHeight(message) / 2
		
		Cls
		
		DrawText message, x, y
		
		FlushMem
		
		Flip
		
		Delay seconds
	End Function
'** End message:Int(message:String, seconds:Int) ***************************************************

'** testInput:Int() ********************************************************************************
	
	Function testInput:Int()
		
		If KeyDown(KEY_LEFT) Then
			
			player.x = player.x - PLAYERSPEED
			
			If player.x < 0 Then
				
				player.x = 0
			End If
		Else If KeyDown(KEY_RIGHT) Then
			
			player.x = player.x + PLAYERSPEED
			
			If player.x > WIDTH - playerWidth Then
				
				player.x = WIDTH - playerWidth
			End If
		End If
		
		If KeyHit(KEY_ESCAPE) Then
			
			gameOver()
			
			playing = 0
		End If
	End Function
'** End testInput:Int() ****************************************************************************

'** getFramesPerSecond:Int() ***********************************************************************
	
	Function getFramesPerSecond:Int()
		
		framesPerSecondCounter = framesPerSecondCounter + 1
		
		If framesPerSecondTime = 0 Then
			
			framesPerSecondTime = MilliSecs()
		End If
		
		If MilliSecs() > framesPerSecondTime + 1000 Then
			
			framesPerSecond			= framesPerSecondCounter
			framesPerSecondCounter	= 0
			framesPerSecondTime		= MilliSecs()
		End If
	End Function
'** End getFramesPerSecond:Int() *******************************************************************

'** gameOver:Int() *********************************************************************************
	
	Function gameOver:Int()
		
		message("Game over! Level " + level + ", score " + score + ".", 3500)
	End Function
'** End gameOver:Int() *****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		checkBall()
		drawBlocks()
		
		DrawText player.playerImage, player.x, player.y
		DrawText ball.ballImage, ball.x, ball.y
		DrawText "Level " + level, 10, HEIGHT - TextHeight("Level") - 10
		DrawText "Score " + score, WIDTH / 2 - TextWidth("Score " + score) / 2, HEIGHT - TextHeight("Score") - 10
		DrawText "FPS " + framesPerSecond, WIDTH - TextWidth("FPS " + framesPerSecond) - 10, HEIGHT - TextHeight("FPS") - 10
	End Function
'** End draw:Int() *********************************************************************************

'** checkBall:Int() ********************************************************************************
	
	Function checkBall:Int()
		
		checkBallWithPlayer()
		checkBallWithBlocks()
		checkBallWithWalls()
		correctBall()
		
		ball.x = ball.x + ball.directionX
		ball.y = ball.y + ball.directionY
	End Function
'** End checkBall:Int() ****************************************************************************

'** checkBallWithPlayer:Int() **********************************************************************
	
	Function checkBallWithPlayer:Int()
		
		If ball.x + ballWidth >= player.x And ball.x <= player.x + playerWidth Then
			
			If ball.y + ballHeight >= player.y And ball.y + ballHeight <= player.y + playerHeight Then
				
				score			= score - 5
				ball.y			= player.y - ballWidth
				ball.directionY	= -ball.directionY + Rand(-MAXIMUMBALLSPEED, -1)
				
				If ball.x + ballWidth >= player.x And ball.x + ballWidth <= player.x + (playerWidth / 3) Then
					
					ball.directionX = Rand(-MAXIMUMBALLSPEED, -1)
				Else If ball.x >= player.x + (playerWidth / 3) And ball.x + ballWidth <= player.x + (playerWidth / 3 * 2) Then
					
					ball.directionX = Rand(-MAXIMUMBALLSPEED, MAXIMUMBALLSPEED)
				Else If ball.x >= player.x + (playerWidth / 3 * 2) And ball.x <= player.x + playerWidth Then
					
					ball.directionX = Rand(1, MAXIMUMBALLSPEED)
				End If
			End If
		End If
	End Function
'** End checkBallWithPlayer:Int() ******************************************************************

'** checkBallWithBlocks:Int() **********************************************************************
	
	Function checkBallWithBlocks:Int()
		
		block.y = 10
		
		Local rows:Int
		Local columns:Int
		
		For rows = 0 To level - 1
			
			block.x = 10
			
			For columns = 0 To MAXIMUMBLOCKCOLUMNS
				
				If blocks[rows, columns] Then
					
					If ball.x + ballWidth >= block.x And ball.x <= block.x + blockWidth Then
						
						If ball.y >= block.y And ball.y <= block.y + blockHeight Then
							
							score = score + 25
							
							ball.directionY = -ball.directionY
							
							blocks[rows, columns] = 0
						End If
					End If
				End If
				
				block.x = block.x + blockWidth + 15
			Next
			
			block.y = block.y + blockHeight + 15
		Next
	End Function
'** End checkBallWithBlocks:Int() ******************************************************************

'** checkBallWithWalls:Int() ***********************************************************************
	
	Function checkBallWithWalls:Int()
		
		If ball.x < 0 Then
			
			ball.x			= 0
			ball.directionX	= -ball.directionX + Rand(1, MAXIMUMBALLSPEED)
		Else If ball.x > WIDTH - ballWidth Then
			
			ball.x			= WIDTH - ballWidth
			ball.directionX	= -ball.directionX + Rand(-MAXIMUMBALLSPEED, -1)
		Else If ball.y < 0 Then
			
			ball.y			= 0
			ball.directionY	= -ball.directionY + Rand(1, MAXIMUMBALLSPEED)
		Else If ball.y > HEIGHT + ballHeight Then
			
			gameOver()
			
			newGame()
		End If
	End Function
'** End checkBallWithWalls:Int() *******************************************************************

'** correctBall:Int() ******************************************************************************
	
	Function correctBall:Int()
		
		If ball.directionX < -MAXIMUMBALLSPEED + MAXIMUMBALLSPEED + MAXIMUMBALLSPEED Then
			
			ball.directionX = -MAXIMUMBALLSPEED
		Else If ball.directionX > MAXIMUMBALLSPEED + MAXIMUMBALLSPEED + MAXIMUMBALLSPEED Then
			
			ball.directionX = MAXIMUMBALLSPEED
		Else If ball.directionY < -MAXIMUMBALLSPEED + MAXIMUMBALLSPEED + MAXIMUMBALLSPEED Then
			
			ball.directionY = -MAXIMUMBALLSPEED
		Else If ball.directionY > MAXIMUMBALLSPEED + MAXIMUMBALLSPEED + MAXIMUMBALLSPEED Then
			
			ball.directionY = MAXIMUMBALLSPEED
		End If
	End Function
'** End correctBall:Int() **************************************************************************

'** drawBlocks:Int() *******************************************************************************
	
	Function drawBlocks:Int()
		
		block.y = 10
		
		Local rows:Int
		Local columns:Int
		Local blocksLeft:Int = 0
		
		For rows = 0 To level - 1
			
			block.x = 10
			
			For columns = 0 To MAXIMUMBLOCKCOLUMNS
				
				If blocks[rows, columns] Then
					
					blocksLeft = blocksLeft + 1
					
					DrawText block.blockImage, block.x, block.y
				End If
				
				block.x = block.x + blockWidth + 15
			Next
			
			block.y = block.y + blockHeight + 15
		Next
		
		If blocksLeft = 0 Then
			
			message("Level " + level + " completed! Score " + score + ".", 3500)
			
			level = level + 1
			
			If level = 10 Then
				
				gameOver()
			Else
				
				newLevel()
			End If
		End If
	End Function
'** End drawBlocks:Int() ***************************************************************************
