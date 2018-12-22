'**************************************************
'* demo3.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int				= 800
	Const HEIGHT:Int			= 600
	Const DEPTH:Int				= 0
	Const DOTIMAGEWIDTH:Int		= 100
	Const DOTIMAGEHEIGHT:Int	= 100
	Const DOTIMAGEDOTS:Int		= 1000
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

SeedRnd MilliSecs()

Local counter:Int
Local red:Int
Local green:Int
Local blue:Int
Local alpha:Int					= 255
Local dotImage:TImage			= CreateImage(DOTIMAGEWIDTH, DOTIMAGEHEIGHT)
Local pixmapDotImage:TPixmap	= LockImage(dotImage)

For counter = 0 To DOTIMAGEDOTS
	
	red		= Rnd(255)
	green	= Rnd(255)
	blue	= Rnd(255)
	
	WritePixel pixmapDotImage, Rnd(DOTIMAGEWIDTH), Rnd(DOTIMAGEHEIGHT), alpha Shl 24 + red Shl 16 + green Shl 8 + blue
Next

UnlockImage dotImage

'** Main loop **************************************************************************************
	
	Repeat
		
		TileImage dotImage
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
