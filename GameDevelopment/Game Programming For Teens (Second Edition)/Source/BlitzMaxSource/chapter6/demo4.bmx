'**************************************************
'* demo4.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int				= 800
	Const HEIGHT:Int			= 600
	Const DEPTH:Int				= 0
	Const TESTIMAGEWIDTH:Int	= 200
	Const TESTIMAGEHEIGHT:Int	= 200
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

AutoMidHandle True

Local counterOne:Int
Local counterTwo:Int
Local alpha:Int					= 255
Local red:Int					= 255
Local green:Int					= 255
Local blue:Int					= 255
Local testImage:TImage 			= CreateImage(TESTIMAGEWIDTH, TESTIMAGEHEIGHT)
Local pixmapTestImage:TPixmap	= LockImage(testImage)

For counterOne = 0 To TESTIMAGEWIDTH - 1
	
	For counterTwo = 0 To TESTIMAGEHEIGHT - 1
		
		WritePixel pixmapTestImage, counterOne, counterTwo, alpha Shl 24 + red Shl 16 + green Shl 8 + blue
	Next
Next

UnlockImage testImage

DrawImage testImage, WIDTH / 2, HEIGHT / 2

FlushMem

Flip

WaitKey

End
