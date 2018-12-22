'**************************************************
'* demo6.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Types ******************************************************************************************
	
	Type TPoint
		
		Field x:Int
		Field y:Int
	End Type
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global xs:Int		= WIDTH / 2
	Global ys:Int		= HEIGHT / 2 + 100
	Global xys:Float	= 2
	
	Global pointOne:TPoint		= New TPoint
	Global pointTwo:TPoint		= New TPoint
	Global pointThree:TPoint	= New TPoint
'** End Global variables ***************************************************************************

pointOne.x		= 0
pointOne.y		= -100
pointTwo.x		= 100
pointTwo.y		= 0
pointThree.x	= -100
pointThree.y	= 0

DrawText "Triagnle 1", 10, HEIGHT - 4 * TextHeight("Triangle 1") - 40

myDrawTri(pointOne.x, pointOne.y, pointTwo.x, pointTwo.y, pointThree.x, pointThree.y)

DrawText "Press [any] key to continue", 10, HEIGHT - 3 * TextHeight("Press [any] key to continue") - 30

Flip

WaitKey

pointOne.x		= pointOne.x * xys
pointOne.y		= pointOne.y * xys
pointTwo.x		= pointTwo.x * xys
pointTwo.y		= pointTwo.y * xys
pointThree.x	= pointThree.x * xys
pointThree.y	= pointthree.y * xys

DrawText "Triangle 2", 10, HEIGHT - 2 * TextHeight("Triangle 2") - 20

myDrawTri(pointOne.x, pointOne.y, pointTwo.x, pointTwo.y, pointThree.x, pointThree.y)

DrawText "Press [any] key to quit", 10, HEIGHT - TextHeight("Press [any] key to quit") - 10

FlushMem

Flip

WaitKey

End

'** myDrawTri:Int(pointOneX:Float, pointOneY:Float, pointTwoX:Float, pointTwoY:Float, pointThreeX:Float, pointThreeY:Float) **
	
	Function myDrawTri:Int(pointOneX:Float, pointOneY:Float, pointTwoX:Float, pointTwoY:Float, pointThreeX:Float, pointThreeY:Float)
		
		DrawLine pointOneX + xs, pointOneY + ys, pointTwoX + xs, pointTwoY + ys
		DrawLine pointTwoX + xs, pointTwoY + ys, pointThreeX + xs, pointThreeY + ys
		DrawLine pointThreeX + xs, pointThreeY + ys, pointOneX + xs, pointOneY + ys
	End Function
'** End myDrawTri:Int(pointOneX:Float, pointOneY:Float, pointTwoX:Float, pointTwoY:Float, pointThreeX:Float, pointThreeY:Float) **
