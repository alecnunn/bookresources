'**************************************************
'* demo5.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 640
	Const HEIGHT:Int	= 480
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

SetMaskColor 63, 63, 63

Local testImage:TImage = LoadImage("testImage.png", MASKEDIMAGE)

MidHandleImage testImage

DrawImage testImage, WIDTH / 2, HEIGHT / 2

Flip

FlushMem

WaitKey

End
