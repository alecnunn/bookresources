'**************************************************
'* demo3.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int				= 800
	Const HEIGHT:Int			= 600
	Const DEPTH:Int				= 0
	Const RECTANGLESTARTX:Int	= 10
	Const RECTANGLESTARTY:Int	= 10
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

'** Global variables *******************************************************************************
	
	Global xys:Float			= 2
	Global rectangleWidth:Int	= 100
	Global rectangleHeight:Int	= 100
'** End Global variables ***************************************************************************

DrawText "Rectangle 1", 10, HEIGHT -  4 * TextHeight("Rectangle 1") - 40

myDrawRect(RECTANGLESTARTX, RECTANGLESTARTY, rectangleWidth, rectangleHeight)

DrawText "Press [any] key to continue", 10, HEIGHT - 3 * TextHeight("Press [any] key to continue") - 30

Flip

WaitKey

rectangleWidth	= rectangleWidth * xys
rectangleHeight	= rectangleHeight * xys

DrawText "Rectangle 2", 10, HEIGHT - 2 * TextHeight("Rectangle 2") - 20

myDrawRect(RECTANGLESTARTX, RECTANGLESTARTY, rectangleWidth, rectAngleHeight)

DrawText "Press [any] key to quit", 10, HEIGHT - TextHeight("Press [any] key to quit") - 10

FlushMem

Flip

WaitKey

End

'** myDrawRect:Int(myDrawRectX:Float, myDrawRectY:Float, myDrawRectWidth:Float, myDrawRectHeight:Float) **
	
	Function myDrawRect:Int(myDrawRectX:Float, myDrawRectY:Float, myDrawRectWidth:Float, myDrawRectHeight:Float)
		
		DrawLine myDrawRectX, myDrawRectY, myDrawRectX + myDrawRectWidth, myDrawRectY
		DrawLine myDrawRectX, myDrawRectY, myDrawRectX, myDrawRectY + myDrawRectHeight
		DrawLine myDrawRectX + myDrawRectWidth, myDrawRectY, myDrawRectX + myDrawRectWidth, myDrawRectY + myDrawRectHeight
		DrawLine myDrawRectX, myDrawRectY + myDrawRectHeight, myDrawRectX + myDrawRectHeight, myDrawRectY + myDrawRectHeight
	End Function
'** End myDrawRect:Int(myDrawRectX:Float, myDrawRectY:Float, myDrawRectWidth:Float, myDrawRectHeight:Float) **
