'**************************************************
'* demo10.bmx									  *
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
		
		Const SPEED:Int = 5
		
		Field y
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global shipImage:TImage = LoadImage("ship.png")
	
	Global background:TBackground = New TBackground
'** End Global variables ***************************************************************************

background.y		= 0
background.image	= LoadImage("stars.png")

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		TileImage background.image, 0, background.y
		
		background.y = MouseZ() * background.SPEED
		
		If KeyHit(KEY_UP) Then
			
			background.y :- background.SPEED
		ElseIf KeyHit(KEY_DOWN) Then
			
			background.y :+ background.SPEED
		EndIf
		
		DrawImage shipImage, MouseX(), MouseY()
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
