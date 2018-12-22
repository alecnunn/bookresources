'**************************************************
'* demo9.bmx									  *
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

SetMaskColor 1, 1, 1

'** Global variables *******************************************************************************
	
	Global r:Float	= 90
	
	Global testImage:TImage = LoadImage("testImage.png", MASKEDIMAGE)
'** End Global variables ***************************************************************************

DrawText "Phase 1", 10, HEIGHT - 4 * TextHeight("Phase 1") - 40

DrawImage testImage, WIDTH / 2, HEIGHT / 2

DrawText "Press [any] key to continue", 10, HEIGHT - 3 * TextHeight("Press [any] key to continue") - 30

Flip

WaitKey

DrawText "Phase 2", 10, HEIGHT - 2 * TextHeight("Phase 2") - 20

SetRotation r

DrawImage testImage, WIDTH / 2, HEIGHT / 2

SetRotation 0

DrawText "Press [any] key to quit", 10, HEIGHT - TextHeight("Press [any] key to quit") - 10

FlushMem

Flip

WaitKey

End
