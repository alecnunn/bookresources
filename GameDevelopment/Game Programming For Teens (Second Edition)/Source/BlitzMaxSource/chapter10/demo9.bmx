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

'** Types ******************************************************************************************
	
	Type TBackground
		
		Const SPEED:Int = 1
		
		Field y:Int
		Field image:TImage
	EndType
	
	Type TPlayer Extends TBackground
		
		Const SPEED:Int = 5
		
		Field x:Int
	EndType
	
	Type TBullet
		
		Const SPEED:Int			= 7
		Const TYPEBULLET:Int	= 1
		Const TYPELASER:Int		= 2
		
		Field x:Int
		Field y:Int
		Field bulletType:Int
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global leftMousebuttonClicks:Int	= 0
	Global rightMousebuttonClicks:Int	= 0
	Global bulletImage:TImage			= LoadImage("bullet.png")
	Global laserImage:TImage			= LoadImage("laser.png")
	
	Global player:TPlayer			= New TPlayer
	Global background:TBackground	= New TBackground
	Global bulletList:TList			= New TList
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.image		= LoadImage("ship.png")
background.y		= 0
background.image	= LoadImage("stars.png")

SetImageHandle laserImage, ImageWidth(laserImage) / 2, ImageHeight(laserImage)

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		testInput()
		draw()
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End

'** testInput:Int() ********************************************************************************
	
	Function testInput:Int()
		
		player.x	= MouseX()
		player.y	= MouseY()
		
		If MouseHit(KEY_MOUSELEFT) Then
			
			Local bullet:TBullet = New TBullet
			
			bullet.x			= player.x
			bullet.y			= player.y
			bullet.bulletType	= bullet.TYPEBULLET
			
			leftMousebuttonClicks :+ 1
			
			ListAddLast bulletList, bullet
		ElseIf MouseDown(KEY_MOUSERIGHT) Or KeyDown(KEY_CONTROL) And MouseDown(KEY_MOUSELEFT) Then
			
			Local bullet:TBullet = New TBullet
			
			bullet.x			= player.x
			bullet.y			= player.y
			bullet.bulletType	= bullet.TYPELASER
			
			rightMouseButtonClicks :+ 1
			
			ListAddLast bulletList, bullet
		EndIf
	EndFunction
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		TileImage background.image, 0, background.y
		
		updateBackground()
		updateBullets()
		
		DrawImage player.image, player.x, player.y
		
		DrawText "X " + player.x, 10, HEIGHT - TextHeight("X ") - 70
		DrawText "Y " + player.y, 10, HEIGHT - TextHeight("Y ") - 50
		DrawText "Left mousebutton " + leftMousebuttonClicks, 10, HEIGHT - TextHeight("Left mousebutton ") - 30
		DrawText "Right mousebutton " + rightMousebuttonClicks, 10, HEIGHT - TextHeight("Right mousebutton ") - 10
	EndFunction
'** End draw:Int() *********************************************************************************

'** updateBackground:Int() *************************************************************************
	
	Function updateBackground:Int()
		
		background.y :+ background.SPEED
		
		If background.y >= ImageHeight(background.image) Then
			
			background.y = 0
		EndIf
	EndFunction
'** End updateBackground:Int() *********************************************************************

'** updateBullets:Int() ****************************************************************************
	
	Function updateBullets:Int()
		
		Local counter:TBullet = New TBullet
		
		For counter = EachIn bulletList
			
			If counter.bulletType = counter.TYPEBULLET Then
				
				counter.y :- counter.SPEED
				
				If counter.y < -ImageHeight(bulletImage) / 2 Then
					
					ListRemove bulletList, counter
				Else
					
					DrawImage bulletImage, counter.x, counter.y
				EndIf
			ElseIf counter.bulletType = counter.TYPELASER Then
				
				If counter.x <> player.x Then
					
					ListRemove bulletlist, counter
				Else
					
					DrawImage laserImage, counter.x, counter.y
				EndIf
			EndIf
		Next
	EndFunction
'** End updateBullets:Int() ************************************************************************
