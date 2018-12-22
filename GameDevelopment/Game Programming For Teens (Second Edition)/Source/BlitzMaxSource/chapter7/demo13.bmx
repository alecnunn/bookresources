'**************************************************
'* demo13.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Global variables *******************************************************************************
	
	Global scrollY:Int		= 0
	Global testImageSmall	= LoadImage("testImageSmall.png")
'** End Global variables ***************************************************************************

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		TileImage testImageSmall, 0, scrollY
		
		scrollY = scrollY + 1
		
		If scrollY >= ImageHeight(testImageSmall) Then
			
			scrollY = 0
		End If
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
