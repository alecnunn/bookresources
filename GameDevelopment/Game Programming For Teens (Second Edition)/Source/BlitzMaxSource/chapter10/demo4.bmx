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
	
	Type TBackground
		
		Const SPEED:Int = 5
		
		Field y:Int
		Field image:TImage
	EndType
	
	Type TPlayer Extends TBackground
		
		Field x:Int
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global player:TPlayer			= New TPlayer
	Global background:TBackground	= New TBackground
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.image		= LoadImage("ship.png")
background.y		= 0
background.image	= LoadImage("stars.png")

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyHit(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			If player.x < ImageWidth(player.image) Then
				
				player.x = ImageWidth(player.image)
			EndIf
		ElseIf KeyHit(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			If player.x > WIDTH - ImageWidth(player.image) / 2 Then
				
				player.x = WIDTH - ImageWidth(player.image) / 2
			EndIf
		ElseIf KeyHit(KEY_UP) Then
			
			player.y :- player.SPEED
			
			If player.y < ImageHeight(player.image) / 2 Then
				
				player.y = ImageHeight(player.image) / 2
			EndIf
		ElseIf KeyHit(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			If player.y > HEIGHT - ImageHeight(player.image) / 2 Then
				
				player.y = ImageHeight(player.image) / 2
			EndIf
		EndIf
		
		TileImage background.image, 0, background.y
		
		background.y :+ background.SPEED
		
		If background.y >= ImageHeight(background.image) Then
			
			background.y = 0
		EndIf
		
		DrawImage player.image, player.x, player.y
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
