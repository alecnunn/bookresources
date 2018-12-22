'**************************************************
'* demo1.bmx									  *
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
	
	Global x:Int					= 0
	Global y:Int					= 0
	Global backgroundImage:TImage	= LoadImage("stars.png")
'** End Global variables ***************************************************************************

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			x :- SPEED
		ElseIf KeyDown(KEY_RIGHT) Then
			
			x :+ SPEED
		ElseIf KeyDown(KEY_UP) Then
			
			y :- SPEED
		ElseIf KeyDown(KEY_DOWN) Then
			
			y :+ SPEED
		EndIf
		
		If x >= ImageWidth(backgroundImage) Or x <= -ImageWidth(backgroundImage) Then
			
			x = 0
		ElseIf y >= ImageHeight(backgroundImage) Or y <= -ImageHeight(backgroundImage) Then
			
			y = 0
		EndIf
		
		TileImage backgroundImage, x, y
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
