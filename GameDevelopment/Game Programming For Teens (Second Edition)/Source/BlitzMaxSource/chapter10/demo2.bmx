'**************************************************
'* demo2.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
	Const SPEED:Int		= 5
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Global variables *******************************************************************************
	
	Global y:Int					= 0
	Global backgroundImage:TImage	= LoadImage("stars.png")
'** End Global variables ***************************************************************************

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		DrawText "Press [Esc] to quit", 10, HEIGHT - TextHeight("Press [Esc] to quit") - 10
		
		TileImage backgroundImage, 0, y
		
		y :+ SPEED
		
		If y >= ImageHeight(backgroundImage) Then
			
			y = 0
		EndIf
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

DrawText "Press [any] key to quit", 10, HEIGHT - TextHeight("Press [any] key to quit") - 10

WaitKey

End
