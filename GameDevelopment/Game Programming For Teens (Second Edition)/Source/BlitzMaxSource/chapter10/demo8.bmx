'**************************************************
'* demo8.bmx									  *
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
		
		Const SPEED = 1
		
		Field y:Int
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global cursorImage = LoadImage("cursor.png")
	
	Global background:TBackground = New TBackground
'** End Global variables ***************************************************************************

background.y		= 0
background.image	= LoadImage("stars.png")

SetImageHandle cursorImage, 0, 0

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		DrawText "X " + MouseX(), 10, HEIGHT - TextHeight("X ") - 30
		DrawText "Y " + MouseY(), 10, HEIGHT - TextHeight("Y ") - 10
		
		TileImage background.image, 0, background.y
		
		background.y :+ background.SPEED
		
		If background.y >= ImageHeight(background.image) Then
			
			background.y = 0
		EndIf
		
		DrawImage cursorImage, MouseX(), MouseY()
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
