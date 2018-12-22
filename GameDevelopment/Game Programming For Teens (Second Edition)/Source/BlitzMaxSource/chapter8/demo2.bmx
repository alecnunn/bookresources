'**************************************************
'* demo2.bmx									  *
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
		
		Const SPEED:Int = 5
		
		Field x:Int
		Field y:Int
		Field frame:Int
		Field playerImage:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global animatedBoyImage:TImage = LoadAnimImage("animatedBoy.png", 95, 71, 0, 8)
	
	Global player:TPlayer = New TPlayer
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.frame		= 0
player.playerImage	= animatedBoyImage

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			If player.x < ImageWidth(player.playerImage) / 2 Then
				
				player.x = ImageWidth(player.playerImage) / 2
			EndIf
			
			player.frame :- 1
		ElseIf KeyDown(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			If player.x > WIDTH - ImageWidth(player.playerImage) / 2 Then
				
				player.x = WIDTH - ImageWidth(player.playerImage) / 2
			EndIf
			
			player.frame :+ 1
		ElseIf KeyDown(KEY_UP) Then
			
			player.y :- player.SPEED
			
			If player.y < ImageHeight(player.playerImage) / 2 Then
				
				player.y = ImageHeight(player.playerImage) / 2
			EndIf
			
			player.frame :+ 1
		ElseIf KeyDown(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			If player.y > HEIGHT - ImageHeight(player.playerImage) / 2 Then
				
				player.y = HEIGHT - ImageHeight(player.playerImage) / 2
			EndIf
			
			player.frame :- 1
		EndIf
		
		If player.frame > 7 Then
			
			player.frame = 0
		ElseIf player.frame < 0 Then
			
			player.frame = 7
		EndIf
		
		DrawImage player.playerImage, player.x, player.y, player.frame
		
		DrawText "X " + player.x, 10, HEIGHT - TextHeight("X ") - 30
		DrawText "Y " + player.y, 10, HEIGHT - TextHeight("Y ") - 10
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
