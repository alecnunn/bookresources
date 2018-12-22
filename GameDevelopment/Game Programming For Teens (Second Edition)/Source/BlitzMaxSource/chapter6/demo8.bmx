'**************************************************
'* demo8.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const BLACK:Int		= 1
	Const WHITE:Int		= 2
	Const RED:Int		= 3
	Const GREEN:Int		= 4
	Const BLUE:Int		= 5
	Const WIDTH:Int		= 640
	Const HEIGHT:Int	= 480
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Global variables *******************************************************************************
	
	Global running			= 1
	Global selectedColor	= WHITE
	Global picture:TImage	= CreateImage(WIDTH, HEIGHT)
'** End Global variables ***************************************************************************

GrabImage picture, 0, 0

'** Main loop **************************************************************************************
	
	While running
		
		Cls
		
		testInput()
		draw()
		
		FlushMem
		
		Flip
	Wend
'** End Main loop **********************************************************************************

End

'** testInput:Int() ********************************************************************************

	Function testInput:Int()
		
		If KeyHit(KEY_1) Then
			
			selectedColor = BLACK
		Else If KeyHit(KEY_2) Then
			
			selectedColor = WHITE
		Else If KeyHit(KEY_3) Then
			
			selectedColor = RED
		Else If KeyHit(KEY_4) Then
			
			selectedColor = GREEN
		Else If KeyHit(KEY_5) Then
			
			selectedColor = BLUE
		End If
		
		If KeyHit(KEY_ESCAPE) Then
			
			running = 0
		End If
		
		If MouseDown(KEY_MOUSELEFT) Then
			
			Local pixmapPicture = LockImage(picture)
			
			Select selectedColor
				
				Case BLACK
					
					WritePixel pixmapPicture, MouseX(), MouseY(), 255 Shl 24 + 0 Shl 16 + 0 Shl 8 + 0
				Case WHITE
					
					WritePixel pixmapPicture, MouseX(), MouseY(), 255 Shl 24 + 255 Shl 16 + 255 Shl 8 + 255
				Case RED
					
					WritePixel pixmapPicture, MouseX(), MouseY(), 255 Shl 24 + 255 Shl 16 + 0 Shl 8 + 0
				Case GREEN
					
					WritePixel pixmapPicture, MouseX(), MouseY(), 255 Shl 24 + 0 Shl 16 + 255 Shl 8 + 0
				Case BLUE
					
					WritePixel pixmapPicture, MouseX(), MouseY(), 255 Shl 24 + 0 Shl 16 + 0 Shl 8 + 255
			End Select
			
			UnlockImage picture
		End If
	End Function
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************

	Function draw:Int()
		
		DrawImage picture, 0, 0
		
		Local selectedColorName:String
		
		Select selectedcolor
			
			Case BLACK
				
				selectedColorName = "Black"
			Case WHITE
				
				selectedColorName = "White"
			Case RED
				
				selectedColorName = "Red"
			Case GREEN
				
				selectedColorName = "Green"
			Case BLUE
				
				selectedColorName = "Blue"
		End Select
		
		DrawText selectedColorName, 10, HEIGHT - TextHeight(selectedColorName) - 10
		DrawText "X " + MouseX() + " Y " + MouseY(), WIDTH - TextWidth("X " + MouseX() + " Y " + MouseY()) - 10, HEIGHT - TextHeight("X") - 10
	End Function
'** End draw:Int() *********************************************************************************
