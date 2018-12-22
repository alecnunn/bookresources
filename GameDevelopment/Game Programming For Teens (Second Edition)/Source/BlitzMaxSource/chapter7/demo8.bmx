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
	
	Type TPoint
		
		Field x:Int
		Field y:Int
	End Type
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global xs:Int		= WIDTH / 2
	Global ys:Int		= HEIGHT / 2
	Global xys:Float	= 2
	
	Global pointOne:TPoint		= New TPoint
	Global pointTwo:TPoint		= New TPoint
	Global pointThree:TPoint	= New TPoint
'** End Global variables ***************************************************************************

pointOne.x		= 0
pointOne.y		= -100
pointTwo.x		= 100
pointTwo.y		= 100
pointThree.x	= -100
pointThree.y	= 100

DrawText "Triagnle 1", 10, HEIGHT - 4 * TextHeight("Triangle 1") - 40

DrawLine pointOne.x + xs, pointOne.y + ys, pointTwo.x + xs, pointTwo.y + ys
DrawLine pointTwo.x + xs, pointTwo.y + ys, pointThree.x + xs, pointThree.y + ys
DrawLine pointThree.x + xs, pointThree.y + ys, pointOne.x + xs, pointOne.y + ys

Local testImage:TImage = CreateImage(-pointThree.x + pointTwo.x + 1, -pointOne.y + pointThree.y + 1)

GrabImage testImage, WIDTH / 2 + pointThree.x, HEIGHT / 2 - pointThree.y

DrawText "Press [any] key to continue", 10, HEIGHT - 3 * TextHeight("Press [any] key to continue") - 30

Flip

WaitKey

DrawText "Triangle 2", 10, HEIGHT - 2 * TextHeight("Triangle 2") - 20

SetScale xys, xys

DrawImage testimage, WIDTH / 2, HEIGHT / 2

SetScale 1, 1

DrawText "Press [any] key to quit", 10, HEIGHT - TextHeight("Press [any] key to quit") - 10

FlushMem

Flip

WaitKey

End
