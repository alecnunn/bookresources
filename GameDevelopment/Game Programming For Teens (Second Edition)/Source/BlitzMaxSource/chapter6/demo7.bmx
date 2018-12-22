'**************************************************
'* demo7.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

DrawText "Press [any] key to use ReadPixel", 10, 10

Flip

WaitKey

Cls

Local testImage:TImage = LoadImage("testImage.png")

DrawImage testimage, 0, HEIGHT / 2
DrawImage testImage, WIDTH / 2, HEIGHT / 2

Local tempImage:TImage = CreateImage(WIDTH, HEIGHT)

GrabImage tempImage, 0, 0

Local counterOne:Int
Local counterTwo:Int
Local pixmapTempImage:TPixmap = LockImage(tempImage)

Local pixelArray:Int[WIDTH, HEIGHT]

For counterOne = 0 To WIDTH - 1
	
	For counterTwo = 0 To HEIGHT - 1
		
		pixelArray[counterOne, counterTwo] = ReadPixel(pixmapTempImage, counterOne, counterTwo)
	Next
Next

UnlockImage tempImage

Flip

Delay 2500

Cls

DrawText "Press [any] key to use WritePixel", 10, 10

Flip

WaitKey

Cls

pixmapTempImage = LockImage(tempImage)

For counterOne = 0 To WIDTH - 1
	
	For counterTwo = 0 To HEIGHT - 1
		
		WritePixel pixmapTempImage, counterOne, counterTwo, pixelArray[WIDTH - counterOne, counterTwo]
	Next
Next

UnlockImage tempImage

DrawImage tempImage, 0, 0

DrawText "Press [any] key to quit", 10, 10

FlushMem

Flip

WaitKey

End
