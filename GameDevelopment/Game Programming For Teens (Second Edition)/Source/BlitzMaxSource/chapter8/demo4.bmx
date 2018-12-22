'**************************************************
'* demo4.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

AutoMidHandle True

'** Types ******************************************************************************************
	
	Type TPlayer
		
		Const SPEED				= 5
		Const DIRECTIONLEFT		= 1
		Const DIRECTIONRIGHT	= 3
		Const DIRECTIONUP		= 2
		Const DIRECTIONDOWN		= 4
		
		Field x:Int
		Field y:Int
		Field frame:Int
		Field direction:Int
		Field playerImage:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global player:TPlayer = New TPlayer
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.frame		= 4
player.direction	= player.DIRECTIONRIGHT
player.playerImage	= LoadAnimImage("animatedMonkey.png", 48, 40, 0, 8)

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			player.direction	= player.DIRECTIONLEFT
			player.frame		= (player.frame + 1) Mod 2 + (2 * player.direction - 2)
			
			If player.x < ImageWidth(player.playerImage) / 2 Then
				
				player.x = ImageWidth(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			player.direction	= player.DIRECTIONRIGHT
			player.frame		= (player.frame + 1) Mod 2 + (2 * player.direction - 2)
			
			If player.x > WIDTH - ImageWidth(player.playerImage) / 2
				
				player.x = WIDTH - ImageWidth(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_UP) Then
			
			player.y :- player.SPEED
			
			player.direction	= player.DIRECTIONUP
			player.frame		= (player.frame + 1) Mod 2 + (2 * player.direction - 2)
			
			If player.y < ImageHeight(player.playerImage) / 2 Then
				
				player.y = ImageHeight(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			player.direction	= player.DIRECTIONDOWN
			player.frame		= (player.frame + 1) Mod 2 + (2 * player.direction - 2)
			
			If player.y > HEIGHT - ImageHeight(player.playerImage) / 2 Then
				
				player.y = HEIGHT - ImageHeight(player.playerImage) / 2
			EndIf
		EndIf
		
		DrawImage player.playerImage, player.x, player.y, player.frame
		
		DrawText "X " + player.x, 10, HEIGHT - TextHeight("X ") - 70
		DrawText "Y " + player.y, 10, HEIGHT - TextHeight("Y ") - 50
		DrawText "Frame " + player.frame, 10, HEIGHT - TextHeight("Frame ") - 30
		DrawText "Direction " + player.direction, 10, HEIGHT - TextHeight("Direction ") - 10
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
