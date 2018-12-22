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

SeedRnd MilliSecs()

'** Main loop **************************************************************************************
	
	Repeat
		
		SetColor Rand(0, 255), Rand(0, 255), Rand(0, 255)
		
		DrawLine Rand(0, WIDTH), Rand(0, HEIGHT), Rand(0, WIDTH), Rand(0, HEIGHT)
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
