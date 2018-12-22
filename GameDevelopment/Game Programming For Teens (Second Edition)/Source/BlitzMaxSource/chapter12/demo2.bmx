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
	
	Type TFly
		
		Const SPEED:Int	= 10
		Const TIME:Int	= 1500
		
		Field x:Int
		Field y:Int
		Field xd:Int
		Field yd:Int
		Field frame:Int
		Field timer:Int
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global fly:TFly = New TFly
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

fly.x		= WIDTH / 2
fly.y		= HEIGHT / 2
fly.xd		= Rand(-fly.SPEED, fly.SPEED)
fly.yd		= Rand(-fly.SPEED, fly.SPEED)
fly.frame	= 0
fly.timer	= MilliSecs()
fly.image	= LoadAnimImage("fly.png", 64, 64, 0, 4)

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		DrawText "Fly X " + fly.x, 10, HEIGHT - TextHeight("Fly X ") - 50
		DrawText "Fly Y " + fly.y, 10, HEIGHT - TextHeight("Fly Y ") - 30
		DrawText "Timer " + (fly.TIME - MilliSecs() + fly.timer), 10, HEIGHT - TextHeight("Timer ") - 10
		
		DrawImage fly.image, fly.x, fly.y, fly.frame
		
		If MilliSecs() >= fly.TIME + fly.timer
			
			fly.xd		= Rand(-fly.SPEED / 2, fly.SPEED / 2)
			fly.yd		= Rand(-fly.SPEED / 2, fly.SPEED / 2)
			fly.timer	= MilliSecs()
		EndIf
		
		fly.x :+ fly.xd
		fly.y :+ fly.yd
		
		If fly.x > WIDTH - ImageWidth(fly.image) / 2 Or fly.x < ImageWidth(fly.image) / 2 Then
			
			fly.xd = -fly.xd
		ElseIf fly.y > HEIGHT - ImageHeight(fly.image) / 2 Or fly.y < ImageWidth(fly.image) / 2 Then
			
			fly.yd = -fly.yd
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
