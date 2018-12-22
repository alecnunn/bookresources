'**************************************************
'* demo3.bmx									  *
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
		
		Const SPEED:Int = 1
		
		Field y:Int
		Field image:TImage
	EndType
	
	Type TPlayer Extends TBackground
		
		Const SPEED:Int = 10
		
		Field x:Int
	EndType
	
	Type TEnemy Extends TPlayer
		
		Const SPEED:Int = 5
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global background:TBackground	= New TBackground
	Global player:TPlayer			= New TPlayer
	Global enemy:TEnemy				= New TEnemy
'** End Global variables ***************************************************************************

background.y		= 0
background.image	= LoadImage("stars.png")
player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.image		= LoadImage("ship.png")
enemy.x				= WIDTH / 2
enemy.image			= LoadImage("enemyShip.png")
enemy.y				= ImageHeight(enemy.image) / 2 + 10

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		TileImage background.image, 0, background.y
		
		background.y :+ 1
		
		If background.y >= ImageHeight(background.image) Then
			
			background.y = 0
		EndIf
		
		DrawImage player.image, player.x, player.y
		DrawImage enemy.image, enemy.x, enemy.y
		
		If KeyDown(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			If player.x < ImageWidth(player.image) / 2 Then
				
				player.x = ImageWidth(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			If player.x > WIDTH - ImageWidth(player.image) / 2 Then
				
				player.x = WIDTH - ImageWidth(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_UP) Then
			
			player.y :- player.SPEED
			
			If player.y < ImageHeight(player.image) / 2 Then
				
				player.y = ImageHeight(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			If player.y > HEIGHT - ImageHeight(player.image) / 2 Then
				
				player.y = HEIGHT - ImageHeight(player.image) / 2
			EndIf
		EndIf
		
		If enemy.x > player.x Then
			
			enemy.x :- enemy.SPEED
		EndIf
		
		If enemy.x < player.x Then
			
			enemy.x :+ enemy.SPEED
		EndIf
		
		If enemy.y > player.y
			
			enemy.y :- enemy.SPEED
		EndIf
		
		If enemy.y < player.y
			
			enemy.y :+ enemy.SPEED
		EndIf
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
