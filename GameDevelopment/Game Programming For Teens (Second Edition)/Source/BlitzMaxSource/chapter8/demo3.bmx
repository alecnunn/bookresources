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

'** Global variables *******************************************************************************
	
	Global r:Int = 0
	
	Global rectanglesImage:TImage = LoadAnimImage("rectangles.png", 250, 250, 0, 2)
'** End Global variables ***************************************************************************

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		DrawText "R " + r, 10, HEIGHT - TextHeight("R ") - 10
		
		DrawImage rectanglesImage, WIDTH / 2, HEIGHT / 2, r Mod 2
		
		r :+ 1
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
