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
	
	Type TFly
		
		Const SPEED:Int = 10
		
		Field x:Int
		Field y:Int
		Field frame:Int
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global fly:TFly = New TFly
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

fly.x		= WIDTH / 2
fly.y		= HEIGHT / 2
fly.frame	= 0
fly.image	= LoadAnimImage("fly.png", 64, 64, 0, 4)

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		DrawText "Fly X " + fly.x, 10, HEIGHT - TextHeight("Fly X ") - 30
		DrawText "Fly Y " + fly.y, 10, HEIGHT - TextHeight("Fly Y ") - 10
		
		DrawImage fly.image, fly.x, fly.y, fly.frame
		
		fly.x :+ Rand(-fly.SPEED, fly.SPEED)
		fly.y :+ Rand(-fly.SPEED, fly.SPEED)
		
		If fly.x > WIDTH - ImageWidth(fly.image) / 2 Then
			
			fly.x = WIDTH - ImageWidth(fly.image) / 2
		ElseIf fly.x < ImageWidth(fly.image) / 2 Then
			
			fly.x = ImageWidth(fly.image) / 2
		ElseIf fly.y > HEIGHT - ImageHeight(fly.image) / 2 Then
			
			fly.y = HEIGHT - ImageHeight(fly.image) / 2
		ElseIf fly.y < ImageHeight(fly.image) / 2 Then
			
			fly.y = ImageHeight(fly.image) / 2
		EndIf
		
		fly.frame :+ 1
		
		If fly.frame > 3 Then
			
			fly.frame = 0
		EndIf
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
