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
	
	Global x:Int	= WIDTH / 2
	Global y:Int	= HEIGHT / 2
'** End Global variables ***************************************************************************

Local collisions:Int = 0

'** Main loop **************************************************************************************
	
	Repeat
		
		If KeyDown(KEY_LEFT) Then
			
			x :- SPEED
		ElseIf KeyDown(KEY_RIGHT) Then
			
			x :+ SPEED
		ElseIf KeyDown(KEY_UP) Then
			
			y :- SPEED
		ElseIf KeyDown(KEY_DOWN) Then
			
			y :+ SPEED
		EndIf
		
		collisions = checkPixelWithWalls(collisions)
		
		Plot x, y
		
		DrawText "Collisions " + collisions, 10, HEIGHT - TextHeight("Collisions ") - 10
		
		Delay 50
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End

'** checkPixelWithWalls:Int(collisions:Int) ********************************************************
	
	Function checkPixelWithWalls:Int(collisions:Int)
		
		If x >= WIDTH Or x <= 0 Or y >= HEIGHT Or y <= 0 Then
			
			collisions :+ 1
			
			Cls
			
			DrawText "Collision", WIDTH / 2 - TextWidth("Collision") / 2, HEIGHT / 2 - TextHeight("Collision") / 2
			
			FlushMem
			
			Flip
			
			Delay 1000
			
			Cls
			
			x	= WIDTH / 2
			y	= HEIGHT / 2
		EndIf
		
		Return collisions
	EndFunction
'** End checkPixelWithWalls:Int(collisions:Int) ****************************************************
