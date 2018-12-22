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

DrawText "Hit [Esc] as many times you can in the next five seconds", 10, HEIGHT - TextHeight("Hit [Esc] as many times you can in the next five seconds") - 30

Flip

Delay 5000

Local counter:Int

counter = KeyHit(KEY_ESCAPE)

DrawText "You hit [Esc] " + counter + " times, press [any] key to quit", 10, HEIGHT - TextHeight("You hit [Esc] ") - 10

Flip

FlushKeys

Delay 1000

WaitKey

End
