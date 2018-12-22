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

'** Global variables *******************************************************************************
	
	Global r:Float = 0
	
	Global testImage:TImage = LoadImage("testImage.png")
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			r = r - 5
		Else If KeyDown(KEY_RIGHT) Then
			
			r = r + 5
		End If
		
		SetRotation r
		
		DrawImage testimage, WIDTH / 2, HEIGHT / 2
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
