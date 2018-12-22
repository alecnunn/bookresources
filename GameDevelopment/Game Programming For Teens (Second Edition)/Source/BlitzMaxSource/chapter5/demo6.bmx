'**************************************************
'* demo6.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int				= 800
	Const HEIGHT:Int			= 600
	Const DEPTH:Int				= 0
	Const MAXIMUMOVALWIDTH:Int	= 200
	Const MAXIMUMOVALHEIGHT:Int	= 200
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

SeedRnd MilliSecs()

'** Main loop **************************************************************************************
	
	Repeat
		
		SetColor Rand(0, 255), Rand(0, 255), Rand(0, 255)
		
		DrawOval Rand(0, WIDTH), Rand(0, HEIGHT), Rand(0, MAXIMUMOVALWIDTH), Rand(0, MAXIMUMOVALHEIGHT)
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
