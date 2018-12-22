'**************************************************
'* demo11.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
	Const ROTATIONS:Int	= 16
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

AutoMidHandle True

'** Global variables *******************************************************************************
	
	Global frame:Int
	Global testImage:TImage = LoadImage("testImage.png")
	
	Global testImageArray:TImage[ROTATIONS]
'** End Global variables ***************************************************************************

For frame = 0 To ROTATIONS - 1
	
	testImageArray[frame] = CreateImage(ImageWidth(testImage), ImageHeight(testImage))
Next

For frame = 0 To ROTATIONS - 1
	
	Cls
	
	DrawImage testimage, ImageWidth(testImage) / 2, ImageHeight(testImage) / 2
	
	GrabImage testImageArray[frame], 0, 0
	
	SetRotation frame * 360 / ROTATIONS
Next

frame = 0

SetRotation 0

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyDown(KEY_LEFT) Then
			
			frame = frame - 1
			
			If frame < 1 Then
				
				frame = ROTATIONS - 1
			End If
		Else If KeyDown(KEY_RIGHT) Then
			
			frame = frame + 1
			
			If frame >= ROTATIONS - 1 Then
				
				frame = 0
			End If
		End If
		
		DrawImage testImageArray[frame], WIDTH / 2, HEIGHT / 2
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
