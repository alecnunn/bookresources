'**************************************************
'* demo1.bmx									  *
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
		Field playerImage:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global staticBoyImage:TImage = LoadImage("staticBoy.png")
	
	Global player:TPlayer = New TPlayer
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.playerImage	= staticBoyImage

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			If player.x < ImageWidth(player.playerImage) / 2 Then
				
				player.x = ImageWidth(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			If player.x > WIDTH - ImageWidth(player.playerImage) / 2 Then
				
				player.x = WIDTH - ImageWidth(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_UP) Then
			
			player.y :- player.SPEED
			
			If player.y < ImageHeight(player.playerImage) / 2 Then
				
				player.y = ImageHeight(player.playerImage) / 2
			EndIf
		ElseIf KeyDown(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			If player.y > HEIGHT - ImageHeight(player.playerImage) / 2 Then
				
				player.y = HEIGHT - ImageHeight(player.playerImage) / 2
			EndIf
		EndIf
		
		DrawImage player.playerImage, player.x, player.y
		
		DrawText "X " + player.x, 10, HEIGHT - TextHeight("X ") - 30
		DrawText "Y " + player.y, 10, HEIGHT - TextHeight("Y ") - 10
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
