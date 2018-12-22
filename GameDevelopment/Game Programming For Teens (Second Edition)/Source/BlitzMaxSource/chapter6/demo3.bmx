'**************************************************
'* demo3.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int			= 800
	Const HEIGHT:Int		= 600
	Const DEPTH:Int			= 0
	Const PLAYERSPEED:Int	= 8
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Types ******************************************************************************************
	
	Type TPlayer
		
		Field x:Int
		Field y:Int
		Field image:TImage
	End Type
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global testImage:TImage = LoadImage("testImage.png")
	
	Global player:TPlayer = New TPlayer
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

player.image	= testImage
player.x		= WIDTH / 2 - ImageWidth(testImage) / 2
player.y		= HEIGHT / 2 - ImageHeight(testImage) / 2

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		player.x = player.x + Rand(-PLAYERSPEED, PLAYERSPEED)
		player.y = player.y + Rand(-PLAYERSPEED, PLAYERSPEED)
		
		If player.x < 0 Then
			
			player.x = 0
		Else If player.x > WIDTH - ImageWidth(player.image) Then
			
			player.x = WIDTH - ImageWidth(player.image)
		Else If player.y < 0 Then
			
			player.y = 0
		Else If player.y > HEIGHT - ImageHeight(player.image) Then
			
			player.y = HEIGHT - ImageHeight(player.image)
		End If
		
		DrawImage player.image, player.x, player.y
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
